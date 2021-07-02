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
const std::string categoryBox = R"xml(
    <brls:Label
        id="browse_submenu_category_label"
        shrink="0.5"
        text="Category"
        width="100%"
        height="35"
        textColor="#000000"
        focusable="true"
    />
)xml";
const std::string browseSubmenuContainerBox = R"xml(
    <brls:Box
        id="browse_submenu_container"
        width="100%"
        height="90%"
        justifyContent="spaceBetween"
        axis="column"
        paddingTop="5"
        paddingBottom="5"
        paddingRight="5"
        paddingLeft="5"
    />
)xml";


Browse::Browse() {
    brls::Logger::debug("Initializing browse menu...");
    this->pages = new brls::LayerView();
    this->inflateFromXMLRes("xml/tabs/browse.xml");
    //this->loadCategoryFilters();
    this->loadPage(this->current_page);
    ((brls::Box*)this->getView("browse_box"))->addView(this->pages);

    /*this->registerAction(
        "Submenu", brls::ControllerButton::BUTTON_Y, [this](brls::View* view) {
            this->toggleSubmenu();
            return false;
        },
        false, brls::Sound::SOUND_FOCUS_SIDEBAR);*/

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
        
    delete this->pages;
}

void Browse::getNewGbSubmissions(int page, int category) {

    // vector of NUM_SUBMISSIONS_PER_PAGE elements
    gb::GbSubmissions* new_subs;
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
        brls::Logger::error("Found 0 new submissions... is something broken?");
    }
    delete new_subs;
}


void Browse::loadPage(int page, int category) {
    this->getNewGbSubmissions(page, category);

    int submission_index = 0;
    // start idx of the submissions vector to pull the submissions for this page from
    int start_idx = (NUM_SUBMISSIONS_PER_PAGE * page) - NUM_SUBMISSIONS_PER_PAGE;

    brls::Box* this_page = (brls::Box*)this->createFromXMLString(basicBox);
    this->pages->addLayer(this_page);

    CURL_builder curl;

    for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {
        brls::Box* horzHolderBox = (brls::Box*)this->createFromXMLString(horzHolder);
        this_page->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {

            SubmissionNode* submission_node;
            int curr_idx = start_idx + submission_index;
            if (curr_idx >= this->subs.size()) // if we have less than 9 mods to load return early
                return;

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
                        if (!thumbnail_url.empty()) {
                            MemoryStruct s = curl::DownloadToMem(thumbnail_url, &curl);
                            if (s.memory != nullptr && s.size > 0)
                                submission_image->setImageFromMem((unsigned char*)s.memory, s.size);
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
}


void Browse::scroll(brls::FocusDirection dir) {
    brls::Label* page_right = (brls::Label*)this->getView("arrow_right_label");
    brls::Label* page_left = (brls::Label*)this->getView("arrow_left_label");
    switch (dir) {
        case brls::FocusDirection::LEFT:

            if (this->current_page > 1) {
                this->current_page -= 1;
                //this->loadPage(this->current_page);
                this->pages->changeLayer(this->pages->getLayerIndex()-1);
            }

            break;
        case brls::FocusDirection::RIGHT:

            this->current_page += 1;
            if (this->current_page > this->pages->getLayersSize()) {
                this->loadPage(this->current_page);
            }
            this->pages->changeLayer(this->pages->getLayerIndex()+1);

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
==========================================================================================
*/





/*
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
*/

Category* recurseCatSearch(Category* rootToSearch, int parentIdToSearch) {
    if (rootToSearch->subcategories.empty()) { // base case to handle "leaves"
        if (rootToSearch->id == parentIdToSearch)
            return rootToSearch;
        else
            return nullptr;
    }
    else {
        for (Category* x : rootToSearch->subcategories) { // iterate through non-leaf cats
            Category* ret = recurseCatSearch(x, parentIdToSearch);
            if (ret != nullptr)
                return ret;
        }
        if (rootToSearch->id == parentIdToSearch) // check non-leaf cat
            return rootToSearch;
        else
            return nullptr;

    }
}

Category* recurseCatSearchHelper(std::map<int, Category*>* mapToSearch, int parentIdToSearch) {
    for (auto &[key, val] : *mapToSearch) {
        Category* c = recurseCatSearch(val, parentIdToSearch);
        if (c != nullptr)
            return c;
    }
    return nullptr;
}

bool Browse::category_action_inner(brls::View* view, const std::map<int, Category*>& categories, brls::Box* browse_submenu_container, brls::Box* browse_submenu, brls::ActionListener category_action) {
    brls::Logger::debug("Category action...");
    int view_id = std::stoi(view->getID());
    brls::Logger::debug("Category clickde! {}", view_id);
    if (!categories.at(view_id)->subcategories.empty()) {
        brls::Logger::debug("Has subcats");
        browse_submenu->removeView(browse_submenu_container);
        brls::Box* browse_submenu_container_new = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);
        *browse_submenu_container = *browse_submenu_container_new;
        //browse_submenu->addView(browse_submenu_container_new);
        if (categories.count(view_id)) {
            brls::Logger::debug("Adding new subcats");
            std::vector<Category*> subcategories = categories.at(view_id)->subcategories;
            for (Category* cat : subcategories) {
                brls::Label* v = (brls::Label*)brls::View::createFromXMLString(categoryBox);
                v->setId(std::to_string(view_id));
                v->setText(cat->name);
                v->registerClickAction(category_action);
                browse_submenu_container->addView(v);
            }
            brls::Logger::debug("Finished iterations");
            brls::Application::giveFocus(browse_submenu);
        }
        
    }
    else {
        brls::Logger::debug("No subcats");
        this->toggleSubmenu();
    }
    
    return false;
}

bool submenu_back(brls::View* view) {
    brls::Logger::debug("Submenu back");
    return false;
}

void Browse::loadCategoryFilters() {
    //      init map

    // dl first page of categories
    std::string init_category_url = "https://gamebanana.com/apiv3/ModCategory/ByGame?_aGameRowIds[]=6498&_sRecordSchema=Custom&_csvProperties=_idRow,_sName,_sIconUrl,_idParentCategoryRow&_nPerpage=50&_bReturnMetadata=true&_nPage=1";
    json categories_json = curl::DownloadJson(init_category_url);

    std::string subsequent_category_url = "https://gamebanana.com/apiv3/ModCategory/ByGame?_aGameRowIds[]=6498&_sRecordSchema=Custom&_csvProperties=_idRow,_sName,_sIconUrl,_idParentCategoryRow&_nPerpage=50&_nPage=";
    CURL_builder curl;

    // loop through all other pages of categories and append them to json
    for (int i = 2; i <= categories_json[gb::Fields::Metadata::Metadata][gb::Fields::Metadata::PageCount].get<int>(); i++) {
        json category_json_page = curl::DownloadJson(subsequent_category_url + std::to_string(i), &curl);
        for (json j : category_json_page)
            categories_json[gb::Fields::Records] += j;
    }
    categories_json.erase(gb::Fields::Metadata::Metadata); // don't need it anymore
    categories_json = categories_json[gb::Fields::Records];

    // parent category - category info
    std::map<int, Category*> categories;
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
                .subcategories = {}
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

    //      init views

    brls::Box* browse_submenu_holder = (brls::Box*)this->getView("browse_menu_box");
    brls::LayerView* browse_layerview = (brls::LayerView*)browse_submenu_holder->getView("browse_menu_layerview");

    brls::Box* browse_submenu = (brls::Box*)brls::View::createFromXMLResource("views/browse_submenu.xml");
    browse_layerview->addLayer(browse_submenu);

    brls::Box* browse_submenu_container = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);
    ((brls::ScrollingFrame*)this->getView("browse_submenu_scrollingframe"))->addView(browse_submenu_container);

    brls::ActionListener category_action = [this, categories, browse_submenu_container, browse_submenu, category_action](brls::View* view) {
        return category_action_inner(view, categories, browse_submenu_container, browse_submenu, category_action);
    };

    for (const auto &[key, value] : categories) {
        brls::Label* v = (brls::Label*)brls::View::createFromXMLString(categoryBox);
        v->setId(std::to_string(key));
        v->setText(value->name);
        v->registerClickAction(category_action);
        //v->registerAction("Submenu Back", brls::ControllerButton::BUTTON_B, submenu_back, false, brls::Sound::SOUND_FOCUS_CHANGE);
        browse_submenu_container->addView(v);
    }
}





void Browse::toggleSubmenu() {
    brls::Box* browse_submenu_holder = (brls::Box*)this->getView("browse_menu_box");
    if (browse_submenu_holder->getVisibility() != brls::Visibility::VISIBLE) {
        browse_submenu_holder->setVisibility(brls::Visibility::VISIBLE);
        brls::Application::giveFocus(browse_submenu_holder);
        brls::Logger::debug("Toggle submenu on!");
    }
    else {
        brls::Application::giveFocus(this);
        browse_submenu_holder->setVisibility(brls::Visibility::GONE);
        brls::Logger::debug("Toggle submenu off!");
    }
}