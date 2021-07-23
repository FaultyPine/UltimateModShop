#pragma once

#include "ums_utils.h"
#include "../popup.h"

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();
    void addInstalledItem(InstalledMod* mod, CURL_builder* curl = nullptr);
    void willAppear(bool resetState) override;
    void willDisappear(bool resetState) override;

    private:

    Popup* confirm_uninstall_popup = nullptr;
};