/*
  ==============================================================================

    EmotionsComponent.cpp
    Created: 10 Jul 2017 4:25:58pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "EmotionsComponent.h"

//==============================================================================
EmotionsComponent::EmotionsComponent(Clara *clara)
: clara(clara), happinessChart("Happy / Sad", Colours::green, MAX_LOOKBACK)
{
    clara->addListener(this);
    addAndMakeVisible(happinessChart);
}

EmotionsComponent::~EmotionsComponent()
{
    clara->removeListener(this);
}

void EmotionsComponent::paint (Graphics& g)
{
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    
}

void EmotionsComponent::resized()
{
    happinessChart.setBounds(20, 20, getWidth() - 40, getHeight() - 40);
}

void EmotionsComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
}
