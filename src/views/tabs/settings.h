#pragma once

#include "ums_utils.h"

// when adding settings, make sure to properly serialize them in the to_json/from_json funcs in settings.cpp

struct SettingsInfo {
    bool preferAutomaticInstallation;
};

static SettingsInfo settings = { //default settings
    false,
};

class Settings : public brls::Box
{
    public:
      Settings();
      static brls::Box* create();

      static SettingsInfo* getSettings();

      private:

      void onUpdateCheckClicked(brls::View* view);
      void onTogglePreferredInstallation(brls::View* view);

      void serializeSettings();
      void readSettings();

};