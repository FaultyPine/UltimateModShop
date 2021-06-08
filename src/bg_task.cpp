#include "bg_task.h"


BgTask::BgTask(brls::Time period) : brls::RepeatingTask(period) {
    this->start();
}
BgTask::~BgTask() {
    this->stop();
    for (std::pair<brls::Box*, brls::View*> pair : this->viewsToAdd) {
        delete pair.first;
        delete pair.second;
    }
    this->viewsToAdd.clear();
    for (brls::Activity* a : this->activitiesToAdd) {
        delete a;
    }
    this->activitiesToAdd.clear();
    this->callbacksToAdd.clear();
}

void BgTask::addViewToQueue(brls::Box* parent, brls::View* toAdd) {
    this->viewsToAddMutex.lock();
    this->viewsToAdd.push_back(std::make_pair(parent, toAdd));
    this->viewsToAddMutex.unlock();
}

void BgTask::pushActivityToQueue(brls::Activity* act) {
    this->activitiesToAddMutex.lock();
    this->activitiesToAdd.push_back(act);
    this->activitiesToAddMutex.unlock();
}

void BgTask::pushCallbackToQueue(brls::TickingGenericCallback cb) {
    this->callbacksToAddMutex.lock();
    this->callbacksToAdd.push_back(cb);
    this->callbacksToAddMutex.unlock();
}

void BgTask::run() {
    if (!this->viewsToAdd.empty()) {
        this->viewsToAddMutex.lock();
        for (std::pair<brls::Box*, brls::View*> pair : this->viewsToAdd) {
            pair.first->addView(pair.second);
            this->viewsToAdd.erase(this->viewsToAdd.begin());
        }
        this->viewsToAddMutex.unlock();
    }
    if (!this->activitiesToAdd.empty()) {
        this->activitiesToAddMutex.lock();
        for (brls::Activity* act : this->activitiesToAdd) {
            brls::Application::pushActivity(act);
            this->activitiesToAdd.erase(this->activitiesToAdd.begin());
        }
        this->activitiesToAddMutex.unlock();
    }
    if (!this->callbacksToAdd.empty()) {
        this->callbacksToAddMutex.lock();
        for (brls::TickingGenericCallback cb : this->callbacksToAdd) {
            cb();
            this->callbacksToAdd.erase(this->callbacksToAdd.begin());
        }
        this->callbacksToAddMutex.unlock();
    }
}