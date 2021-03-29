#pragma once

#include <borealis.hpp>

class TopBar : public brls::Box
{
  public:
    TopBar();

    static brls::View* create();
};