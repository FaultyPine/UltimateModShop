#pragma once

#include <borealis.hpp>
#include "ums_utils.h"
#include "gb.h"

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();
    void addInstalledItem(InstalledMod* mod);

    private:
    bool onInstalledItemClicked(brls::View* view);
};