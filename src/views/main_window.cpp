#include "main_window.h"

MainWindow::MainWindow() {
    this->inflateFromXMLRes("xml/views/main_window.xml");
    if (this->current_window)
        this->addView(this->current_window);
}


// this sucks and is basically a band-aid solution until the brls rewrite is more fleshed out and LayerView is available again
/// Set the window displayed by main_window - used in conjunction with TopBar. 'box' is the Box that should be the new current window
void MainWindow::setCurrentWindow(brls::Box* box) {
    // set owner of passed in node to nothing so that it can be properly set when added
    if (box->getYGNode()->getOwner())
        box->getYGNode()->setOwner(nullptr);

    // hide previous contents of current_window
    for (brls::View* v : this->current_window->getChildren()) {
        v->hide(stub);
        // idk why this even works but it does so yay... maybe becuause when the view is re-shown it resets its pos???
        v->setPositionType(brls::PositionType::ABSOLUTE);
        v->setPositionTop(0.0);
        v->setPositionBottom(0.0);
        v->setPositionLeft(0.0);
        v->setPositionRight(0.0);
    }
    brls::Application::giveFocus(nullptr);

    if (this->current_window->getView(box->getID())) { // view already exists in MainWindow, show it
        box->show(stub);
        brls::Application::giveFocus(box);
    }
    else { // view doesn't exist in MainWindow, add it in
        this->current_window->addView(box);
    }
}

brls::View* MainWindow::create() {
    return new MainWindow();
}
