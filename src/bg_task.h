#pragma once

#include <borealis.hpp>
#include "borealis/core/time.hpp"

namespace BgTask {

    void Begin(brls::Time period);
    void Destroy();

    void addViewToQueue(brls::Box* parent, brls::View* toAdd);
    void pushActivityToQueue(brls::Activity* act);
    void pushCallbackToQueue(brls::TickingGenericCallback cb);

    void run();
}; 

class BgTaskInner : public brls::RepeatingTask {
    public:
    BgTaskInner(brls::Time period);
    ~BgTaskInner();
    void run() override;
};