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
    int screenshot_idx = 0;
    SubmissionNode* submission; // don't free
    brls::LayerView* screenshots_layers; // doesn't need to be freed as long as you're adding it to some other view that gets freed
    std::vector<PreviewMediaContainer*> medias;

    void setupModPage();
    void screenshotsScroll(brls::FocusDirection dir);
    bool onInstallButtonClicked(brls::View* v);

};