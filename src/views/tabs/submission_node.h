#pragma once

#include <borealis.hpp>
#include "gb.h"

class SubmissionNode : public brls::Box
{
  public:
    SubmissionNode(gb::GbSubmission* _submission);
    SubmissionNode();
    
    private:

    gb::GbSubmission* submission;

    bool onSubmissionNodeClicked(brls::View* view);
};