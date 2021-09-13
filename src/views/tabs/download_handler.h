#pragma once

#include "ums_utils.h"
#include "mod_page.h"
#include "../progress_bar.h"



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

    InstalledMod* downloadSubmissionAuto(const std::vector<bool>& dl_idxs, CURL_builder* curl = nullptr);


    ModPage* modpage = nullptr;
    std::vector<bool> dl_idxs = {};

    ProgressBar* progress_bar = nullptr; // gets added to main view and freed by brls

};