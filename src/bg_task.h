#pragma once

#include "ums_utils.h"
#include "borealis/core/time.hpp"

class BgTask : public brls::RepeatingTask {
    public:

    BgTask(brls::Time period = 5000);
    ~BgTask();

    void addViewToQueue(brls::Box* parent, brls::View* toAdd);
    void pushActivityToQueue(brls::Activity* act);
    void pushCallbackToQueue(brls::TickingGenericCallback cb);

    void run() override;

    private:
    std::vector<brls::Activity*> activitiesToAdd;
    std::vector<std::pair<brls::Box*, brls::View*>> viewsToAdd;
    std::vector<brls::TickingGenericCallback> callbacksToAdd;

    std::mutex viewsToAddMutex;
    std::mutex activitiesToAddMutex;
    std::mutex callbacksToAddMutex;
};