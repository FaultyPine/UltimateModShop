#pragma once

#include "ums_utils.h"
#include "submission_node.h"

struct PreviewMediaContainer {
    brls::Image* img;
    std::string caption;
};

class ModPage : public brls::Activity {
    public:
    
    ModPage(SubmissionNode* sub);
    ~ModPage();

    void onContentAvailable() override;
    brls::View* createContentView() override { return v; }
    brls::View* v;

    private:
    const int num_screenshot_dots_per_row = 10;
    int screenshot_idx = 0;
    SubmissionNode* submission; // don't free, is used for browse menu.
    brls::LayerView* screenshots_layers; // don't free, is freed by brls on remove.
    std::vector<PreviewMediaContainer*> medias;

    void setupModPage();
    void screenshotsScroll(brls::FocusDirection dir);

};