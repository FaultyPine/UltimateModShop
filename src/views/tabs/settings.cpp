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

}