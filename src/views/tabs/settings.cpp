#include "settings.h"


void to_json(json& j, const SettingsInfo& s) {
    j = json {
         { "preferAutomaticInstallation", s.preferAutomaticInstallation },
    };
}
void from_json(const json& j, SettingsInfo& s) {
    j.at("preferAutomaticInstallation").get_to(s.preferAutomaticInstallation);
}



void Settings::serializeSettings() {
    installed_mods->GetMemJson().at("Settings") = json(*this->getSettings());
    installed_mods->OverwriteFileFromMem();
}

void Settings::readSettings() {
    SettingsInfo* s = this->getSettings();
    (*s) = installed_mods->GetMemJson().at("Settings").get<SettingsInfo>();
}


brls::Box* Settings::create() {
    return new Settings();
}

Settings::Settings() {
    this->inflateFromXMLRes("xml/tabs/settings.xml");

    this->getView("update_check_button")->registerClickAction([this](brls::View* v) { this->onUpdateCheckClicked(v); return false; });
    this->getView("prefer_auto_install_toggle")->registerClickAction([this](brls::View* v) { this->onTogglePreferredInstallation(v); return false; });

    if (!installed_mods->GetMemJson().at("Settings").empty()) {
        this->readSettings();
    }
    else {
        this->serializeSettings();
    }
}


void Settings::onUpdateCheckClicked(brls::View* view) {
    brls::Logger::debug("onUpdateCheckClicked");

    #ifdef __SWITCH__

    curl::DownloadFile(stdstr(UMS_GITHUB_RELEASE_URL) + UMS_GITHUB_RELEASE_SW, UMS_NRO_PATH);

    envSetNextLoad(UMS_NRO_PATH, UMS_NRO_PATH);

    brls::Application::quit();

    #else

    curl::DownloadFile(stdstr(UMS_GITHUB_RELEASE_URL) + UMS_GITHUB_RELEASE_PC, UMS_NRO_PATH);

    brls::Application::quit();

    #endif

}

void Settings::onTogglePreferredInstallation(brls::View* view) {
    SettingsInfo* s = this->getSettings();
    s->preferAutomaticInstallation = !s->preferAutomaticInstallation;
    this->serializeSettings();
}

SettingsInfo* Settings::getSettings() {
    return &settings;
}
