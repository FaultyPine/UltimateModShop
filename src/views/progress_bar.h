#pragma once

#include "ums_utils.h"


extern brls::Box* download_progress_bar;

class ProgressBar : public brls::Box {
    public:

    ProgressBar(brls::Box* root);
    ~ProgressBar();

    void setTitle(std::string txt);

    void start(std::string title = "");
    /// sets title, sets title color, gives focus to this progress bar, and registers cb as the click-callback
    void finish(std::string title, NVGcolor color, std::function<bool(brls::View*)> cb);
    /// uses default cb for clicking which hides progress bar and gives focus to parent or nullptr
    void finish(std::string title, NVGcolor color);
    /// finishes as normal but calls cb immediately
    void finishImmediate(std::string title, NVGcolor color, std::function<bool(brls::View*)> cb);



    static brls::Box* getDownloadProgressBar() { return download_progress_bar; }

    void onParentFocusGained(View* focusedView) override;

    void defaultFinish();
    
    private:


};