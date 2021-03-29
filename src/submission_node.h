#pragma once

#include <borealis.hpp>

class SubmissionNode : public brls::Box
{
  public:
    SubmissionNode();

    static brls::View* create();
    
    private:
    bool onSubmissionNodeClicked(brls::View* view);
};