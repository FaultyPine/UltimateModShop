#include "manual_install.h"

namespace fs = std::filesystem;

ManualInstall::ManualInstall(json files, std::vector<bool> dl_idxs) {
    if (files.size() == dl_idxs.size()) {
        for (int i = 0; i < files.size(); i++) {
            if (dl_idxs[i])
                this->files.push_back(files[i]);
        }
        this->contentView = (brls::Box*)brls::View::createFromXMLResource("tabs/mod_page/manual_install.xml");
        this->progress_bar = new ProgressBar(this->contentView);
    }
    else {
        brls::Logger::error("Files and download indicies sizes are not equal!");
        brls::Application::giveFocus(nullptr);
        brls::Application::popActivity();
    }
}


void ManualInstall::onContentAvailable() {
    /*
    int idx = 0; //tmp

    // [folder/file] -> [destination]
    // this map gets passed into Manager::InstallFilesManual
    std::unordered_map<std::string, std::string> pathMap = {};

    std::string url = this->files[idx][gb::Fields::Files::DownloadURL].get<std::string>();
    std::string filename = this->files[idx][gb::Fields::Files::FileName].get<std::string>();

    std::string tmp_extraction_dir = TEMP_EXTRACTION_DIR;
    std::string root_file = stdstr(SD_ROOT) + filename;

    // create temp directory to allow us to easily read the folder structure
    fs::create_directory(tmp_extraction_dir);

    curl::DownloadFile(url, root_file);
    
    if (UnZip::ArchiveExtract(root_file, tmp_extraction_dir)) {
        //std::vector<fs::path> tmp_dir_paths = {};
        //for (const auto& dirEntry : fs::recursive_directory_iterator(tmp_extraction_dir)) {
        //    tmp_dir_paths.push_back(dirEntry);
        //}
    }
    */


    this->contentView->registerAction("Close", brls::ControllerButton::BUTTON_B, [](brls::View* v){
        brls::Application::giveFocus(nullptr);
        brls::Application::popActivity();
        return true;
    }, false, brls::Sound::SOUND_FOCUS_CHANGE);
}
