/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <borealis.hpp>
#include <vector>

namespace brls
{

// A view containing multiple children views with the ability to freely switch between these layers
class LayerView : public brls::Box
{
  public:
    LayerView();
    ~LayerView();

    void addLayer(brls::View* view, bool focusIfFirst = true);
    void changeLayer(int index, bool focus = false);
    void removeLayer(int index, bool should_free);
    void clearLayers();
    brls::View* getLayer(std::string id);
    brls::View* getLayer(int idx);
    int getLayerIndex();
    size_t getLayersSize();

    brls::View* getDefaultFocus() override;

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;

    void willAppear(bool resetState = false) override;
    void willDisappear(bool resetState = false) override;

    static brls::View* create();

  private:
    bool isValidSelectedIdx();
    std::vector<brls::View*> layers;
    int selectedIndex = 0;
};

}