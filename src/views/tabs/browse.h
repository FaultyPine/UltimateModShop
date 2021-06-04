#pragma once

#include "submission_node.h"
#include "ums_utils.h"


/** 
 *      _______________________
 *      |  0   |   1   |   2  |
 *      |______|_______|______|
 *      |  3   |   4   |   5  |     <- submissions are indexed like this on every page
 *      |______|_______|______|
 *      |  6   |   7   |   8  |
 *      |______|_______|______|
 */

class Browse : public brls::Box
{
    public:

    Browse();
    ~Browse();

    void onChildFocusGained(View* directChild, View* focusedView) override;

    static brls::Box* create();

    private:

    const static int NUM_SUBMISSIONS_ROOT = 3;
    const int NUM_SUBMISSIONS_PER_PAGE = NUM_SUBMISSIONS_ROOT * NUM_SUBMISSIONS_ROOT;

    const int NUM_PRELOADED_PAGES = 2;
    void loadNextPage();

    void getNewGbSubmissions(int page);

    // scroll left or right
    void scroll(brls::FocusDirection dir);

    brls::LayerView* pages;

    // hold submission datas. Will be filled up NUM_SUBMISSIONS_PER_PAGE elements at a time. I.E. will always be divisible by that
    std::vector<gb::GbSubmission*> subs;

    int current_page = 1;
};