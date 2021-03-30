#include "top_bar.h"

TopBar::TopBar() {
    this->inflateFromXMLRes("xml/views/top_bar.xml");

    for (BarItem* baritem : Menus) {
        this->addView(baritem);
    }
}

brls::View* TopBar::create()
{
    return new TopBar();
}