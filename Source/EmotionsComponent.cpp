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
: clara(clara)
{
    clara->addListener(this);
}

EmotionsComponent::~EmotionsComponent()
{
    clara->removeListener(this);
}

void EmotionsComponent::paint (Graphics& g)
{
}

void EmotionsComponent::resized()
{

}

void EmotionsComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::IntervalGenerator::IntervalGeneratorOutput* intervalOut = dynamic_cast<Clara::IntervalGenerator::IntervalGeneratorOutput*>(msg);
    if (intervalOut != nullptr) {
    }
}
