#include "top_bar.h"

void setSelected(brls::View* selected_view) {
    selected_view->setBackgroundColor(nvgRGBf(0.5, 0.0, 0.1));
    selected_view->setHeightPercentage(60.0);
    selected_view->setWidthPercentage(20.0);
}
void unsetSelected(brls::View* previously_selected_view) {
    previously_selected_view->setBackgroundColor(nvgRGBf(1.0, 1.0, 1.0));
    previously_selected_view->setHeightPercentage(45.0);
    previously_selected_view->setWidthPercentage(15.0);
}

TopBar::TopBar() {
    this->Menus = { new BarItem("Browse", Browse::create()), new BarItem("Installed"), new BarItem("Settings") };
    this->inflateFromXMLRes("xml/views/top_bar.xml");
    for (int i = 0; i < this->Menus.size(); i++) {
        BarItem* baritem = this->Menus.at(i);
        if (i == this->currently_selected_menu_idx)
            setSelected(baritem);
        this->addView(baritem);
    }
}

TopBar::~TopBar() {
    for (BarItem* x : Menus)
        delete x;
}

/// 'view' must have "main_window" as a child. (called from MainActivity)
void TopBar::tabNavigateLeft(brls::View* view) {
    unsetSelected(this->Menus.at(this->currently_selected_menu_idx));
    this->currently_selected_menu_idx = this->currently_selected_menu_idx-1 < 0 ? this->Menus.size()-1 : this->currently_selected_menu_idx-1;
    setSelected(this->Menus.at(this->currently_selected_menu_idx));
    MainWindow* mainwindow = (MainWindow*)view->getView("main_window");
    mainwindow->setCurrentWindow(this->Menus.at(this->currently_selected_menu_idx)->getTabView());
}

/// 'view' must have "main_window" as a child. (called from MainActivity)
void TopBar::tabNavigateRight(brls::View* view) {
    unsetSelected(this->Menus.at(this->currently_selected_menu_idx));
    this->currently_selected_menu_idx = (this->currently_selected_menu_idx+1) % this->Menus.size();
    setSelected(this->Menus.at(this->currently_selected_menu_idx));
    MainWindow* mainwindow = (MainWindow*)view->getView("main_window");
    mainwindow->setCurrentWindow(this->Menus.at(this->currently_selected_menu_idx)->getTabView());
}

BarItem* TopBar::getCurrentMenu() {
    return this->Menus.at(this->currently_selected_menu_idx);
}

brls::View* TopBar::create()
{
    return new TopBar();
}