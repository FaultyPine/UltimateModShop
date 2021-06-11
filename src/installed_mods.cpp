#include "installed_mods.h"

InstalledMods::~InstalledMods() {
    for (InstalledMod* m : this->mods) {
        delete m;
    }
}

InstalledMods::InstalledMods(json default_json) {
    if (!std::filesystem::exists(UMS_INSTALLED_JSON_PATH)) {
        std::fstream file;
        file.open(UMS_INSTALLED_JSON_PATH, std::ios::out);
        file << default_json;
        file.close();
        this->installed_json = default_json;
        brls::Logger::debug("Init new InstalledMods json");
    }
    else {
        this->OverwriteMemFromFile();
    }
}

void InstalledMods::OverwriteFileFromMem() {
    std::fstream file;
    file.open(UMS_INSTALLED_JSON_PATH, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << this->installed_json;
    }
    else {
        brls::Logger::error("Failed to open file");
    }
    file.close();
}

void InstalledMods::AppendToFile(json j) {
    std::fstream file;
    file.open(UMS_INSTALLED_JSON_PATH, std::ios::out | std::ios::app);
    if (file.is_open())
        file << j;
    else
        brls::Logger::error("Failed to open file");
    file.close();
}

json InstalledMods::GetFileJson() {
    std::fstream file;
    json j;
    file.open(UMS_INSTALLED_JSON_PATH, std::ios::in);
    if (file.is_open())
        file >> j;
    else
        brls::Logger::error("Failed to open file");
    file.close();
    return j;
}

void InstalledMods::OverwriteMemFromFile() {
    this->installed_json = this->GetFileJson();
}

void InstalledMods::AppendToMem(json j) {
    this->installed_json += j;
}

json InstalledMods::GetMemJson() {
    return this->installed_json;
}

json* InstalledMods::GetMemJsonPtr() {
    return &(this->installed_json);
}

void InstalledMods::resetFile() {
    if (std::filesystem::exists(UMS_INSTALLED_JSON_PATH))
        std::filesystem::remove(UMS_INSTALLED_JSON_PATH);
    delete installed_mods;
    installed_mods = new InstalledMods();
}


void InstalledMods::addInstalledMod(InstalledMod* m) {
    this->mods.push_back(m);
}

void InstalledMods::removeInstalledMod(InstalledMod* m) {
    auto idx = std::find(this->mods.begin(), this->mods.end(), m);
    this->mods.erase(idx);
    this->installed_json["Installed"].erase(m->itemid);
    this->OverwriteFileFromMem();
    delete m;
}

InstalledMod* InstalledMods::getInstalledMod(int idx) {
    return this->mods.at(idx);
}

size_t InstalledMods::getInstalledModsSize() {
    return this->mods.size();
}