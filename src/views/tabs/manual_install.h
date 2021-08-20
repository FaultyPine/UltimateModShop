#pragma once

#include "ums_utils.h"
#include "download_handler.h"


struct ManualInstallInfo {
    std::unordered_map<std::string, std::string> pathMap = {};
};

/**
 * Take in files of the submisson and the indexes the user selected. Set our files json to the files the user wants
 * 
 *  For every file...
 * 
 * Display the filename/description/filesize and whatnot as a title-type-thing
 * download the file & extract to temp dir
 * read in temp dir folder structure
 * display it to user
 * allow user to select folders/files and select where those should go on emulated/actual SD card
 * Once user is done with that submission, move files/folders to specified paths (files/folders without specific paths get deleted) 
 * repeat
 * 
 * 
 */ 

class ManualInstall : public brls::Activity {
    public:

    ManualInstall(json files, std::vector<bool> dl_idxs);

    void onContentAvailable() override;
    brls::View* createContentView() override { return contentView; }
    brls::Box* contentView;

    private:

    json files;
    ManualInstallInfo info = {};

    ProgressBar* progress_bar = nullptr;

    bool can_exit = true;
};