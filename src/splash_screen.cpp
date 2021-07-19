#include "splash_screen.h"

#include "main_activity.h"

void SplashScreen::onContentAvailable() {
    //brls::Logger::debug("Splash screen content available");
    std::string motds = 
    R""""(
    .------------------------.
    |      HELP DESK         |
    |________________________|
    ||     .-"""--.         ||
    ||    /        \.-.     ||
    ||   |     ._,     \    ||
    ||   \_/`-'   '-.,_/    ||
    ||   (_   (' _)') \     ||
    ||   /|           |\    ||
    ||  | \     __   / |    ||
    ||   \_).,_____,/}/     ||
___||____;_--'___'/ (      ||
|\ ||   (__,\\    \_/------||
||\||______________________||
||||                        |
||||       0700 - 1700      |
\|||                (------)|
\||_________________//||\\_|
)"""";
    ((brls::Label*)this->getView("splash_screen_motd_text"))->setText(motds);

    // not-so-great way of utilizing the splash screen. Will (maybe?) stay until async tasks for brls are implemented
    this->getView("splash_screen_continue_button")->registerClickAction([](brls::View* v) { 
        brls::Application::popActivity();
        brls::Application::pushActivity(new MainActivity());
        return false;
    });
    BgTask::Begin();
}

SplashScreen::SplashScreen() {
    //brls::Logger::debug("Splash screen ctor");
    //setup();
}