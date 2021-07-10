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
        justifyContent="flexStart"
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

void Browse::ResetPages() {
    brls::Box* browse_box = ((brls::Box*)this->getView("browse_box"));
    //for (brls::View* v : browse_box->getChildren())
    //    browse_box->removeView(v);
    //this->pages = new brls::LayerView();
    //browse_box->addView(this->pages); 
    this->pages->clearLayers();

    for (gb::GbSubmission* sub : this->subs) {
        delete sub;
    }
    this->subs.clear();

    this->current_page = 1;

    this->loadPage(this->current_page, this->current_category_filter);

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
Category* recurseCatSearchHelper(std::map<int, Category*>* mapToSearch, int idToSearch) {
    for (auto &[key, val] : *mapToSearch) {
        Category* c = recurseCatSearch(val, idToSearch);
        if (c != nullptr)
            return c;
    }
    return nullptr;
}
// ----------------------------------------------------------------------

brls::ActionListener category_action = [](brls::View* v) { brls::Logger::debug("Category action empty!"); return false; };
brls::ActionListener submenu_back = [](brls::View* v) { brls::Logger::debug("Submenu back action empty!"); return false; };


bool Browse::show_submenu_category(brls::View* view, int curr_category) {
    brls::Application::giveFocus(nullptr);
    //brls::LayerView* browse_layerview = (brls::LayerView*)this->getView("browse_menu_layerview");
    brls::Box* browse_layerview = (brls::Box*)this->getView("browse_menu_layerview");

    //brls::Box* browse_submenu = (brls::Box*)browse_layerview->getLayer("browse_submenu_scrollingframe");
    brls::Box* browse_submenu = (brls::Box*)browse_layerview->getView("browse_submenu_scrollingframe");
    brls::Box* browse_submenu_container = (brls::Box*)browse_submenu->getView("browse_submenu_container");

    //int curr_category = std::stoi(view->getID());
    Category* selected_category = recurseCatSearchHelper(&this->categories, curr_category);
    brls::Logger::debug("Category {}", curr_category);
    if (selected_category) {

        if (!selected_category->subcategories.empty()) {
            brls::Application::giveFocus(browse_submenu);

            browse_submenu->removeView(browse_submenu_container);

            brls::Box* browse_submenu_container_new = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);
            browse_submenu->addView(browse_submenu_container_new);

            std::vector<Category*> categories_to_iterate = selected_category->subcategories;
            if (curr_category == 0) {
                categories_to_iterate.clear();
                for (const auto &[key, value] : this->categories) {
                    categories_to_iterate.push_back(value);
                }
            }
            
            for (Category* cat : categories_to_iterate) {
                brls::Label* v = (brls::Label*)brls::View::createFromXMLString(categoryBox);
                v->setId(std::to_string(cat->id));
                v->setText(cat->name);
                v->registerClickAction(category_action);
                v->registerAction("Submenu Back", brls::ControllerButton::BUTTON_B, submenu_back, false, brls::Sound::SOUND_FOCUS_CHANGE);
                browse_submenu_container_new->addView(v);
            }
            this->curr_parent_category = selected_category->parentCatId;
        }
        else {
            brls::Logger::debug("No subcats");
            this->current_category_filter = curr_category;
            this->toggleSubmenu();
            this->ResetPages();
        }

    }
    
    return false;
}


void Browse::submenu_back_inner(brls::View* v) {
    int curr_category = std::stoi(v->getID());
    brls::Logger::debug("Submenu back. [ViewID={}] [Current parent={}]", curr_category, this->curr_parent_category);

    this->show_submenu_category(v, this->curr_parent_category);

}


void Browse::loadCategoryFilters() {
    //      init map

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

    /*for (const auto &[key, value] : categories) {
        brls::Logger::debug("Root: {}, id: {}", value->name, value->id);
        printSubCats(value);
    }*/

    //      init views

    brls::Box* browse_layerview = (brls::Box*)this->getView("browse_menu_layerview");
    brls::Box* browse_submenu = (brls::Box*)brls::View::createFromXMLResource("views/browse_submenu.xml");
    browse_layerview->addView(browse_submenu);

    brls::Box* browse_submenu_container = (brls::Box*)brls::View::createFromXMLString(browseSubmenuContainerBox);
    ((brls::Box*)this->getView("browse_submenu_scrollingframe"))->addView(browse_submenu_container);

    category_action = [this](brls::View* v) {
        return this->show_submenu_category(v, std::stoi(v->getID()));
    };
    submenu_back = [this](brls::View* v) {
        brls::Logger::debug("HERE");
        this->submenu_back_inner(v);
        return false;
    };

    for (const auto &[key, value] : categories) {
        brls::Label* v = (brls::Label*)brls::View::createFromXMLString(categoryBox);
        v->setId(std::to_string(key));
        v->setText(value->name);
        v->registerClickAction(category_action);
        v->setCustomNavigationRoute(brls::FocusDirection::LEFT, nullptr);
        v->setCustomNavigationRoute(brls::FocusDirection::RIGHT, nullptr);
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
        browse_submenu_holder->setVisibility(brls::Visibility::INVISIBLE);
        brls::Logger::debug("Toggle submenu off!");
    }
}