/*
  ==============================================================================

    LoudnessComponent.cpp
    Created: 10 Jul 2017 4:25:50pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoudnessComponent.h"

//==============================================================================
LoudnessComponent::LoudnessComponent(Clara *clara) : clara(clara)
{
    clara->addListener(this);
}

LoudnessComponent::~LoudnessComponent()
{
    clara->addListener(this);
}

void LoudnessComponent::paint (Graphics& g)
{
    g.setColour(Colours::grey);
    g.drawText("Loudness", getWidth() / 2 - 50, 10, 100, 20, Justification::centred);
    
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    
    int rightSide = getWidth() - 20;
    int leftSide = 20;
    int yStart = 20;
    int yEnd = getHeight() - 20;
    
    g.setColour(Colours::red);
    for (int i = 0; i < prevLoudness.size(); i++) {
        float h = (yEnd - yStart) * prevLoudness[i] / maxLoudness;
        int xpos = i * (rightSide - leftSide) / prevLoudness.size();
        g.drawLine(xpos, yEnd, xpos, yEnd - h, i == prevLoudness.size() - 1? 4 : 2);
    }
}

void LoudnessComponent::resized()
{
}

void LoudnessComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::LoudnessMetric::LoudnessMetricOutput* loudnessOut = dynamic_cast<Clara::LoudnessMetric::LoudnessMetricOutput*>(msg);
    if (loudnessOut != nullptr) {
        if (prevLoudness.size() > MAX_LOOKBACK) {
            prevLoudness.remove(0);
        }
        
        loudness = loudnessOut->loudness;
        prevLoudness.add(loudnessOut->loudness);
        
        maxLoudness = loudness > maxLoudness? loudness : maxLoudness;
        
        repaint();
    }
}
