#pragma once

#include "ums_utils.h"
#include "mod_page.h"

enum InstallationProcessType {
    MANUAL,
    AUTOMATIC
};

/**
 * First, prompt for which files they want to download.
 * Then download those files
 * Then prompt for automatic/manual installation of those downloaded files (<- 'files' here means the archives)
 * extract/install based on previous choice, or if the 'Prefer automatic installation' setting is toggled
 */

class DownloadHandler : public brls::Activity {
    public:
    
    DownloadHandler(ModPage* mpage);
    ~DownloadHandler();

    void onContentAvailable() override;
    brls::View* createContentView() override { return contentView; }
    brls::Box* contentView;


    private:

    void initDownload();
    void fileSelectionPrompt(const json& files);
    void handleInstallationProcesses(CURL_builder* curl = nullptr);

    InstalledMod* downloadSubmissionAuto(const std::vector<bool>& dl_idxs, CURL_builder* curl = nullptr);


    ModPage* modpage = nullptr;
    std::vector<bool> dl_idxs = {};
    InstallationProcessType install_type = InstallationProcessType::MANUAL;

};