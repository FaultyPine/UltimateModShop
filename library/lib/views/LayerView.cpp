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

#include <borealis/views/LayerView.h>

namespace brls
{

LayerView::LayerView()
{
}

LayerView::~LayerView()
{
    for (unsigned int i = 0; i < this->layers.size(); i++)
        delete this->layers[i];

    this->layers.clear();
}

void LayerView::addLayer(brls::View* view)
{
    if (view)
    {
        this->layers.push_back(view);
        if (this->getLayersSize() == 1)
            this->changeLayer(0, true);
    }
}

void LayerView::changeLayer(int index, bool focus)
{
    if (index >= 0 && index < static_cast<int>(this->layers.size()))
    {
        brls::Application::blockInputs();

        if (this->selectedIndex >= 0)
        {
            this->layers[this->selectedIndex]->willDisappear(true);
            this->layers[this->selectedIndex]->hide([]() {});
            this->removeView(this->layers[this->selectedIndex], false);
        }

        this->selectedIndex = index;
        this->addView(this->layers[this->selectedIndex]);
        this->layers[this->selectedIndex]->willAppear(true);
        this->layers[this->selectedIndex]->show([this, focus]() {
            if (focus)
                brls::Application::giveFocus(this->layers[this->selectedIndex]->getDefaultFocus());
            brls::Application::unblockInputs();
        });
                    brls::Application::giveFocus(this->layers[this->selectedIndex]->getDefaultFocus());


        this->layers[index]->invalidate();
    }

    if (index == -1)
    {
        if (this->selectedIndex > 0)
        {
            this->layers[this->selectedIndex]->willDisappear(true);
            this->layers[this->selectedIndex]->hide([]() {});
        }

        this->selectedIndex = index;
    }
}

int LayerView::getLayerIndex()
{
    return this->selectedIndex;
}

brls::View* LayerView::getDefaultFocus()
{
    if (isValidSelectedIdx())
    {
        brls::View* newFocus = this->layers[this->selectedIndex]->getDefaultFocus();
        if (newFocus)
        {
            return newFocus;
        }
    }

    return nullptr;
}

void LayerView::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    if (isValidSelectedIdx())
        this->layers[this->selectedIndex]->frame(ctx);
}


void LayerView::willAppear(bool resetState)
{
    if (isValidSelectedIdx())
        this->layers[this->selectedIndex]->willAppear(true);
}

void LayerView::willDisappear(bool resetState)
{
    if (isValidSelectedIdx())
        this->layers[this->selectedIndex]->willDisappear(true);
}

size_t LayerView::getLayersSize() {
    return this->layers.size();
}

bool LayerView::isValidSelectedIdx() {
    return this->selectedIndex >= 0 && this->selectedIndex < static_cast<int>(this->layers.size());
}

}