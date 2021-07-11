#include "browse.h"

brls::Box* Browse::create() {
    return new Browse();
}

const std::string horzHolder = R"xml(
    <brls:Box 
        id="horzHolder"
        axis="row"
        width="100%"
        height="25%"
        marginBottom="20"
        marginTop="20"
    />
)xml";
const std::string basicBox = R"xml(
    <brls:Box 
        width="100%"
        height="100%"
        axis="column"
    />
)xml";


Browse::Browse() {
    brls::Logger::debug("Initializing browse menu...");
    this->pages = new brls::LayerView();
    this->inflateFromXMLRes("xml/tabs/browse/browse.xml");
    this->loadCategoryFilters();
    this->loadPage(this->current_page);
    ((brls::Box*)this->getView("browse_box"))->addView(this->pages);

    this->registerAction(
        "Submenu", brls::ControllerButton::BUTTON_Y, [this](brls::View* view) {
            this->toggleSubmenu();
            return false;
        },
        false, brls::Sound::SOUND_FOCUS_SIDEBAR);

    this->registerAction(
        "Page Right", brls::ControllerButton::BUTTON_RSB, [this](brls::View* view) { 
            this->scroll(brls::FocusDirection::RIGHT);
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_SIDEBAR);

    this->registerAction(
        "Page Left", brls::ControllerButton::BUTTON_LSB, [this](brls::View* view) {
            this->scroll(brls::FocusDirection::LEFT);
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_SIDEBAR);
}

Browse::~Browse() {
    for (gb::GbSubmission* e : this->subs) {
        delete e;
    }
    for (const auto &[key, value] : categories) {
        delete value;
    }
        
    delete this->pages;
}

void Browse::getNewGbSubmissions(int page, int category) {
    // vector of NUM_SUBMISSIONS_PER_PAGE elements
    gb::GbSubmissions* new_subs = nullptr;
    if (category != -1)
        new_subs = gb::GetSubmissionsFromCategory(page, category, NUM_SUBMISSIONS_PER_PAGE);
    else
        new_subs = gb::GetNewSubmissions(page, NUM_SUBMISSIONS_PER_PAGE);

    if (!new_subs->empty()) {
        // push submission data onto vec
        for (gb::GbSubmission* e : *new_subs) {
            this->subs.push_back(e);
        }
    }
    else {
        brls::Logger::error("Found 0 new submissions...");
    }
    delete new_subs;
}


bool Browse::loadPage(int page, int category) {
    this->getNewGbSubmissions(page, category);

    int submission_index = 0;
    // start idx of the submissions vector to pull the submissions for this page from
    int start_idx = (NUM_SUBMISSIONS_PER_PAGE * page) - NUM_SUBMISSIONS_PER_PAGE;
    if (start_idx >= this->subs.size())
        return false;

    brls::Box* this_page = (brls::Box*)this->createFromXMLString(basicBox);
    this->pages->addLayer(this_page);

    CURL_builder curl = CURL_builder();

    for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {
        brls::Box* horzHolderBox = (brls::Box*)this->createFromXMLString(horzHolder);
        this_page->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {

            int curr_idx = start_idx + submission_index;
            if (curr_idx >= this->subs.size()) // if we have less than 9 mods to load return early
                return true;

            SubmissionNode* submission_node = nullptr;

            if (!this->subs.empty()) {

                gb::GbSubmission* sub = this->subs.at(curr_idx);

                submission_node = new SubmissionNode(sub);

                if (!sub->submission_data.empty()) {
                    brls::Image* submission_image = (brls::Image*)submission_node->getView("submission_image");
                    brls::Label* submission_label = (brls::Label*)submission_node->getView("submission_label");
                    std::string itemid = gb::getItemIdFromProfileURL(sub->submission_data[gb::Fields::ProfileURL].get<std::string>());
                    sub->submission_data[gb::Fields::idRow] = itemid;

                    std::string submission_title = sub->submission_data[gb::Fields::Name].get<std::string>();
                    submission_label->setText(submission_title);

                    // download image to memory
                    if (!REDUCED_NET_REQUESTS) {
                        std::string thumbnail_url = gb::Fields::PreviewMedia::BaseURL + sub->submission_data[gb::Fields::PreviewMedia::PreviewMedia][0][gb::Fields::PreviewMedia::File220].get<std::string>();
                        sub->submission_data[gb::Fields::Custom::ThumbnailURL] = thumbnail_url;
                        if (!thumbnail_url.empty() && !strHasEnding(thumbnail_url, ".webp")) {
                            MemoryStruct s = curl::DownloadToMem(thumbnail_url, &curl);
                            if (s.memory != nullptr && s.size > 0) {
                                submission_image->setImageFromMem((unsigned char*)s.memory, s.size);
                            }
                        }
                    }
                    

                }
            }
            else {
                submission_node = new SubmissionNode();
            }
            
            // handle navigation between SubmissionNodes
            submission_node->setId( std::to_string( submission_index ) );

            int scroll_down_idx  =  (submission_index+NUM_SUBMISSIONS_ROOT) % NUM_SUBMISSIONS_PER_PAGE;
            int scroll_up_idx    =  submission_index-NUM_SUBMISSIONS_ROOT < 0 ? NUM_SUBMISSIONS_PER_PAGE-(sqrt(NUM_SUBMISSIONS_PER_PAGE))+submission_index : submission_index-NUM_SUBMISSIONS_ROOT;
            //int scroll_right_idx =  ((submission_index+1) % NUM_SUBMISSIONS_ROOT) == 0 ? (submission_index-(NUM_SUBMISSIONS_ROOT-1)) : submission_index+1;
            //int scroll_left_idx  =  (submission_index % NUM_SUBMISSIONS_ROOT) == 0 ? (submission_index+(NUM_SUBMISSIONS_ROOT-1)) : submission_index-1;

            submission_node->setCustomNavigationRoute(brls::FocusDirection::DOWN,  std::to_string( scroll_down_idx  ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::UP,    std::to_string( scroll_up_idx    ));
            //submission_node->setCustomNavigationRoute(brls::FocusDirection::RIGHT, std::to_string( scroll_right_idx ));
            //submission_node->setCustomNavigationRoute(brls::FocusDirection::LEFT,  std::to_string( scroll_left_idx  ));

            submission_index += 1;
            horzHolderBox->addView(submission_node);
        }
    }
    return true;
}


void Browse::scroll(brls::FocusDirection dir) {
    brls::Application::giveFocus(nullptr);
    brls::Label* page_right = (brls::Label*)this->getView("arrow_right_label");
    brls::Label* page_left = (brls::Label*)this->getView("arrow_left_label");
    bool should_scroll = true;
    switch (dir) {
        case brls::FocusDirection::LEFT:

            if (this->current_page > 1) {
                this->current_page -= 1;
                this->pages->changeLayer(this->pages->getLayerIndex()-1);
            }

            break;
        case brls::FocusDirection::RIGHT:

            this->current_page += 1;
            if (this->current_page > this->pages->getLayersSize()) {
                should_scroll = this->loadPage(this->current_page, this->current_category_filter);
            }

            if (should_scroll)
                this->pages->changeLayer(this->pages->getLayerIndex()+1);
            else
                this->current_page -= 1;

            break;
        default:
            brls::Logger::error("Cannot scroll up/down in browse menu!");
    }
    setHintText("Page: " + std::to_string(this->current_page));
    brls::Application::giveFocus(this->pages->getDefaultFocus());
}


void Browse::onChildFocusGained(View* directChild, View* focusedView) {
    std::string current_id = focusedView->getID();
    if (current_id == "arrow_left") {
        // if we just scrolled left on the left-most submission
        this->scroll(brls::FocusDirection::LEFT);
    }
    else if (current_id == "arrow_right") {
        // if we just scrolled right on the right-most submission
        this->scroll(brls::FocusDirection::RIGHT);
    }
    else {
        Box::onChildFocusGained(directChild, focusedView);
    }
}



/*
===========================================================================================
                                    Category Filtering


-- using a naive implementation here...
My initial thought was to use a BST, but wasn't feeling like implementing one since
stl doesn't provide one. To get the categories, we request from the gb api, and unfortunately,
the gb api just gives us a massive list of all the categories and their parent.
I had an idea for properly sorting them without lots of recursive iterations, but just got
mentally tired around the time of trying to implement this, so right now this is a naive impl
where i just recurse a bunch of times to find everything. It all gets put into a map 
where each item of the map is the "root" category, and each cateogry (root or not) contains info about
that category along with any of their subcategories
Since none of the gamebanana categories have more than like 2-3 subcategories, this isn't absolutely abysmal lol
==========================================================================================
*/

const std::string browseSubmenuContainerBox = R"xml(
    <brls:Box
        id="browse_submenu_container"
        width="100%"
        height="90%"
        justifyContent="flexStart"
        axis="column"
        paddingTop="5"
        paddingBottom="5"
        paddingRight="5"
        paddingLeft="5"
    />
)xml";




void printSubCats(Category* c, int i = 0) {
    if (!c->subcategories.empty()) {
        for (Category* x : c->subcategories) {
            std::string indent = "";
            for (int b = 0; b < i; b++)
                indent += "\t";
            brls::Logger::debug("{}[Name: '{}', id: {}, parentCatId: {}", indent, x->name, x->id, x->parentCatId);
            printSubCats(x, i+1);
        }    
    }
}

// ----------------------------------------------------------------------- Searching for an id within all categories
Category* recurseCatSearch(Category* rootToSearch, int idToSearch) {
    if (rootToSearch->subcategories.empty()) { // base case to handle "leaves"
        if (rootToSearch->id == idToSearch)
            return rootToSearch;
        else
            return nullptr;
    }
    else {
        for (Category* x : rootToSearch->subcategories) { // iterate through non-leaf cats
            Category* ret = recurseCatSearch(x, idToSearch);
            if (ret != nullptr)
                return ret;
        }
        if (rootToSearch->id == idToSearch) // check non-leaf cat
            return rootToSearch;
        else
            return nullptr;

    }
}
Category* recurseCatSearchHelper(CategoryMap* mapToSearch, int idToSearch) {
    for (auto &[key, val] : *mapToSearch) {
        Category* c = recurseCatSearch(val, idToSearch);
        if (c != nullptr)
            return c;
    }
    return nullptr;
}
// ----------------------------------------------------------------------


brls::ActionListener onClickCategoryFilter = [](brls::View* v) { brls::Logger::warning("Blank onClickCategoryFilter action! {}", v->describe()); return false; };
brls::ActionListener onSubmenuBack = [](brls::View* v) { brls::Logger::warning("Blank onSubmenuBack action! {}", v->describe()); return false; };


void Browse::onClickCategoryFilterInner(brls::View* view) {
    brls::Logger::debug("onClickCategoryFilter: {}", view->describe());

    int category_id = std::stoi(view->getID());
    Category* cat = recurseCatSearchHelper(&this->categories, category_id);
    brls::LayerView* l = (brls::LayerView*)this->getView("browse_submenu_content");

    bool has_subcats = cat->childLayerIdx >= 0;
    if (has_subcats) {
        l->changeLayer(cat->childLayerIdx, true);
    }
    else {
        l->changeLayer(0, false);
        this->current_page = 1;
        this->current_category_filter = category_id;

        for (gb::GbSubmission* sub : this->subs) {
            delete sub;
        }
        this->subs.clear();
        this->pages->clearLayers();

        this->toggleSubmenu();
        this->loadPage(this->current_page, this->current_category_filter);
        setTopText("Category: " + cat->name);

    }

}

void Browse::onSubmenuBackInner(brls::View* view) {
    brls::Logger::debug("onSubmenuBackInner: {}", view->describe());

    int category_id = std::stoi(view->getID());
    Category* cat = recurseCatSearchHelper(&this->categories, category_id);
    brls::LayerView* l = (brls::LayerView*)this->getView("browse_submenu_content");

    l->changeLayer(cat->parentLayerIdx, true);
}

brls::Box* createViewFromCategory(Category* cat, CURL_builder* curl, bool is_root) {
    brls::Box* v = (brls::Box*)brls::View::createFromXMLResource("tabs/browse/browse_submenu_category.xml");
    brls::Label* lab = (brls::Label*)v->getView("browse_submenu_category_label");
    brls::Image* img = (brls::Image*)v->getView("browse_submenu_category_image");

    /*if (!cat->iconUrl.empty()) {
        MemoryStruct img_mem = curl::DownloadToMem(cat->iconUrl, curl);
        if (img_mem.memory != nullptr && img_mem.size > 0)
            img->setImageFromMem((unsigned char*)img_mem.memory, img_mem.size);
    }*/

    lab->setText(cat->name);

    v->setId(std::to_string(cat->id));
    v->setCustomNavigationRoute(brls::FocusDirection::LEFT, nullptr);
    v->setCustomNavigationRoute(brls::FocusDirection::RIGHT, nullptr);
    v->registerClickAction(onClickCategoryFilter);
    if (!is_root) {
        v->registerAction("Submenu back", brls::ControllerButton::BUTTON_B, onSubmenuBack, false, brls::Sound::SOUND_FOCUS_CHANGE);
    }

    return v;
}

void recursiveCategorySetup(const std::vector<Category*>& cats, CURL_builder* curl, brls::LayerView* browse_submenu_layerview, brls::Box* browse_submenu_container) {
    for (Category* cat : cats) {
        brls::Box* v = createViewFromCategory(cat, curl, false);
        browse_submenu_container->addView(v);

        if (!cat->subcategories.empty()) {
            brls::Box* browse_submenu_container_new = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);
            cat->childLayerIdx = browse_submenu_layerview->getLayersSize();
            browse_submenu_layerview->addLayer(browse_submenu_container_new);
            recursiveCategorySetup(cat->subcategories, curl, browse_submenu_layerview, browse_submenu_container_new);
        }
        
    }
}


void Browse::loadCategoryFilters() {
    //      ----- init map -----

    // dl first page of categories
    std::string init_category_url = "https://gamebanana.com/apiv3/ModCategory/ByGame?_aGameRowIds[]=6498&_sRecordSchema=Custom&_csvProperties=_idRow,_sName,_sIconUrl,_idParentCategoryRow&_nPerpage=50&_bReturnMetadata=true&_nPage=1";
    json categories_json = curl::DownloadJson(init_category_url);

    std::string subsequent_category_url = "https://gamebanana.com/apiv3/ModCategory/ByGame?_aGameRowIds[]=6498&_sRecordSchema=Custom&_csvProperties=_idRow,_sName,_sIconUrl,_idParentCategoryRow&_nPerpage=50&_nPage=";
    CURL_builder curl = CURL_builder();

    // loop through all other pages of categories and append them to json
    for (int i = 2; i <= categories_json[gb::Fields::Metadata::Metadata][gb::Fields::Metadata::PageCount].get<int>(); i++) {
        json category_json_page = curl::DownloadJson(subsequent_category_url + std::to_string(i), &curl);
        for (json j : category_json_page)
            categories_json[gb::Fields::Records] += j;
    }
    categories_json.erase(gb::Fields::Metadata::Metadata); // don't need it anymore
    categories_json = categories_json[gb::Fields::Records];

    // parent category - category info
    while (!categories_json.empty()) {
        for (int i = 0; i < categories_json.size(); i++) {
            json category_json = categories_json[i];
            int parent_id = category_json[gb::Fields::Category::ParentCategory].get<int>();
            int id = category_json[gb::Fields::idRow].get<int>();
            Category* this_category = new Category {
                .name = category_json[gb::Fields::Name].get<std::string>(),
                .id = id,
                .parentCatId = parent_id,
                .iconUrl = category_json[gb::Fields::IconURL].get<std::string>(),
                .subcategories = {},
                .childLayerIdx = -1,
                .parentLayerIdx = 0,
            };

            // root category
            if (parent_id == 0) {
                categories[id] = this_category;
                categories_json.erase(i);
            }
            // subcategory
            else if (categories.count(parent_id)) {
                categories[parent_id]->subcategories.push_back(this_category);
                categories_json.erase(i);
            }
            // nested subcategory
            else {
                Category* c = recurseCatSearchHelper(&categories, parent_id);
                if (c != nullptr) {
                    c->subcategories.push_back(this_category);
                    categories_json.erase(i);
                }
            }
        }
    }


    //      ----- init views -----


    // add in browse_submenu.xml
    brls::Box* browse_submenu_root = (brls::Box*)this->getView("browse_menu_root");
    brls::Box* browse_submenu = (brls::Box*)brls::View::createFromXMLResource("tabs/browse/browse_submenu.xml");
    browse_submenu_root->addView(browse_submenu);

    // create browse 
    brls::Box* browse_submenu_container = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);

    // add root category container to layerview
    brls::LayerView* browse_submenu_layerview = (brls::LayerView*)this->getView("browse_submenu_content");
    browse_submenu_layerview->addLayer(browse_submenu_container);


    onClickCategoryFilter = [this](brls::View* v) { this->onClickCategoryFilterInner(v); return false; };
    onSubmenuBack =         [this](brls::View* v) { this->onSubmenuBackInner(v); return false; };

    // set up root/subcategory layerviews
    for (const auto &[key, value] : categories) {
        bool has_subcats = !value->subcategories.empty();
        brls::Box* v = createViewFromCategory(value, &curl, true);
        browse_submenu_container->addView(v);

        if (has_subcats) { // if this category has subcats, add them in recursively
            brls::Box* browse_submenu_container_new = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);
            value->childLayerIdx = browse_submenu_layerview->getLayersSize();
            browse_submenu_layerview->addLayer(browse_submenu_container_new);
            recursiveCategorySetup(value->subcategories, &curl, browse_submenu_layerview, browse_submenu_container_new);
        }
    }

}



void Browse::toggleSubmenu() {
    brls::View* browse_submenu_holder = (brls::View*)this->getView("browse_menu_root");
    if (browse_submenu_holder->getVisibility() != brls::Visibility::VISIBLE) {
        browse_submenu_holder->setVisibility(brls::Visibility::VISIBLE);
        brls::Application::giveFocus(browse_submenu_holder);
    }
    else {
        brls::Application::giveFocus(this);
        browse_submenu_holder->setVisibility(brls::Visibility::INVISIBLE);
    }
}