#pragma once

#include <borealis.hpp>
#include "bar_item.h"

class TopBar : public brls::Box
{
  public:
    TopBar();

    static brls::View* create();

    private:
    BarItem* Menus[3] = { new BarItem("Browse"), new BarItem("Installed"), new BarItem("Settings") };

};