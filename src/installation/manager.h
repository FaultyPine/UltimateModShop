#pragma once

#define TOML_IMPLEMENTATION
#include "ums_utils.h"

namespace fs = std::filesystem;



namespace Manager {
    typedef std::unordered_map<std::filesystem::path, std::filesystem::path> pathMap;

    // files: data on files to download  :::::  dl_idxs: whether or not that mod file index should be downloaded
    std::vector<fs::path> InstallModFilesAuto(const json &files, const std::vector<bool>& dl_idxs, CURL_builder* curl = nullptr);
    std::vector<fs::path> InstallModFileAuto(std::string url, std::string filename, CURL_builder* curl = nullptr);

    bool canUseAutomaticInstallation(const std::string& itemid, CURL_builder* curl = nullptr);


    void ToggleMod(InstalledMod* mod);
    void UninstallMod(InstalledMod* mod);
}