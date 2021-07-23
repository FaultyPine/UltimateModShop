#include "popup.h"

Popup::~Popup() {

}

Popup::Popup(std::string msg, PopupConfirmationCallback cb) {
    this->callback = cb;
    this->inflateFromXMLRes("xml/views/popup.xml");
    brls::Label* popup_title_label = (brls::Label*)this->getView("popup_title_label");
    popup_title_label->setText(msg);

    BRLS_REGISTER_CLICK_BY_ID("popup_confirm", this->onConfirmClick);
    BRLS_REGISTER_CLICK_BY_ID("popup_deny", this->onDenyClick);
}

void afterButtonPress(brls::View* this_popup) {
    if (this_popup->hasParent()) {
        brls::Application::giveFocus(this_popup->getParent());
    }
    else {
        brls::Application::giveFocus(nullptr);
    }
    this_popup->setVisibility(brls::Visibility::GONE);
}

bool Popup::onConfirmClick(brls::View* v) {
    brls::Logger::debug("onConfirmClick");
    if (this->callback)
        this->callback();
    afterButtonPress(this);
    return true;
}
bool Popup::onDenyClick(brls::View* v) {
    brls::Logger::debug("onDenyClick");
    afterButtonPress(this);
    return true;
}

void Popup::registerCustomCallback(PopupConfirmationCallback cb) {
    this->callback = cb;
}
