#include "submission_node.h"
#include "../main_window.h"
#include "installed.h"
#include "installation/manager.h"
#include "mod_page.h"

SubmissionNode::SubmissionNode(gb::GbSubmission* _submission) {
    this->inflateFromXMLRes("xml/views/submission_node.xml");

    this->submission = _submission;

    BRLS_REGISTER_CLICK_BY_ID("submission_node_box", this->onSubmissionNodeClicked);
}

SubmissionNode::~SubmissionNode() {
    delete this->submission;
}

SubmissionNode::SubmissionNode() {
    this->inflateFromXMLRes("xml/views/submission_node.xml");
    BRLS_REGISTER_CLICK_BY_ID("submission_node_box", this->onSubmissionNodeClicked);
}

bool SubmissionNode::onSubmissionNodeClicked(brls::View* view)
{
    brls::Application::pushActivity(new ModPage(this));
    return true;
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

InstalledMod* SubmissionNode::downloadSubmission() {
    InstalledMod* ret = nullptr;
    if (this->submission != nullptr && !this->submission->submission_data.empty()) {
        brls::Logger::debug("--------\nDownloading submission...");
        json sd = this->submission->submission_data;

        std::vector<std::filesystem::path> paths = Manager::InstallModFiles(sd[gb::Fields::Files::Files]);

        sd[gb::Fields::Custom::Paths] = paths;
        
        sd[gb::Fields::Custom::Enabled] = true;
        
        installed_mods->GetMemJsonPtr()->at("Installed")[sd[gb::Fields::idRow].get<std::string>()] = sd;
        installed_mods->OverwriteFileFromMem();

        InstalledMod* m = new InstalledMod({
            sd[gb::Fields::Name].get<std::string>(), 
            sd[gb::Fields::Submitter::Submitter][gb::Fields::Name].get<std::string>(), 
            true,
            sd[gb::Fields::idRow].get<std::string>(), 
            sd[gb::Fields::Custom::ThumbnailURL].get<std::string>(), 
            paths
        });

        ret = m;

        brls::Logger::debug("Successfully downloaded {}\n--------", sd[gb::Fields::Name].get<std::string>());
    }
    else {
        brls::Logger::debug("Attempted to download submission with invalid data... ignoring...");
    }
    return ret;
}