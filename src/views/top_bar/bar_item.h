#pragma once

#include <borealis.hpp>

const std::string placeholder_tab_box = R"xml(
    <brls:Box 
        id="placeholder_tab_box"
        width="100%"
        height="100%"
        borderThickness="3"
        borderColor="#1300FF"
    />
)xml";

class BarItem : public brls::Box
{
    public:
    /// Initialize each BarItem with the name of the tab, and the view that the tab should show
    BarItem(std::string text, brls::Box* box);
    BarItem(std::string text);

    /// Returns the brls::View* this BarItem "contains". nullptr if there is no associated view.
    brls::Box* getTabView();

    private:
    brls::Box* tab_box = (brls::Box*)brls::Box::createFromXMLString(placeholder_tab_box);
};