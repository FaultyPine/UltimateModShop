#include "top_bar.h"


TopBar::TopBar() {
    this->inflateFromXMLRes("xml/tabs/top_bar.xml");
}

brls::View* TopBar::create()
{
    // Called by the XML engine to create a new ComponentsTab
    return new TopBar();
}