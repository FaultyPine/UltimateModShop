#pragma once

#include "ums_utils.h"

namespace fs = std::filesystem;

namespace Manager {
    // files: data on files to download  :::::  dl_idxs: whether or not that mod file index should be downloaded. All files downloaded by default
    std::vector<fs::path> InstallModFiles(const json &files, const std::vector<bool>& dl_idxs = {});
    std::vector<fs::path> InstallModFile(std::string url, std::string filename);


    void ToggleMod(InstalledMod* mod);
    void UninstallMod(InstalledMod* mod);
}