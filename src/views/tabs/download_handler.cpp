#include "download_handler.h"

#include "../../installation/manager.h"
#include "installed.h"
#include "../main_window.h"

brls::Box* download_progress_bar = nullptr;

const std::string fileSelectionItemTemplate = R"item(
    <brls:Box
        axis="column"
        height="200"
        width="100%"
        justifyContent="spaceEvenly"
        marginTop="5"
        marginBottom="5"
        paddingLeft="10"
        borderThickness="3"
        borderColor="#000000"
        focusable="true"
    >

        <brls:Label
            text="File name:"
        />
        <brls:Label
            id="file_selection_item_title"
            fontSize="17"
            lineBottom="1"
            lineColor="#000000"
            marginBottom="5"
            marginLeft="10"
            textAlign="center"
        />

        <brls:Label
            text="Description:"
        />
        <brls:Label
            id="file_selection_item_description"
            fontSize="17"
            marginBottom="5"
            lineBottom="1"
            lineColor="#000000"
            marginLeft="10"
            textAlign="center"
        />

        <brls:Label
            text="File Info:"
        />
        <brls:Label
            id="file_selection_item_fileinfo"
            fontSize="17"
            marginBottom="5"
            marginLeft="10"
            textAlign="center"
        />

    </brls:Box>
)item";


DownloadHandler::~DownloadHandler() {
    download_progress_bar = nullptr;
}

DownloadHandler::DownloadHandler(ModPage* mpage) {
    this->contentView = (brls::Box*)brls::View::createFromXMLResource("tabs/mod_page/download_handler.xml");
    this->modpage = mpage;
    download_progress_bar = (brls::Box*)(this->contentView->getView("download_progress_bar"));
}



void DownloadHandler::onContentAvailable() {
    brls::Box* download_progress_box = (brls::Box*)this->contentView->getView("download_progress_box");

    // Download confirmation action
    std::function<bool(brls::View*)> confirm_download_action = 
    [this, download_progress_box](brls::View* v){
        download_progress_box->setVisibility(brls::Visibility::GONE);
        brls::Application::giveFocus(nullptr);
        brls::Application::popActivity();
        return false;
    };
    download_progress_box->registerClickAction(confirm_download_action);
    download_progress_box->registerAction("Close", brls::ControllerButton::BUTTON_B, confirm_download_action, false, brls::Sound::SOUND_FOCUS_CHANGE);
    initDownload();
}

std::function<void()> downloadThreadFunc = [](){};

void DownloadHandler::initDownload() {
    
    SubmissionNode* submission = this->modpage->getSubmissionNode();
    brls::Box* download_progress_box = (brls::Box*)this->contentView->getView("download_progress_box");
    brls::Label* download_progress_label = (brls::Label*)download_progress_box->getView("download_progress_label");
    brls::Box* install_button_box = (brls::Box*)modpage->getContentView()->getView("install_box");

    download_progress_bar->setWidthPercentage(0.0);
    download_progress_label->setText("Downloading...");
    download_progress_label->setTextColor(nvgRGB(0, 0, 0));

    const json& files = submission->getSubmissionData()->submission_data[gb::Fields::Files::Files];
    int num_files = files.size();
    this->dl_idxs.resize(num_files, false);
    this->dl_idxs.shrink_to_fit();


    CURL_builder* curl = new CURL_builder();

    downloadThreadFunc = 
    [this, submission, download_progress_box, download_progress_label, install_button_box, curl]() {
        brls::Application::blockInputs();
        download_progress_box->setVisibility(brls::Visibility::VISIBLE);

        InstalledMod* m = nullptr;
        if (this->install_type == InstallationProcessType::AUTOMATIC) {
            m = this->downloadSubmissionAuto(this->dl_idxs, curl);
        }
        else {
            brls::Logger::warning("Manual installation not supported currently");
            //m = this->downloadSubmissionManual(....);
        }

        if (m) {
            BgTask::pushCallbackToQueue([m]() {
                Installed* installed = (Installed*)((MainWindow*)main_box->getView("main_window"))->getLayerView()->getLayer("installed_box");
                installed->addInstalledItem(m);
            });
            download_progress_label->setText("Downloaded Successfully! Press A to confirm.");
            download_progress_label->setTextColor(nvgRGB(56, 189, 32));
            install_button_box->setAlpha(0.2);
            this->modpage->can_install = false;
        }
        else { // unsuccessful install
            brls::Logger::warning("Install unsuccessful!");
            download_progress_label->setText("Download failed! Press A to confirm.");
            download_progress_label->setTextColor(nvgRGB(255, 0, 0));
        }
        brls::Application::giveFocus(download_progress_box);
        delete curl;
        brls::Application::unblockInputs();
    };


    if (num_files == 1)
        this->dl_idxs[0] = true;

    this->fileSelectionPrompt(files);

    this->handleInstallationProcesses(curl);
}


void DownloadHandler::fileSelectionPrompt(const json& files) {
    brls::Box* download_file_selection_box = (brls::Box*)this->getView("download_file_selection_box");
    brls::Box* fileSelectionPromptBox = (brls::Box*)brls::View::createFromXMLResource("tabs/mod_page/download_file_selection.xml");
    download_file_selection_box->addView(fileSelectionPromptBox);

    brls::Box* download_file_selection_list = (brls::Box*)fileSelectionPromptBox->getView("download_file_selection_list");

    // for each file in the submission, create a box containing info
    // like the filename, filesize, download count, and file description
    for (int i = 0; i < files.size(); i++) {
        const json& file = files[i];

        brls::Box* fileSelectionItem = (brls::Box*)brls::View::createFromXMLString(fileSelectionItemTemplate);
        download_file_selection_list->addView(fileSelectionItem);

        if (!dl_idxs[i])
            fileSelectionItem->setAlpha(0.5);

        brls::Label* file_selection_item_title = (brls::Label*)fileSelectionItem->getView("file_selection_item_title");
        brls::Label* file_selection_item_description = (brls::Label*)fileSelectionItem->getView("file_selection_item_description");
        brls::Label* file_selection_item_fileinfo = (brls::Label*)fileSelectionItem->getView("file_selection_item_fileinfo");

        file_selection_item_title->setText(file[gb::Fields::Files::FileName].get<std::string>());
        file_selection_item_description->setText(file[gb::Fields::Subtitle].get<std::string>());
        std::string fileinfo = 
            "Filesize: " + std::to_string(file[gb::Fields::Files::FileSize].get<size_t>()) + "                     Download Count: " + std::to_string(file[gb::Fields::DownloadCount].get<size_t>());
        file_selection_item_fileinfo->setText(fileinfo);

        // file selection toggle
        fileSelectionItem->registerClickAction([this, fileSelectionItem, i](brls::View* v) {
            this->dl_idxs[i] = !this->dl_idxs[i];
            if (this->dl_idxs[i]) {
                fileSelectionItem->setAlpha(1.0);
            }
            else {
                fileSelectionItem->setAlpha(0.5);
            }
            return false;
        });
    }
}


void DownloadHandler::handleInstallationProcesses(CURL_builder* curl) {
    brls::Box* installation_process_selection_box = (brls::Box*)this->getView("installation_process_selection_box");

    brls::Button* installation_process_automatic = (brls::Button*)installation_process_selection_box->getView("installation_process_automatic");
    brls::Button* installation_process_manual = (brls::Button*)installation_process_selection_box->getView("installation_process_manual");
    brls::Button* installation_process_go = (brls::Button*)installation_process_selection_box->getView("installation_process_go");
    brls::Label* installation_process_status = (brls::Label*)installation_process_selection_box->getView("installation_process_status");

    // handle alpha for default option
    if (this->install_type == InstallationProcessType::MANUAL) {
        installation_process_automatic->setAlpha(0.5);
    }
    else {
        installation_process_manual->setAlpha(0.5);
    }

    SettingsInfo* settings = Settings::getSettings();
    std::string itemid = this->modpage->getSubmissionNode()->getSubmissionData()->submission_data[gb::Fields::idRow].get<std::string>();
    
    bool canUseAutoInstall = Manager::canUseAutomaticInstallation(itemid, curl);

    if (!canUseAutoInstall) { // if can't auto install...
        // disable auto button
        installation_process_automatic->setVisibility(brls::Visibility::GONE);
        // mark install as manual
        this->install_type = InstallationProcessType::MANUAL;
        // "select" & disable manual toggle
        installation_process_manual->setFocusable(false);
        installation_process_manual->setAlpha(0.5);
        installation_process_status->setText("Unable to auto-install. Defaulted to manual.");
        installation_process_status->setVisibility(brls::Visibility::VISIBLE);
        installation_process_go->setVisibility(brls::Visibility::VISIBLE);
    }
    else if (canUseAutoInstall) {
        if (settings->preferAutomaticInstallation) {
            // if we prefer auto install, and can do it, get rid of the other options entirely
            installation_process_manual->setVisibility(brls::Visibility::GONE);
            installation_process_automatic->setVisibility(brls::Visibility::GONE);
            installation_process_status->setText("Using Automatic Installation.");
            installation_process_status->setVisibility(brls::Visibility::VISIBLE);
            this->install_type = InstallationProcessType::AUTOMATIC;
            installation_process_go->setVisibility(brls::Visibility::VISIBLE);
        }
        else {
            // automatic install toggle
            installation_process_automatic->registerClickAction([this, installation_process_automatic, installation_process_manual](brls::View* v){
                if (this->install_type == InstallationProcessType::MANUAL) {
                    this->install_type = InstallationProcessType::AUTOMATIC;
                    installation_process_automatic->setAlpha(1.0);
                    installation_process_manual->setAlpha(0.5);
                }
                return false;
            });

            // manual install toggle
            installation_process_manual->registerClickAction([this, installation_process_manual, installation_process_automatic](brls::View* v){
                if (this->install_type == InstallationProcessType::AUTOMATIC) {
                    this->install_type = InstallationProcessType::MANUAL;
                    installation_process_manual->setAlpha(1.0);
                    installation_process_automatic->setAlpha(0.5);
                }
                return false;
            });
            installation_process_go->setVisibility(brls::Visibility::VISIBLE);
        }
    }


    

    // start installation
    installation_process_go->registerClickAction([](brls::View* v){
        std::thread download_thread(downloadThreadFunc);
        download_thread.detach();
        return false;
    });

}




InstalledMod* DownloadHandler::downloadSubmissionAuto(const std::vector<bool>& dl_idxs, CURL_builder* curl) {
    InstalledMod* ret = nullptr;
    SubmissionNode* submission = this->modpage->getSubmissionNode();
    if (submission != nullptr && !submission->getSubmissionData()->submission_data.empty()) {
        brls::Logger::debug("--------\nDownloading submission...");
        json& sd = submission->getSubmissionData()->submission_data;

        std::vector<std::filesystem::path> paths = Manager::InstallModFilesAuto(sd[gb::Fields::Files::Files], dl_idxs, curl);

        if (!paths.empty()) {
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
            brls::Logger::warning("Installed paths empty!");
        }

    }
    else {
        brls::Logger::debug("Attempted to download submission with invalid data... ignoring...");
    }
    return ret;
}