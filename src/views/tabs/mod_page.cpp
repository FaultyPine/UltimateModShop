#include "mod_page.h"


ModPage::ModPage(SubmissionNode* sub) {
    this->submission = sub;
    this->v = brls::View::createFromXMLResource("tabs/mod_page/mod_page.xml");
}


void ModPage::onContentAvailable() {
    this->registerAction(
        "Close", brls::ControllerButton::BUTTON_B, [](brls::View* view) {
            brls::Application::popActivity();
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_CHANGE);

}