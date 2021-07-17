#pragma once

#include "ums_utils.h"

class Settings : public brls::Box
{
  public:
    Settings();
    static brls::Box* create();

    private:

    void onUpdateCheckClicked(brls::View* view);
};