#pragma once

#include <borealis.hpp>
#include "ums_utils.h"

class SubmissionNode : public brls::Box
{
  public:
    SubmissionNode(gb::GbSubmission* _submission);
    SubmissionNode();
    ~SubmissionNode();
    
    void setSubmissionData(gb::GbSubmission* s);
    gb::GbSubmission* getSubmissionData();

    void onFocusGained() override;
    void onFocusLost() override;

    void downloadSubmission();

    private:

    gb::GbSubmission* submission;

    bool onSubmissionNodeClicked(brls::View* view);

};