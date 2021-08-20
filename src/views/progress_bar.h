#pragma once

#include "ums_utils.h"


extern brls::Box* download_progress_bar;

class ProgressBar : public brls::Box {
    public:

    ProgressBar(brls::Box* root);
    ~ProgressBar();

    void setTitle(std::string txt);
    void defaultFinish();

    void start(std::string title = "");
    void finish(std::string title, NVGcolor color, std::function<bool(brls::View*)> cb = [](brls::View* v){ return true; });

    static brls::Box* getDownloadProgressBar() { return download_progress_bar; }

    void onParentFocusGained(View* focusedView) override;

    private:


};