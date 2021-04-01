#pragma once

#include <borealis.hpp>
#include "bar_item.h"
#include "../tabs/browse.h"
#include "../main_window.h"

class TopBar : public brls::Box
{
    public:
    TopBar();
    ~TopBar();

    static brls::View* create();

    /// Returns the first item in the Menus vector
    BarItem* getCurrentMenu();
    
    void tabNavigateLeft(brls::View* view);
    void tabNavigateRight(brls::View* view);

    private:

    int currently_selected_menu_idx = 0;

    /// initialized in constructor
    static std::vector<BarItem*> Menus; 

};