#pragma once

#include <borealis.hpp>

class BarItem : public brls::Box
{
  public:
    BarItem(std::string text);

    static brls::View* create(std::string text = "");

    private:
    
};