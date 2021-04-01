#include "bar_item.h"

BarItem::BarItem(std::string text, brls::Box* box) {
    this->inflateFromXMLRes("xml/views/bar_item.xml");
    this->tab_box = box;
    brls::Label* bar_item_label = (brls::Label*)this->getView("top_bar/bar_item_label");
    bar_item_label->setText(text);
}

BarItem::BarItem(std::string text) {
    this->inflateFromXMLRes("xml/views/bar_item.xml");
    brls::Label* bar_item_label = (brls::Label*)this->getView("top_bar/bar_item_label");
    bar_item_label->setText(text);
}

brls::Box* BarItem::getTabView() {
    if (!this->tab_box)
        brls::Logger::error("Attempting to get tab view on nullptr!");
    return this->tab_box;
}