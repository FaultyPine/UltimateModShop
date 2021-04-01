#pragma once

#include <borealis.hpp>
#include "submission_node.h"
#include "ums_utils.h"

class Browse : public brls::Box
{
  public:
    Browse();
    
    static brls::Box* create();

};