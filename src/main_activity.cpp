#include "main_activity.h"

#include "views/top_bar/top_bar.h"
#include "views/main_window.h"
#include "views/tabs/browse.h"

void MainActivity::onContentAvailable() {
    TopBar* top_bar = (TopBar*)this->getView("top_bar");
    MainWindow* main_window = (MainWindow*)this->getView("main_window");
    main_window->setCurrentWindow(top_bar->getCurrentMenu()->getTabView());

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

}

MainActivity::MainActivity() {
    
}