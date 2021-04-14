#pragma once

#include <borealis.hpp>
#include "bar_item.h"
#include "../main_window.h"

#include "../tabs/browse.h"
#include "../tabs/installed.h"

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
    std::vector<BarItem*> Menus; 

};