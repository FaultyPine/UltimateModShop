#include "bar_item.h"

BarItem::BarItem(std::string text) {
    this->inflateFromXMLRes("xml/views/bar_item.xml");
    brls::Label* bar_item_label = (brls::Label*)this->getView("top_bar/bar_item_label");
    bar_item_label->setText(text);
}

brls::View* BarItem::create(std::string text)
{
    return new BarItem(text);
}