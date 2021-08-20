#pragma once

#include "ums_utils.h"
#include "mod_page.h"
#include "../progress_bar.h"

enum InstallationProcessType {
    MANUAL,
    AUTOMATIC
};



class DownloadHandler : public brls::Activity {
    public:
    
    DownloadHandler(ModPage* mpage);
    ~DownloadHandler();

    void onContentAvailable() override;
    brls::View* createContentView() override { return contentView; }
    brls::Box* contentView;


    private:

    void activateDownloadAuto();
    void fileSelectionPrompt(const json& files);
    void handleInstallationProcesses(json& files);

    InstalledMod* downloadSubmissionAuto(InstallationProcessType type, const std::vector<bool>& dl_idxs, CURL_builder* curl = nullptr);


    ModPage* modpage = nullptr;
    std::vector<bool> dl_idxs = {};
    InstallationProcessType install_type = InstallationProcessType::MANUAL;

    ProgressBar* progress_bar = nullptr;

};