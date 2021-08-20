#pragma once

#include "ums_utils.h"
#include "../views/tabs/manual_install.h"

namespace fs = std::filesystem;



namespace Manager {
    typedef std::unordered_map<std::filesystem::path, std::filesystem::path> pathMap;

    // files: data on files to download  :::::  dl_idxs: whether or not that mod file index should be downloaded
    std::vector<fs::path> InstallModFilesAuto(const json &files, const std::vector<bool>& dl_idxs, CURL_builder* curl = nullptr);
    std::vector<fs::path> InstallModFileAuto(std::string url, std::string filename, CURL_builder* curl = nullptr);

    // use a callback for after we extract the files
    std::vector<fs::path> InstallModFilesManual(const json &files, const std::vector<bool>& dl_idxs, ManualInstallInfo& info, CURL_builder* curl = nullptr);
    std::vector<fs::path> InstallModFileManual(std::string url, std::string filename, ManualInstallInfo& info, CURL_builder* curl = nullptr);

    bool canUseAutomaticInstallation(const std::string& itemid, CURL_builder* curl = nullptr);


    void ToggleMod(InstalledMod* mod);
    void UninstallMod(InstalledMod* mod);
}