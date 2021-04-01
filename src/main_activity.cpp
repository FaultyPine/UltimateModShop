#include "main_activity.h"


void MainActivity::onContentAvailable() {
    TopBar* top_bar = (TopBar*)this->getView("top_bar");
    this->registerAction(
        "Tab Right", brls::ControllerButton::BUTTON_RB, [this, top_bar](brls::View* view) {
            top_bar->tabNavigateRight(view);
            return true;
        },
        false, brls::Sound::SOUND_CLICK_SIDEBAR);
    this->registerAction(
        "Tab Left", brls::ControllerButton::BUTTON_LB, [this, top_bar](brls::View* view) {
            top_bar->tabNavigateLeft(view);
            return true;
        },
        false, brls::Sound::SOUND_CLICK_SIDEBAR);
    MainWindow* main_window = (MainWindow*)this->getView("main_window");
    main_window->setCurrentWindow(top_bar->getCurrentMenu()->getTabView());
}