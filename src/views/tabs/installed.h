#pragma once

#include "ums_utils.h"

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();
    void addInstalledItem(InstalledMod* mod);
    void willAppear(bool resetState) override;
    void willDisappear(bool resetState) override;

    private:

};