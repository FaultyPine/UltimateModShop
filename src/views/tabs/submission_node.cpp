#include "submission_node.h"


SubmissionNode::SubmissionNode(gb::GbSubmission* _submission) {
    this->inflateFromXMLRes("xml/views/submission_node.xml");

    this->submission = _submission;

    BRLS_REGISTER_CLICK_BY_ID("submission_node_box", this->onSubmissionNodeClicked);
}

SubmissionNode::SubmissionNode() {
    this->inflateFromXMLRes("xml/views/submission_node.xml");
    BRLS_REGISTER_CLICK_BY_ID("submission_node_box", this->onSubmissionNodeClicked);
}

bool SubmissionNode::onSubmissionNodeClicked(brls::View* view)
{
    brls::Logger::debug("Submission node Clicked");
    return false;
}

void SubmissionNode::onFocusGained() {
    Box::onFocusGained();
    ((brls::Label*)(this->getView("submission_label")))->setTextColor(nvgRGBf(0.0, 0.0, 0.0));
}
void SubmissionNode::onFocusLost() {
    Box::onFocusLost();
    ((brls::Label*)(this->getView("submission_label")))->setTextColor(nvgRGBf(1.0, 1.0, 1.0));
}

void SubmissionNode::setSubmissionData(gb::GbSubmission* s) {
    this->submission = s;
}

gb::GbSubmission* SubmissionNode::getSubmissionData() {
    return this->submission;
}