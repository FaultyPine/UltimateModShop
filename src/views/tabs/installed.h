#pragma once

#include <borealis.hpp>
#include "ums_utils.h"
#include "gb.h"

struct InstalledMod {
  std::string name;
  std::string author;
  bool enabled;
  MemoryStruct img;
  std::vector<std::string> paths;
};

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();

    void show(std::function<void(void)> cb) override;

    private:
    void LoadInstalledFromMem();

};