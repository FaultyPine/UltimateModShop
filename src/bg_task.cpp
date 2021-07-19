#include "bg_task.h"

namespace {
    std::vector<brls::Activity*> activitiesToAdd;
    std::vector<std::pair<brls::Box*, brls::View*>> viewsToAdd;
    std::vector<brls::TickingGenericCallback> callbacksToAdd;

    std::mutex viewsToAddMutex;
    std::mutex activitiesToAddMutex;
    std::mutex callbacksToAddMutex;

    BgTaskInner* bg_task_inner;
}

void BgTask::Begin() {
    bg_task_inner = new BgTaskInner();
    bg_task_inner->start();
}
void BgTask::Destroy() {
    bg_task_inner->stop();
    for (std::pair<brls::Box*, brls::View*> pair : viewsToAdd) {
        delete pair.first;
        delete pair.second;
    }
    viewsToAdd.clear();
    for (brls::Activity* a : activitiesToAdd) {
        delete a;
    }
    activitiesToAdd.clear();
    callbacksToAdd.clear();
}

void BgTask::addViewToQueue(brls::Box* parent, brls::View* toAdd) {
    viewsToAddMutex.lock();
    viewsToAdd.push_back(std::make_pair(parent, toAdd));
    viewsToAddMutex.unlock();
}

void BgTask::pushActivityToQueue(brls::Activity* act) {
    activitiesToAddMutex.lock();
    activitiesToAdd.push_back(act);
    activitiesToAddMutex.unlock();
}

void BgTask::pushCallbackToQueue(brls::TickingGenericCallback cb) {
    callbacksToAddMutex.lock();
    callbacksToAdd.push_back(cb);
    callbacksToAddMutex.unlock();
}

void BgTask::run() {
    if (!viewsToAdd.empty()) {
        viewsToAddMutex.lock();
        for (std::pair<brls::Box*, brls::View*> pair : viewsToAdd) {
            pair.first->addView(pair.second);
        }
        viewsToAdd.clear();
        viewsToAddMutex.unlock();
    }
    if (!activitiesToAdd.empty()) {
        activitiesToAddMutex.lock();
        for (brls::Activity* act : activitiesToAdd) {
            brls::Application::pushActivity(act);
        }
        activitiesToAdd.clear();
        activitiesToAddMutex.unlock();
    }
    if (!callbacksToAdd.empty()) {
        callbacksToAddMutex.lock();
        for (brls::TickingGenericCallback cb : callbacksToAdd) {
            cb();
        }
        callbacksToAdd.clear();
        callbacksToAddMutex.unlock();
    }
}




// BgTaskInner


BgTaskInner::BgTaskInner(brls::Time period) : brls::RepeatingTask(period) {
}
BgTaskInner::~BgTaskInner() {
    BgTask::Destroy();
}
void BgTaskInner::run() {
    BgTask::run();
}