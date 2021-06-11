#pragma once

#include "ums_utils.h"

namespace fs = std::filesystem;

namespace Manager {
    std::vector<fs::path> InstallModFiles(const json &files);
    std::vector<fs::path> InstallModFile(std::string url, std::string filename);


    void ToggleMod(InstalledMod* mod);
    void UninstallMod(InstalledMod* mod);
}