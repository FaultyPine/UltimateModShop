#pragma once

#include "ums_utils.h"

class MainActivity : public brls::Activity {
    public:
    void onContentAvailable() override;
    CONTENT_FROM_XML_RES("main.xml");
};