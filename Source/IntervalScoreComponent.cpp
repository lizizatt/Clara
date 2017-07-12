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

    const StringArray rootNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    for (int i = 0; i < 12; i++) {
        backlookCharts.add(new BacklookChart(rootNames[i], Colours::cyan, MAX_LOOKBACK));
        addAndMakeVisible(backlookCharts[i]);
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
    
    int leftSide = 20;
    int yStart = 20;
    
    g.setColour(Colours::cyan);
    g.drawText(rootNames[root], leftSide, yStart - 12, 200, 12, Justification::left);
}

void IntervalScoreComponent::resized()
{
    for (int i = 0; i < 12; i++) {
        backlookCharts[i]->setBounds(20, 20 + (getHeight() - 40) / 12 * i, getWidth() - 40, (getHeight() - 40) / 12);
    }
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
            backlookCharts[i]->addItem(intervalOut->intervalPresenceWeights[i]);
        }
        
        repaint();
    }
}
