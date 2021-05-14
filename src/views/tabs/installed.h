#pragma once

#include <borealis.hpp>
#include "ums_utils.h"
#include "gb.h"

struct InstalledMod {
  std::string name;
  std::string author;
  std::string ver;
  bool enabled;
  std::string thumbnail_url;
  std::vector<std::string> paths;
};

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();
    void addInstalledItem(InstalledMod* mod);

    private:

};