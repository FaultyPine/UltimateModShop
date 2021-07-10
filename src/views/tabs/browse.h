#pragma once

#include "submission_node.h"
#include "ums_utils.h"

struct Category {
    std::string name;
    int id;
    int parentCatId;
    std::string iconUrl;
    std::vector<Category*> subcategories;
};
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

    std::map<int, Category*> categories = {};

    // loads the current page (based on current_page)
    bool loadPage(int page, int category = -1);

    void loadCategoryFilters();

    void toggleSubmenu();

    bool show_submenu_category(brls::View* view, int curr_category);
    void submenu_back_inner(brls::View* v);

    void ResetPages();

    void getNewGbSubmissions(int page, int category);

    // scroll left or right
    void scroll(brls::FocusDirection dir);

    brls::LayerView* pages = nullptr;

    // hold submission datas. Will be filled up NUM_SUBMISSIONS_PER_PAGE elements at a time. I.E. will always be divisible by that
    std::vector<gb::GbSubmission*> subs = {};

    int current_page = 1;

    int current_category_filter = -1;
    int curr_parent_category = 0;
};