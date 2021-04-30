#include "main_window.h"

MainWindow::~MainWindow() {
    if (this->layer_view)
        delete this->layer_view;
}

MainWindow::MainWindow() {
    brls::Logger::debug("Mainwindow ctor");
    this->inflateFromXMLRes("xml/views/main_window.xml");

    this->layer_view = new brls::LayerView();
    if (main_box) {
        TopBar* top_bar = (TopBar*)main_box->getView("top_bar");
        for (BarItem* b : top_bar->Menus) {
            this->addLayerView(b->getTabView());
        }
    }
    this->addView(this->layer_view);
}

void MainWindow::setLayer(int idx) {
    this->layer_view->changeLayer(idx, true);
}

void MainWindow::addLayerView(brls::View* v) {
    this->layer_view->addLayer(v);
}


brls::View* MainWindow::create() {
    return new MainWindow();
}