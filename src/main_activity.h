#pragma once

#include "utils.h"
#include "views/top_bar/top_bar.h"
#include "views/main_window.h"

class MainActivity : public brls::Activity {
    public:
    void onContentAvailable() override;
    CONTENT_FROM_XML_RES("main.xml");
};