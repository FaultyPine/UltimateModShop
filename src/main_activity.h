#pragma once

#include "ums_utils.h"

class MainActivity : public brls::Activity {
    public:
    MainActivity();
    void onContentAvailable() override;
    brls::View* createContentView() override { return v; }
    brls::View* v;
};