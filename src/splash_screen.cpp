#include "splash_screen.h"

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

    BgTask::Begin(3000);
}

SplashScreen::SplashScreen() {
    //brls::Logger::debug("Splash screen ctor");
}