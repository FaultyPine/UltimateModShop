#include "main_activity.h"

#include "views/top_bar/top_bar.h"
#include "views/main_window.h"
#include "views/tabs/browse.h"

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
}

brls::Box* main_box = nullptr;

MainActivity::MainActivity() {
    this->v = brls::View::createFromXMLResource("main.xml");
    main_box = (brls::Box*)this->v;
    ((brls::Box*)this->v)->addView(TopBar::create());
    ((brls::Box*)this->v)->addView(MainWindow::create());
}