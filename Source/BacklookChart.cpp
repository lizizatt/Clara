/*
  ==============================================================================

    BacklookChart.cpp
    Created: 12 Jul 2017 12:16:37pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BacklookChart.h"

//==============================================================================
BacklookChart::BacklookChart(String name, Colour c, int maxItems, bool unitRange)
: name(name), c(c), maxSize(maxItems), unitRange(unitRange)
{
    if (unitRange) {
        max = 1.0;
    }
}

BacklookChart::~BacklookChart()
{
}

void BacklookChart::paint (Graphics& g)
{
    g.setColour(Colours::grey);
    g.drawText(name, getWidth() / 2 - 50, 0, 100, 20, Justification::centred);
    
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    
    int yMid, maxH;
    if (unitRange) {
        yMid = getHeight() - 10;
        maxH = getHeight() - 20;
    }
    else {
        yMid = getHeight() / 2.0;
        maxH = getHeight() / 2.0 - 10;
    }
    
    g.drawLine(0, yMid, getWidth(), yMid);
    
    g.setColour(c);
    for (int i = 0; i < items.size(); i++) {
        float h = maxH * items[i] / max;
        int xpos = i * getWidth() / maxSize;
        g.drawLine(xpos, yMid, xpos, yMid - h, i == items.size() - 1? 4 : 2);
    }
    
    g.setColour(Colours::grey);
    g.setFont(g.getCurrentFont().withHeight(8.0));
    g.drawText(String::formatted("(%.2f)", items.getLast()), getWidth() - 50, getHeight() - 10, 48, 8, Justification::right);
}

void BacklookChart::resized()
{
}

void BacklookChart::addItem(float newItem)
{
    newItem = unitRange? fmax(fmin(1.0, newItem), 0.0) : newItem;
    
    items.add(newItem);
    if (items.size() > maxSize) {
        items.remove(0);
    }
    
    if (fabs(newItem) > max) {
        max = fabs(newItem);
    }
    
    repaint();
}
