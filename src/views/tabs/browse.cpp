#include "browse.h"


const std::string horzHolder = R"xml(
    <brls:Box 
        id="horzHolder"
        axis="row"
        width="100%"
        height="25%"
        marginBottom="10"
        marginTop="10"
        borderThickness="2"
        borderColor="#AAFF00"
    />
)xml";

// if these are to be changed, you'll have to adjust the horzHolder stuff too like height i think
const int NUM_SUBMISSIONS_ROW = 3;
const int NUM_SUBMISSIONS_COLUMN = 3;

Browse::Browse() {
    this->inflateFromXMLRes("xml/tabs/browse.xml");

    int submission_index = 0;
    constexpr int total_num_submissions = NUM_SUBMISSIONS_COLUMN * NUM_SUBMISSIONS_ROW;
    for (int i = 0; i < NUM_SUBMISSIONS_COLUMN; i++) {
        brls::Box* horzHolderBox = (brls::Box*)this->createFromXMLString(horzHolder);
        this->addView(horzHolderBox);

        for (int i = 0; i < NUM_SUBMISSIONS_ROW; i++) {
            brls::Box* submission_node = new SubmissionNode();
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

brls::Box* Browse::create() {
    return new Browse();
}