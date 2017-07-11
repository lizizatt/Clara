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
    /*
    Array<float> floatArr = clara->getExcitementBuffer();
    
    g.fillAll(Colours::darkgrey);
    g.setColour(Colours::grey);
    
    g.drawRect(20, 20, getWidth() - 40, getHeight() - 40);
    
    float max = -10000;
    for (int i = 0; i < floatArr.size(); i++) {
        max = floatArr[i] > max ? floatArr[i] : max;
    }
    
    float hBase = getHeight() - 40;
    int bottom = getHeight() - 20;
    
    if (floatArr.size() > 0) {
        for (int i = 1; i < floatArr.size() + 1; i++) {
            float x = (getWidth() - 40) * (double) i / ((double)floatArr.size() + 2) + 20;
            g.setColour(Colours::red);
            float h = hBase * (floatArr[i - 1] / max);
            g.drawLine(x, bottom, x, bottom - h);
        }
    }
     */
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
