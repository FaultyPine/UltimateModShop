#pragma once

#include <borealis.hpp>
#include "submission_node.h"
#include "gb.h"
#include "ums_utils.h"

class Browse : public brls::Box
{
    public:
    Browse();
    
    static brls::Box* create();

    private:
    gb::GbSubmissions new_submissions;

    void getNewGbSubmissions();
};