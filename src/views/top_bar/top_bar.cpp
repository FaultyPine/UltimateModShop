#include "top_bar.h"

std::vector<BarItem*> TopBar::Menus;

TopBar::TopBar() {
    this->Menus = { new BarItem("Browse", Browse::create()), new BarItem("Installed"), new BarItem("Settings") };
    this->inflateFromXMLRes("xml/views/top_bar.xml");
    for (int i = 0; i < this->Menus.size(); i++) {
        BarItem* baritem = this->Menus.at(i);
        this->addView(baritem);
    }
}

TopBar::~TopBar() {
    for (BarItem* x : Menus)
        delete x;
}

/// 'view' must have "main_window" as a child. (called from MainActivity)
void TopBar::tabNavigateLeft(brls::View* view) {
    this->currently_selected_menu_idx = this->currently_selected_menu_idx-1 < 0 ? this->Menus.size()-1 : this->currently_selected_menu_idx-1;
    MainWindow* mainwindow = (MainWindow*)view->getView("main_window");
    mainwindow->setCurrentWindow(this->Menus.at(this->currently_selected_menu_idx)->getTabView());
}

/// 'view' must have "main_window" as a child. (called from MainActivity)
void TopBar::tabNavigateRight(brls::View* view) {
    this->currently_selected_menu_idx = (this->currently_selected_menu_idx+1) % this->Menus.size();
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