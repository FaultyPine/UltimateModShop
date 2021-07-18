#include "settings.h"

brls::Box* Settings::create() {
    return new Settings();
}

Settings::Settings() {
    this->inflateFromXMLRes("xml/tabs/settings.xml");

    this->getView("update_check_button")->registerClickAction([this](brls::View* v) { this->onUpdateCheckClicked(v); return false; });
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