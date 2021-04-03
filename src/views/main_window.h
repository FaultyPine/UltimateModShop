#pragma once

#include <borealis.hpp>
#include "top_bar/top_bar.h"

const std::string main_window_initial = R"xml(
    <brls:Box 
        width="100%"
        height="100%"
    />
)xml";

class MainWindow : public brls::Box
{
    public:
    MainWindow();

    static brls::View* create();

    void setCurrentWindow(brls::Box* view);
    
    private:
    brls::Box* current_window = (brls::Box*)brls::Box::createFromXMLString(main_window_initial);
};