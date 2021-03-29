#include "submission_node.h"


SubmissionNode::SubmissionNode() {
    this->inflateFromXMLRes("xml/tabs/submission_node.xml");

    BRLS_REGISTER_CLICK_BY_ID("submission_button", this->onSubmissionNodeClicked);
}

bool SubmissionNode::onSubmissionNodeClicked(brls::View* view)
{
    brls::Logger::info("Submission node Clicked");
    return true;
}

brls::View* SubmissionNode::create()
{
    // Called by the XML engine to create a new ComponentsTab
    return new SubmissionNode();
}