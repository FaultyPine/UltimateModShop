#pragma once

#include "ums_utils.h"

namespace fs = std::filesystem;

namespace Manager {
    // files: data on files to download  :::::  dl_idxs: whether or not that mod file index should be downloaded
    std::vector<fs::path> InstallModFilesAuto(const json &files, const std::vector<bool>& dl_idxs, CURL_builder* curl = nullptr);
    std::vector<fs::path> InstallModFileAuto(std::string url, std::string filename, CURL_builder* curl = nullptr);

    //std::vector<fs::path> InstallModFilesManual();
    //std::vector<fs::path> InstallModFileManual();

    bool canUseAutomaticInstallation(const std::string& itemid, CURL_builder* curl = nullptr);


    void ToggleMod(InstalledMod* mod);
    void UninstallMod(InstalledMod* mod);
}