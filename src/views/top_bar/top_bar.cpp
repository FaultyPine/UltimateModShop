#include "top_bar.h"

void TopBar::setSelected(brls::View* selected_view) {
    selected_view->setBackgroundColor(nvgRGBf(0.5, 0.0, 0.1));
    selected_view->setHeight(selected_view->getHeight() + this->heightIncreaseOnSelect);
    selected_view->setWidth(selected_view->getWidth() + this->widthIncreaseOnSelect);
    //selected_view->setTranslationY(-this->heightIncreaseOnSelect);
}
void TopBar::unsetSelected(brls::View* previously_selected_view) {
    previously_selected_view->setBackgroundColor(nvgRGBf(1.0, 1.0, 1.0));
    previously_selected_view->setHeight(previously_selected_view->getHeight() - this->heightIncreaseOnSelect);
    previously_selected_view->setWidth(previously_selected_view->getWidth() - this->widthIncreaseOnSelect);
    //previously_selected_view->setTranslationY(this->heightIncreaseOnSelect);
}

TopBar::TopBar() {
    this->Menus = { new BarItem("Browse", Browse::create()), new BarItem("Installed", Installed::create()), new BarItem("Settings", Settings::create()) };
    this->inflateFromXMLRes("xml/views/top_bar.xml");
    for (int i = 0; i < this->Menus.size(); i++) {
        BarItem* baritem = this->Menus.at(i);
        if (i == this->currently_selected_menu_idx)
            setSelected(baritem);
        this->addView(baritem);
    }
}

TopBar::~TopBar() {
    for (BarItem* x : Menus) {
        delete x;
    }
}

/// 'view' must have "main_window" as a child. (called from MainActivity)
void TopBar::tabNavigateLeft(brls::View* view) {
    unsetSelected(this->Menus.at(this->currently_selected_menu_idx));
    this->currently_selected_menu_idx = this->currently_selected_menu_idx-1 < 0 ? this->Menus.size()-1 : this->currently_selected_menu_idx-1;
    setSelected(this->Menus.at(this->currently_selected_menu_idx));
    MainWindow* mainwindow = (MainWindow*)view->getView("main_window");
    mainwindow->setLayer(this->currently_selected_menu_idx);
}

/// 'view' must have "main_window" as a child. (called from MainActivity)
void TopBar::tabNavigateRight(brls::View* view) {
    unsetSelected(this->Menus.at(this->currently_selected_menu_idx));
    this->currently_selected_menu_idx = (this->currently_selected_menu_idx+1) % this->Menus.size();
    setSelected(this->Menus.at(this->currently_selected_menu_idx));
    MainWindow* mainwindow = (MainWindow*)view->getView("main_window");
    mainwindow->setLayer(this->currently_selected_menu_idx);
}

BarItem* TopBar::getCurrentMenu() {
    return this->Menus.at(this->currently_selected_menu_idx);
}

brls::View* TopBar::create()
{
    return new TopBar();
}