#pragma once

#include "submission_node.h"
#include "ums_utils.h"

struct Category {
    std::string name;
    int id;
    int parentCatId;
    std::string iconUrl;
    std::vector<Category*> subcategories;
    int childLayerIdx;
    int parentLayerIdx;
};

typedef std::unordered_map<int, Category*> CategoryMap;

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

    CategoryMap categories = {};

    // loads the current page (based on current_page)
    bool loadPage(int page, int category = -1, const std::string& search = "");

    void loadCategoryFilters();

    // Category Filter Submenu Actions
    void onClickCategoryFilterInner(brls::View* view);
    void onSubmenuBackInner(brls::View* view);
    void onExpandSubmenuInner(brls::View* view);

    void toggleSubmenu();

    // Search Bar
    bool onSearchBarClick(brls::View* view);
    void handleSearchBarInput(const std::string& s);

    // returns false when 0 submissions were found for that search query
    bool getNewGbSubmissions(int page, int category, const std::string& search);

    // scroll left or right
    void scroll(brls::FocusDirection dir);

    brls::LayerView* pages = nullptr;

    // hold submission datas. Will be filled up NUM_SUBMISSIONS_PER_PAGE elements at a time. I.E. will always be divisible by that
    std::vector<gb::GbSubmission*> subs = {};


    // properties that can influence `loadPage`
    int current_page = 1;

    int current_category_filter = -1;

    std::string search_txt = "";
};