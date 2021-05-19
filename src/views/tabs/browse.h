#pragma once

#include <borealis.hpp>
#include "submission_node.h"
#include "gb.h"
#include "ums_utils.h"

class Browse : public brls::Box
{
    public:
    Browse();
    ~Browse();

    void onChildFocusGained(View* directChild, View* focusedView) override;
    void onChildFocusLost(View* directChild, View* focusedView) override;

    void initBrowseMenu();

    static brls::Box* create();

    private:
    gb::GbSubmissions new_submissions;

    void getNewGbSubmissions(int page);

    int prev_selected_submission_id = 0;
};