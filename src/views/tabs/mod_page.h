#pragma once

#include "ums_utils.h"
#include "submission_node.h"

class ModPage : public brls::Activity {
    public:
    
    ModPage(SubmissionNode* sub);

    void onContentAvailable() override;
    brls::View* createContentView() override { return v; }
    brls::View* v;

    private:
    SubmissionNode* submission; // don't free

};