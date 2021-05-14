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
    
    static brls::Box* create();

    static void initBrowseMenu(Browse* browse);

    private:
    gb::GbSubmissions new_submissions;

    void getNewGbSubmissions(int page);
};