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


const int NUM_SUBMISSIONS_ROOT = 3;
const int NUM_SUBMISSIONS_PER_PAGE = NUM_SUBMISSIONS_ROOT * NUM_SUBMISSIONS_ROOT;

Browse::Browse() {
    brls::Logger::debug("Initializing browse menu...");
    this->inflateFromXMLRes("xml/tabs/browse.xml");
    if (!REDUCED_NET_REQUESTS)
        this->getNewGbSubmissions(1);
    Browse::initBrowseMenu();
}

Browse::~Browse() {
    for (gb::GbSubmission* sub : this->new_submissions) {
        if (sub)
            delete sub;
    }
}

void Browse::getNewGbSubmissions(int page) {
    gb::GbSubmissions subs = gb::GetNewSubmissions(page);
    if (subs.empty()) {
        brls::Logger::error("Found 0 new submissions... is something broken?");
    }
    this->new_submissions = subs;
}


/**     _______________________
 *      |  0   |   1   |   2  |
 *      |______|_______|______|
 *      |  3   |   4   |   5  |     <- submissions are indexed like this
 *      |______|_______|______|
 *      |  6   |   7   |   8  |
 *      |______|_______|______|
 */


/**
 * pseudo scratchpad
 * 
 * 
 * 
 * queue for GbSubmissions
 * queue for pages (max amount of items in this queue at any time will be NUM_PRELOADED_PAGES)
 *     ^ provide interface for adding/getting items that ensures it never goes above ^
 * 
 */

// TODO: seperate this func into two - one that handles making the horzHolders and setting navigation, and another that handles setting img/labels from gb
void Browse::initBrowseMenu() {

    gb::GetSubmissionDataMulticall(&(this->new_submissions), {gb::Fields::Files, gb::Fields::Thumbnail, gb::Fields::Title, gb::Fields::Author, gb::Fields::NumUpdates});

    int submission_index = 0;
    
    for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {
        brls::Box* horzHolderBox = (brls::Box*)this->createFromXMLString(horzHolder);
        ((brls::Box*)this->getView("browse_box"))->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROOT; i++) {

            gb::GbSubmissions new_subs = this->new_submissions;
            SubmissionNode* submission_node;
            if (!new_subs.empty()) {
                gb::GbSubmission* sub = new_subs.at(submission_index);
                submission_node = new SubmissionNode(sub);

                brls::Image* submission_image = (brls::Image*)submission_node->getView("submission_image");
                brls::Label* submission_label = (brls::Label*)submission_node->getView("submission_label");

                if (!sub->submission_data.empty()) {
                    std::string submission_title = sub->submission_data[gb::Fields::Title].get<std::string>();
                    submission_label->setText(submission_title);

                    // download image to memory
                    std::string thumbnail_url = sub->submission_data[gb::Fields::Thumbnail].get<std::string>();
                    if (thumbnail_url != gb::GB_NO_THUMBNAIL_URL) { // submissions without images will get this url for their thumbnail... stuff like sound clips and the like. The only one out of the AllowedItemTypes that has this is Sound rn
                        MemoryStruct s = curl::DownloadToMem(thumbnail_url);
                        submission_image->setImageFromMem((unsigned char*)s.memory, s.size);
                    }
                    else if (sub->itemtype == "Sound") {
                        submission_image->setImageFromRes("icon/sound_submission.png");
                    }
                }
            }
            else {
                submission_node = new SubmissionNode();
            }
            

            submission_node->setId( std::to_string( submission_index ) );

            /*  ---- Scrolling where going up will loop back down and going to the side will loop to the other side -----
            int scroll_down_idx = (submission_index+NUM_SUBMISSIONS_ROOT) % NUM_SUBMISSIONS_PER_PAGE;
            int scroll_up_idx = submission_index-NUM_SUBMISSIONS_ROOT < 0 ? NUM_SUBMISSIONS_PER_PAGE-(sqrt(NUM_SUBMISSIONS_PER_PAGE))+submission_index : submission_index-NUM_SUBMISSIONS_ROOT;
            int scroll_right_idx = ((submission_index+1) % NUM_SUBMISSIONS_ROOT) == 0 ? (submission_index-(NUM_SUBMISSIONS_ROOT-1)) : submission_index+1;
            int scroll_left_idx = (submission_index % NUM_SUBMISSIONS_ROOT) == 0 ? (submission_index+(NUM_SUBMISSIONS_ROOT-1)) : submission_index-1;

            submission_node->setCustomNavigationRoute(brls::FocusDirection::DOWN, std::to_string( scroll_down_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::UP, std::to_string( scroll_up_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::RIGHT, std::to_string ( scroll_right_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::LEFT, std::to_string( scroll_left_idx ));
            */

            int scroll_down_idx = (submission_index+NUM_SUBMISSIONS_ROOT) % NUM_SUBMISSIONS_PER_PAGE;
            int scroll_up_idx = submission_index-NUM_SUBMISSIONS_ROOT < 0 ? NUM_SUBMISSIONS_PER_PAGE-(sqrt(NUM_SUBMISSIONS_PER_PAGE))+submission_index : submission_index-NUM_SUBMISSIONS_ROOT;
            int scroll_right_idx = ((submission_index+1) % NUM_SUBMISSIONS_ROOT) == 0 ? (submission_index-(NUM_SUBMISSIONS_ROOT-1)) : submission_index+1;
            int scroll_left_idx = (submission_index % NUM_SUBMISSIONS_ROOT) == 0 ? (submission_index+(NUM_SUBMISSIONS_ROOT-1)) : submission_index-1;


            submission_node->setCustomNavigationRoute(brls::FocusDirection::DOWN, std::to_string( scroll_down_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::UP, std::to_string( scroll_up_idx ));

            submission_node->setCustomNavigationRoute(brls::FocusDirection::RIGHT, std::to_string( scroll_right_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::LEFT, std::to_string( scroll_left_idx ));

            submission_index += 1;
            horzHolderBox->addView(submission_node);
        }
    }
}

void Browse::onChildFocusGained(View* directChild, View* focusedView) {
    if ( ((this->prev_selected_submission_id + 1) - NUM_SUBMISSIONS_ROOT) == std::stoi(focusedView->getID()) ) {
        // if we just scrolled right on the right-most submission

    }
    else if ( this->prev_selected_submission_id % NUM_SUBMISSIONS_ROOT == 0 && (std::stoi(focusedView->getID()) + 1) % NUM_SUBMISSIONS_ROOT == 0 ) {
        // if we just scrolled left on the left-most submission
        
    }
    Box::onChildFocusGained(directChild, focusedView);
}
void Browse::onChildFocusLost(View* directChild, View* focusedView) {
    this->prev_selected_submission_id = std::stoi(focusedView->getID());
    Box::onChildFocusLost(directChild, focusedView);
}