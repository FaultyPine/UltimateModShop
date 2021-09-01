#include "download_handler.h"

#include "../../installation/manager.h"
#include "installed.h"
#include "../main_window.h"


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
        paddingTop="5"
        paddingBottom="5"
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
    if (this->contentView->getView(this->progress_bar->getID())) {
        this->contentView->removeView(this->progress_bar);
    }
}

DownloadHandler::DownloadHandler(ModPage* mpage) {
    this->contentView = (brls::Box*)brls::View::createFromXMLResource("tabs/mod_page/download_handler.xml");
    this->modpage = mpage;
    this->progress_bar = new ProgressBar(this->contentView);
}



void DownloadHandler::onContentAvailable() {
    this->registerAction("Close", brls::ControllerButton::BUTTON_B, [](brls::View* v){
        brls::Application::giveFocus(nullptr);
        brls::Application::popActivity();
        return true;
    }, false, brls::Sound::SOUND_FOCUS_CHANGE);

    SubmissionNode* submission = this->modpage->getSubmissionNode();

    this->progress_bar->setTitle("Downloading...");

    json files = submission->getSubmissionData()->submission_data[gb::Fields::Files::Files];
    int num_files = files.size();
    this->dl_idxs.resize(num_files, false);
    this->dl_idxs.shrink_to_fit();

    if (num_files == 1)
        this->dl_idxs[0] = true;

    // set up file selection boxes where users can toggle which files they want to install
    this->fileSelectionPrompt(files);

    // set up buttons on the side which toggle the installation type (manual/automatic), and the install button itself
    this->handleInstallationProcesses(files);
}


void DownloadHandler::activateDownloadAuto() {
    brls::Application::blockInputs();
    
    SubmissionNode* submission = this->modpage->getSubmissionNode();
    brls::Box* install_button_box = (brls::Box*)modpage->getContentView()->getView("install_box");

    this->progress_bar->start();

    InstalledMod* m = this->downloadSubmissionAuto(this->dl_idxs);

    this->progress_bar->setTitle("Installing...");

    if (m) {
        BgTask::pushCallbackToQueue([m]() {
            Installed* installed = (Installed*)((MainWindow*)main_box->getView("main_window"))->getLayerView()->getLayer("installed_box");
            installed->addInstalledItem(m);
        });
        this->progress_bar->finish("Downloaded Successfully! Press A to confirm.", nvgRGB(56, 189, 32), 
        [this](brls::View* v) {
            this->progress_bar->defaultFinish();
            brls::Application::popActivity();
            return true;
        });
        install_button_box->setAlpha(0.2);
        this->modpage->can_install = false;
    }
    else { // unsuccessful install
        brls::Logger::warning("Install unsuccessful!");
        this->progress_bar->finish("Download failed! Press A to confirm.", nvgRGB(255, 0, 0), 
        [this](brls::View* v) {
            this->progress_bar->defaultFinish();
            brls::Application::popActivity();
            return true;
        });
    }
    brls::Application::unblockInputs();
}


void DownloadHandler::handleInstallationProcesses(json& files) {
    brls::Box* installation_process_selection_box = (brls::Box*)this->getView("installation_process_selection_box");

    brls::Button* installation_process_go = (brls::Button*)installation_process_selection_box->getView("installation_process_go");
    brls::Label* installation_process_status = (brls::Label*)installation_process_selection_box->getView("installation_process_status");

    SettingsInfo* settings = Settings::getSettings();
    std::string itemid = this->modpage->getSubmissionNode()->getSubmissionData()->submission_data[gb::Fields::idRow].get<std::string>();
    
    //bool canUseAutoInstall = Manager::canUseAutomaticInstallation(itemid);

    // start installation
    installation_process_go->registerClickAction([this, files](brls::View* v){
        std::thread dl (&DownloadHandler::activateDownloadAuto, this);
        dl.detach();
        return true;
    });

}




InstalledMod* DownloadHandler::downloadSubmissionAuto(const std::vector<bool>& dl_idxs, CURL_builder* curl) {
    InstalledMod* ret = nullptr;
    SubmissionNode* submission = this->modpage->getSubmissionNode();
    if (submission != nullptr && !submission->getSubmissionData()->submission_data.empty()) {
        brls::Logger::debug("--------\nDownloading submission...");
        if (!curl) {
            curl = new CURL_builder();
        }
        json& sd = submission->getSubmissionData()->submission_data;
        const json& files = sd[gb::Fields::Files::Files];

        std::vector<std::filesystem::path> paths = Manager::InstallModFilesAuto(files, dl_idxs, curl);
 
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
        delete curl;
    }
    else {
        brls::Logger::debug("Attempted to download submission with invalid data... ignoring...");
    }
    return ret;
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
            "Filesize: " + readable_fs(file[gb::Fields::Files::FileSize].get<float>())  + "                     Download Count: " + std::to_string(file[gb::Fields::DownloadCount].get<size_t>());
        file_selection_item_fileinfo->setText(fileinfo);

        brls::View* installation_process_go = this->contentView->getView("installation_process_go");

        // file selection toggle
        fileSelectionItem->registerClickAction([this, fileSelectionItem, i, installation_process_go](brls::View* v) {
            this->dl_idxs[i] = !this->dl_idxs[i];
            if (this->dl_idxs[i]) {
                fileSelectionItem->setAlpha(1.0);
            }
            else {
                fileSelectionItem->setAlpha(0.5);
            }
            // if there are no selected files, disable installation --- must have at least one file selected for installation
            if (std::count(this->dl_idxs.begin(), this->dl_idxs.end(), true) == 0) {
                installation_process_go->setVisibility(brls::Visibility::INVISIBLE);
            }
            else {
                installation_process_go->setVisibility(brls::Visibility::VISIBLE);
            }
            return true;
        });
    }
}