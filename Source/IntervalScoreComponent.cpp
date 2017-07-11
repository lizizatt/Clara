/*
  ==============================================================================

    IntervalScoreComponent.cpp
    Created: 10 Jul 2017 4:10:23pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "IntervalScoreComponent.h"

//==============================================================================
IntervalScoreComponent::IntervalScoreComponent(Clara *clara)
: clara(clara)
{
    clara->addListener(this);
    for (int i = 0; i < 12; i++) {
        prevWeights.add(new Array<float>);
    }
}

IntervalScoreComponent::~IntervalScoreComponent()
{
    clara->removeListener(this);
}

void IntervalScoreComponent::paint (Graphics& g)
{
    const Array<float> frequencies = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87};
    const StringArray rootNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    
    g.setColour(Colours::grey);
    g.drawText("Intervals", getWidth() / 2 - 50, 10, 100, 20, Justification::centred);
    
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    
    
    int rightSide = getWidth() - 20;
    int leftSide = 20;
    int yStart = 20;
    int yEnd = getHeight() - 20;
    
    g.setColour(Colours::cyan);
    g.drawText(rootNames[root], leftSide, yStart - 12, 200, 12, Justification::left);
    
    for (int i = 0; i < intervals.size(); i++) {
        int singleChartHeight = (yEnd - yStart) / intervals.size();
        int yPos = singleChartHeight * i + yStart;
        
        g.setColour(Colours::grey);
        g.drawText(String::formatted("%.2f", intervals[i]), leftSide, yPos + singleChartHeight - 20, 50, 20, Justification::left);
        g.drawLine(leftSide, yPos + singleChartHeight / 2, rightSide, yPos + singleChartHeight / 2);
        
        for (int j = 0; j < prevWeights[i]->size(); j++) {
            int widthStep = (rightSide - leftSide) / prevWeights[i]->size();
            int xPos = leftSide + widthStep * j;
            int h = singleChartHeight / 2 * (*prevWeights[i])[j];
            g.setColour(Colours::cyan);
            g.drawLine(xPos, yPos + singleChartHeight / 2, xPos, yPos + singleChartHeight / 2 - fmin(h, singleChartHeight));
        }
    }
}

void IntervalScoreComponent::resized()
{
}

void IntervalScoreComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::IntervalGenerator::IntervalGeneratorOutput* intervalOut = dynamic_cast<Clara::IntervalGenerator::IntervalGeneratorOutput*>(msg);
    if (intervalOut != nullptr) {
        
        if (intervals.size() == 0) {
            intervals = intervalOut->intervals;
        }
        
        root = intervalOut->rootIndex;
        
        for (int i = 0; i < intervalOut->intervalPresenceWeights.size(); i++) {
            if (prevWeights[i]->size() > MAX_LOOKBACK) {
                prevWeights[i]->remove(0);
            }
            prevWeights[i]->add(intervalOut->intervalPresenceWeights[i]);
        }
        
        repaint();
    }
}
