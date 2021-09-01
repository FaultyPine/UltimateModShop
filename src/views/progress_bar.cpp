#include "progress_bar.h"

brls::Box* download_progress_bar = nullptr;

// binds the above pointer to the current view's progress bar so that curl can properly update it
#define BIND_PROGRESS_BAR download_progress_bar = (brls::Box*)this->getView("download_progress_bar")

ProgressBar::~ProgressBar() {
    download_progress_bar = nullptr;
}

ProgressBar::ProgressBar(brls::Box* root) {
    this->inflateFromXMLRes("xml/views/progress_bar.xml");
    root->addView(this);
    BIND_PROGRESS_BAR;
}

void ProgressBar::onParentFocusGained(View* focusedView) {
    BIND_PROGRESS_BAR;
}

void ProgressBar::setTitle(std::string txt) {
    ((brls::Label*)this->getView("download_progress_label"))->setText(txt);
}

void ProgressBar::start(std::string title) {
    BIND_PROGRESS_BAR;
    brls::Label* download_progress_label = (brls::Label*)this->getView("download_progress_label");
    this->setVisibility(brls::Visibility::VISIBLE);

    download_progress_bar->setWidthPercentage(0.0);
    download_progress_label->setTextColor(nvgRGB(0, 0, 0));
    if (!title.empty())
        download_progress_label->setText(title);
}


void ProgressBar::finish(std::string title, NVGcolor color, std::function<bool(brls::View*)> cb) {
    brls::Label* download_progress_label = (brls::Label*)this->getView("download_progress_label");
    download_progress_label->setTextColor(color);
    download_progress_label->setText(title);
    brls::Application::giveFocus(this);
    this->registerClickAction(cb);
}

void ProgressBar::finish(std::string title, NVGcolor color) {
    brls::Label* download_progress_label = (brls::Label*)this->getView("download_progress_label");
    download_progress_label->setTextColor(color);
    download_progress_label->setText(title);
    brls::Application::giveFocus(this);
    this->registerClickAction([this](brls::View* v){
        this->defaultFinish();
        return true;
    });
}

void ProgressBar::finishImmediate(std::string title, NVGcolor color, std::function<bool(brls::View*)> cb) {
    brls::Label* download_progress_label = (brls::Label*)this->getView("download_progress_label");
    download_progress_label->setTextColor(color);
    download_progress_label->setText(title);
    brls::Application::giveFocus(this);
    cb(this);
}


void ProgressBar::defaultFinish() {
    if (this->hasParent())
        brls::Application::giveFocus(this->getParent());
    else
        brls::Application::giveFocus(nullptr);
    this->setVisibility(brls::Visibility::GONE);
}