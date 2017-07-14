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
LoudnessComponent::LoudnessComponent(Clara *clara)
: clara(clara), loudnessChart("Loudness", Colours::red, MAX_LOOKBACK, true), repetitivenessChart("Repetitivensss", Colours::orange, MAX_LOOKBACK, true)
{
    clara->addListener(this);
    addAndMakeVisible(loudnessChart);
    addAndMakeVisible(repetitivenessChart);
}

LoudnessComponent::~LoudnessComponent()
{
    clara->addListener(this);
}

void LoudnessComponent::paint (Graphics& g)
{
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
}

void LoudnessComponent::resized()
{
    int h = (getHeight() - 50) / 2;
    loudnessChart.setBounds(20, 20, getWidth() - 40, h);
    repetitivenessChart.setBounds(20, getHeight() / 2.0 + 5, getWidth() - 40, h);
}

void LoudnessComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::LoudnessMetric::LoudnessMetricOutput* loudnessOut = dynamic_cast<Clara::LoudnessMetric::LoudnessMetricOutput*>(msg);
    if (loudnessOut != nullptr) {
        loudnessChart.addItem(loudnessOut->loudness);
    }

    Clara::RepetitivenessNode::RepetitivenessNodeOutput* repetOut = dynamic_cast<Clara::RepetitivenessNode::RepetitivenessNodeOutput*>(msg);
    if (repetOut != nullptr) {
        repetitivenessChart.addItem(repetOut->repetitiveness);
    }
}
