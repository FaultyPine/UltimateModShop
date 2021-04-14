#pragma once

#include <borealis.hpp>
#include "ums_utils.h"
#include "gb.h"

class Installed : public brls::Box
{
  public:
    Installed();
    static brls::Box* create();

    void show(std::function<void(void)> cb) override;

    private:
    void ReadInstalledFromMem();

};