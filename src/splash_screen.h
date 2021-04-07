#pragma once

#include "ums_utils.h"

class SplashScreen : public brls::Activity {
    public:
    SplashScreen();
    void onContentAvailable() override;
    CONTENT_FROM_XML_RES("splash_screen.xml");
};