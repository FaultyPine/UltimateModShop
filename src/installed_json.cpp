#include "installed_json.h"


InstalledJson::InstalledJson(json default_json) {
    if (!std::filesystem::exists(UMS_INSTALLED_JSON_PATH)) {
        std::fstream file;
        file.open(UMS_INSTALLED_JSON_PATH, std::ios::out);
        file << default_json;
        file.close();
        this->installed_json = default_json;
    }
    else {
        this->OverwriteMemFromFile();
    }
    
}

void InstalledJson::OverwriteFileFromMem() {
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

void InstalledJson::AppendToFile(json j) {
    std::fstream file;
    file.open(UMS_INSTALLED_JSON_PATH, std::ios::out | std::ios::app);
    if (file.is_open())
        file << j;
    else
        brls::Logger::error("Failed to open file");
    file.close();
}

json InstalledJson::GetFileJson() {
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

void InstalledJson::OverwriteMemFromFile() {
    this->installed_json = this->GetFileJson();
}

void InstalledJson::AppendToMem(json j) {
    this->installed_json += j;
}

json InstalledJson::GetMemJson() {
    return this->installed_json;
}

json* InstalledJson::GetMemJsonPtr() {
    return &(this->installed_json);
}