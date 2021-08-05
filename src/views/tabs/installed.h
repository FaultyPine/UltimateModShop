#pragma once

#include "ums_utils.h"

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();
    void addInstalledItem(InstalledMod* mod, CURL_builder* curl = nullptr);
    void willAppear(bool resetState) override;
    void willDisappear(bool resetState) override;

    private:

    void onUninstallPrompt(brls::View* v);
    void afterUninstallPrompt(brls::View* popup, brls::Box* installed_box);
};