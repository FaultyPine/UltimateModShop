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
    this->inflateFromXMLRes("xml/tabs/browse.xml");
    this->loadNextPage();
    ((brls::Box*)this->getView("browse_box"))->addView(this->pages);

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

void Browse::getNewGbSubmissions(int page) {
    // vector of NUM_SUBMISSIONS_PER_PAGE elements
    gb::GbSubmissions* new_subs = gb::GetNewSubmissions(page, NUM_SUBMISSIONS_PER_PAGE);
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


void Browse::loadNextPage() {

    if (!REDUCED_NET_REQUESTS)
        this->getNewGbSubmissions(this->current_page);

    int submission_index = 0;
    // start idx of the submissions vector to pull the submissions for this page from
    int start_idx = (NUM_SUBMISSIONS_PER_PAGE * this->current_page) - NUM_SUBMISSIONS_PER_PAGE;

    brls::Box* this_page = (brls::Box*)this->createFromXMLString(basicBox);
    this->pages->addLayer(this_page);

    for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {
        brls::Box* horzHolderBox = (brls::Box*)this->createFromXMLString(horzHolder);
        this_page->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {

            SubmissionNode* submission_node;

            if (!this->subs.empty()) {
                gb::GbSubmission* sub = this->subs.at(start_idx + submission_index);

                submission_node = new SubmissionNode(sub);

                if (!sub->submission_data.empty()) {
                    brls::Image* submission_image = (brls::Image*)submission_node->getView("submission_image");
                    brls::Label* submission_label = (brls::Label*)submission_node->getView("submission_label");
                    std::string itemid = gb::getItemIdFromProfileURL(sub->submission_data[gb::Fields::ProfileURL].get<std::string>());
                    sub->submission_data[gb::Fields::idRow] = itemid;

                    std::string submission_title = sub->submission_data[gb::Fields::Name].get<std::string>();
                    submission_label->setText(submission_title);

                    // download image to memory
                    std::string thumbnail_url = gb::Fields::PreviewMedia::BaseURL + sub->submission_data[gb::Fields::PreviewMedia::PreviewMedia][0][gb::Fields::PreviewMedia::File220].get<std::string>();
                    sub->submission_data[gb::Fields::Custom::ThumbnailURL] = thumbnail_url;
                    if (!thumbnail_url.empty()) {
                        MemoryStruct s = curl::DownloadToMem(thumbnail_url);
                        if (s.memory != nullptr && s.size > 0)
                            submission_image->setImageFromMem((unsigned char*)s.memory, s.size);
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
                //this->loadNextPage();
                this->pages->changeLayer(this->pages->getLayerIndex()-1);
            }

            break;
        case brls::FocusDirection::RIGHT:

            this->current_page += 1;
            if (this->current_page > this->pages->getLayersSize()) {
                this->loadNextPage();
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