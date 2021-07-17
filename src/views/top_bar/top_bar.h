#pragma once

#include <borealis.hpp>
#include "bar_item.h"
#include "../main_window.h"

#include "../tabs/browse.h"
#include "../tabs/installed.h"
#include "../tabs/settings.h"

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

    int currently_selected_menu_idx = 0;

    /// initialized in constructor
    std::vector<BarItem*> Menus;

    private:

    void setSelected(brls::View* selected_view);
    void unsetSelected(brls::View* previously_selected_view);

    const float heightIncreaseOnSelect = 5.0;
    const float widthIncreaseOnSelect = 65.0;
};