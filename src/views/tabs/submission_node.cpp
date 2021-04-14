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
    this->downloadSubmission();
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

void SubmissionNode::downloadSubmission() {
    if (this->submission != nullptr && !this->submission->submission_data.empty()) {
        brls::Logger::debug("Downloading submission...");
        json sd = this->submission->submission_data;
        for (json file : sd[gb::Fields::Files]) { // iterate through each uploaded file in the submission
            brls::Logger::debug("Downloading file. Size = {} bytes", file["_nFilesize"].get<unsigned long>());
            std::string url = file["_sDownloadUrl"].get<std::string>();
            std::string path = s(SD_ROOT) + file["_sFile"].get<std::string>();
            //curl::DownloadFile(url, path);
        }
        brls::Logger::debug("Successfully downloaded {}", sd[gb::Fields::Title].get<std::string>());
        installed_mods->GetMemJsonPtr()->at("Installed") += sd;
    }
    else {
        brls::Logger::debug("Attempted to download submission with invalid data... ignoring...");
    }
}