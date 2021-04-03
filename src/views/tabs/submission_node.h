#pragma once

#include <borealis.hpp>
#include "gb.h"

class SubmissionNode : public brls::Box
{
  public:
    SubmissionNode(gb::GbSubmission* _submission);
    SubmissionNode();
    
    void setSubmissionData(gb::GbSubmission* s);
    gb::GbSubmission* getSubmissionData();

    void onFocusGained() override;
    void onFocusLost() override;

    private:

    gb::GbSubmission* submission;

    bool onSubmissionNodeClicked(brls::View* view);
};