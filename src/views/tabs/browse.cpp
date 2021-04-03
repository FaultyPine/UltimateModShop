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
const int NUM_SUBMISSIONS_ROW = 3;
const int NUM_SUBMISSIONS_COLUMN = 3;

Browse::Browse() {
    this->inflateFromXMLRes("xml/tabs/browse.xml");
    this->getNewGbSubmissions();
    if (this->new_submissions.size() == 0)
        return;

    int submission_index = 0;
    constexpr int total_num_submissions = NUM_SUBMISSIONS_COLUMN * NUM_SUBMISSIONS_ROW;
    brls::Logger::debug("Populating Browse menu with new submissions...");
    for (int i = 0; i < NUM_SUBMISSIONS_COLUMN; i++) {
        brls::Box* horzHolderBox = (brls::Box*)this->createFromXMLString(horzHolder);
        this->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROW; i++) {

            gb::GbSubmission* sub = this->new_submissions.at(submission_index);
            gb::GetSubmissionData(sub, {gb::Fields::Files, gb::Fields::Thumbnail, gb::Fields::Title, gb::Fields::Author});

            SubmissionNode* submission_node = new SubmissionNode(sub);

            brls::Image* submission_image = (brls::Image*)submission_node->getView("submission_image");
            brls::Label* submission_label = (brls::Label*)submission_node->getView("submission_label");

            std::string submission_title = sub->submission_data[0][gb::Fields::Title].get<std::string>();
            submission_label->setText(submission_title);

            // download image to memory
            std::string thumbnail_url = sub->submission_data[0][gb::Fields::Thumbnail].get<std::string>();
            if (thumbnail_url != gb::GB_NO_THUMBNAIL_URL) {
                MemoryStruct s = curl::DownloadToMem(thumbnail_url);
                submission_image->setImageFromMem((unsigned char*)s.memory, s.size);
            }
            else if (sub->itemtype == "Sound") {
                submission_image->setImageFromRes("icon/sound_submission.png");
            }
            
            submission_node->setId( std::to_string( submission_index ) );

            int scroll_down_idx = (submission_index+3) % total_num_submissions;
            int scroll_up_idx = submission_index-3 < 0 ? total_num_submissions-(sqrt(total_num_submissions))+submission_index : submission_index-3;
            int scroll_right_idx = ((submission_index+1) % NUM_SUBMISSIONS_ROW) == 0 ? (submission_index-(NUM_SUBMISSIONS_ROW-1)) : submission_index+1;
            int scroll_left_idx = (submission_index % NUM_SUBMISSIONS_ROW) == 0 ? (submission_index+(NUM_SUBMISSIONS_ROW-1)) : submission_index-1;

            submission_node->setCustomNavigationRoute(brls::FocusDirection::DOWN, std::to_string( scroll_down_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::UP, std::to_string( scroll_up_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::RIGHT, std::to_string ( scroll_right_idx ));
            submission_node->setCustomNavigationRoute(brls::FocusDirection::LEFT, std::to_string( scroll_left_idx ));

            submission_index += 1;
            horzHolderBox->addView(submission_node);
        }
    }
}

void Browse::getNewGbSubmissions() {
    gb::GbSubmissions subs = gb::GetNewSubmissions(1);
    brls::Logger::debug("Got {} new submissions!", subs.size());
    if (subs.size() == 0) {
        brls::Logger::error("Found 0 new submissions... is something broken?");
        return;
    }
    this->new_submissions = subs;
}