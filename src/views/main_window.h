#pragma once

#include <borealis.hpp>
#include "top_bar/top_bar.h"
#include "top_bar/bar_item.h"

class MainWindow : public brls::Box
{
    public:
    MainWindow();
    ~MainWindow();

    static brls::View* create();

    void setLayer(int idx);
    void addLayerView(brls::View* v);
    
    brls::LayerView* layer_view;

    private:
};