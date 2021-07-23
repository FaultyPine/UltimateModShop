#pragma once

#include "ums_utils.h"

typedef std::function<void()> PopupConfirmationCallback;


class Popup : public brls::Box {
    public:

    Popup(std::string msg, PopupConfirmationCallback cb = {});
    ~Popup();

    void registerCustomCallback(PopupConfirmationCallback cb);

    private:

    bool onConfirmClick(brls::View* v);
    bool onDenyClick(brls::View* v);

    PopupConfirmationCallback callback;
};