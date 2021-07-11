#include "main_activity.h"

#include "views/top_bar/top_bar.h"
#include "views/main_window.h"
#include "views/tabs/browse.h"

// for debugging
#include "views/tabs/submission_node.h"
#include "views/tabs/mod_page.h"

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

    /* ------ actions for debugging ------- */
    /*this->registerAction(
        "ResetJson", brls::ControllerButton::BUTTON_X, [installed_mods] (brls::View* v) {
            installed_mods->resetFile();
            brls::Application::quit();
            return true;
        }, false, brls::Sound::SOUND_CLICK
    );
    this->registerAction( // F1
        "modpage", brls::ControllerButton::BUTTON_BACK, [] (brls::View* v) {
            std::string mod_id = "293274";
            json _j = curl::DownloadJson("https://gamebanana.com/apiv3/Mod/" + mod_id);
            gb::GbSubmission* _g = new gb::GbSubmission {.submission_data = _j};
            SubmissionNode* _sub = new SubmissionNode(_g);
            brls::Application::pushActivity(new ModPage(_sub));
            return true;
        }, false, brls::Sound::SOUND_CLICK
    );*/
    /* -------------------------------------- */

    if (!REDUCED_NET_REQUESTS) {
        //json quote_of_the_day = curl::DownloadJson("http://quotes.rest/qod.json");
        //setMotdText(quote_of_the_day["contents"]["quotes"][0]["quote"].get<std::string>());
    }
}

brls::Box* main_box = nullptr;

MainActivity::MainActivity() {
    this->v = brls::View::createFromXMLResource("main.xml");
    main_box = (brls::Box*)this->v;
    ((brls::Box*)this->v)->addView(TopBar::create());
    ((brls::Box*)this->v)->addView(MainWindow::create());
    //((brls::Box*)this->v)->addView(brls::View::createFromXMLResource("test.xml"));
}