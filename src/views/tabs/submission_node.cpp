#include "submission_node.h"
#include "../main_window.h"
#include "installed.h"

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
        brls::Logger::debug("--------\nDownloading submission...");
        json sd = this->submission->submission_data;
        std::string root(SD_ROOT);
        for (json file : sd[gb::Fields::Files]) { // iterate through each uploaded file in the submission... will probably end up prompting the user somehow or having them select which files in the submission they want
            brls::Logger::debug("Downloading file. Size = {} bytes", file["_nFilesize"].get<unsigned long>());
            std::string url = file["_sDownloadUrl"].get<std::string>();
            std::string path = root + file["_sFile"].get<std::string>();
            if (!REDUCED_NET_REQUESTS) {
                //curl::DownloadFile(url, path);
                brls::Logger::debug("Archive type: {}", file["_aMetadata"]["_sMimeType"].get<std::string>());
                if (file["_aMetadata"]["_sMimeType"].get<std::string>() == "application/zip") {
                    //UnZip::ArchiveExtract(path, root);
                }
            }
        }
        
        sd[gb::Fields::itemid] = submission->itemid;
        installed_mods->GetMemJsonPtr()->at("Installed") += sd;
        installed_mods->OverwriteFileFromMem();

        InstalledMod* m = new InstalledMod({
            sd[gb::Fields::Title].get<std::string>(), 
            sd[gb::Fields::Author].get<std::string>(), 
            std::to_string(sd[gb::Fields::NumUpdates].get<int>()), 
            true, 
            sd[gb::Fields::itemid].get<std::string>(), 
            sd[gb::Fields::Thumbnail].get<std::string>(), 
            {} // <- paths... not filled in yet
        });
        
        Installed* installed = (Installed*)((MainWindow*)main_box->getView("main_window"))->getLayerView()->getLayer("installed_box");
        installed->addInstalledItem(m);

        brls::Logger::debug("Successfully downloaded {}\n--------", sd[gb::Fields::Title].get<std::string>());
    }
    else {
        brls::Logger::debug("Attempted to download submission with invalid data... ignoring...");
    }
}