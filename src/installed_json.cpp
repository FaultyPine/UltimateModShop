#include "installed_json.h"


InstalledJson::InstalledJson(json j) {
    if (!std::filesystem::exists(UMS_INSTALLED_JSON_PATH)) {
        std::fstream file;
        file.open(UMS_INSTALLED_JSON_PATH, std::ios::out);
        file << j;
        file.close();
    }
    
    if (j.empty() && std::filesystem::exists(UMS_INSTALLED_JSON_PATH)) {
        this->OverwriteMemFromFile();
    }
    else {
        this->installed_json = j;
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
    if (file.is_open()) {
        file << j;
    }
    else {
        brls::Logger::error("Failed to open file");
    }
    file.close();
}


json InstalledJson::GetFileJson() {
    std::fstream file;
    std::string line;
    std::stringstream file_json;
    file.open(UMS_INSTALLED_JSON_PATH, std::ios::in);

    while (std::getline(file, line)) {
        file_json << line;
    }
    file.close();
    return json(file_json.str());
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