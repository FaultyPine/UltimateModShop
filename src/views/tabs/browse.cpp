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


// if these are to be changed, you'll have to adjust the horzHolder stuff too like height i think
// also these need to be numbers that multiply to a perfect square
const int NUM_SUBMISSIONS_ROW = 3;
const int NUM_SUBMISSIONS_COLUMN = 3;
static_assert( (NUM_SUBMISSIONS_ROW * NUM_SUBMISSIONS_COLUMN) == ( sqrt(NUM_SUBMISSIONS_ROW * NUM_SUBMISSIONS_COLUMN) * sqrt(NUM_SUBMISSIONS_ROW * NUM_SUBMISSIONS_COLUMN) ), "Number of submission rows and columns must multiply to a perfect square!");

Browse::Browse() {
    this->inflateFromXMLRes("xml/tabs/browse.xml");
    if (!NO_GB_REQUESTS)
        this->getNewGbSubmissions(1);
    Browse::initBrowseMenu(this);
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


// TODO: seperate this func into two - one that handles making the horzHolders and setting navigation, and another that handles setting img/labels from gb
void Browse::initBrowseMenu(Browse* browse) {

    gb::GetSubmissionDataMulticall(&(browse->new_submissions), {gb::Fields::Files, gb::Fields::Thumbnail, gb::Fields::Title, gb::Fields::Author});

    int submission_index = 0;
    constexpr int total_num_submissions = NUM_SUBMISSIONS_COLUMN * NUM_SUBMISSIONS_ROW;
    
    brls::Logger::debug("Populating Browse menu with new submissions...");

    for (int i = 0; i < NUM_SUBMISSIONS_COLUMN; i++) {
        brls::Box* horzHolderBox = (brls::Box*)browse->createFromXMLString(horzHolder);
        browse->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROW; i++) {

            gb::GbSubmissions new_subs = browse->new_submissions;
            SubmissionNode* submission_node;
            if (!new_subs.empty()) {
                gb::GbSubmission* sub = new_subs.at(submission_index);
                //gb::GetSubmissionData(sub, {gb::Fields::Files, gb::Fields::Thumbnail, gb::Fields::Title, gb::Fields::Author});
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
            int scroll_down_idx = (submission_index+NUM_SUBMISSIONS_ROW) % total_num_submissions;
            int scroll_up_idx = submission_index-NUM_SUBMISSIONS_ROW < 0 ? total_num_submissions-(sqrt(total_num_submissions))+submission_index : submission_index-NUM_SUBMISSIONS_ROW;
            int scroll_right_idx = ((submission_index+1) % NUM_SUBMISSIONS_ROW) == 0 ? (submission_index-(NUM_SUBMISSIONS_ROW-1)) : submission_index+1;
            int scroll_left_idx = (submission_index % NUM_SUBMISSIONS_ROW) == 0 ? (submission_index+(NUM_SUBMISSIONS_ROW-1)) : submission_index-1;

            submission_node->setCustomNavigationRoute(brls::FocusDirection::DOWN, std::to_string( scroll_down_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::UP, std::to_string( scroll_up_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::RIGHT, std::to_string ( scroll_right_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::LEFT, std::to_string( scroll_left_idx ));
            */

            // ---- Scrolling where scrolling all the way up/down/left/right doesn't loop back
            int scroll_down_idx = submission_index+NUM_SUBMISSIONS_ROW <= total_num_submissions ? submission_index+NUM_SUBMISSIONS_ROW : submission_index;
            int scroll_up_idx = submission_index-NUM_SUBMISSIONS_ROW < 0 ? total_num_submissions-(sqrt(total_num_submissions))+submission_index : submission_index-NUM_SUBMISSIONS_ROW;

            submission_node->setCustomNavigationRoute(brls::FocusDirection::DOWN, std::to_string( scroll_down_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::UP, std::to_string( scroll_up_idx ));


            submission_index += 1;
            horzHolderBox->addView(submission_node);
        }
    }
}
