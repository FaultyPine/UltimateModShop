#pragma once

#include <borealis.hpp>
#include "borealis/core/time.hpp"

namespace BgTask {

    void Begin();
    void Destroy();

    void addViewToQueue(brls::Box* parent, brls::View* toAdd);
    void pushActivityToQueue(brls::Activity* act);
    void pushCallbackToQueue(brls::TickingGenericCallback cb);

    void run();
}; 

class BgTaskInner : public brls::RepeatingTask {
    public:
    BgTaskInner(brls::Time period = 2000);
    ~BgTaskInner();
    void run() override;
};