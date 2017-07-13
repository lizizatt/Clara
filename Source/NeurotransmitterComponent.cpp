/*
  ==============================================================================

    NeurotransmitterComponent.cpp
    Created: 12 Jul 2017 2:03:43pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NeurotransmitterComponent.h"

//==============================================================================
NeurotransmitterComponent::NeurotransmitterComponent(Clara *clara)
: clara(clara), serotoninChart("Serotonin", Colours::aquamarine, MAX_LOOKBACK, true), dopamineChart("Dopamine", Colours::aquamarine, MAX_LOOKBACK, true), noraldrenalineChart("Noraldrenaline", Colours::aquamarine, MAX_LOOKBACK, true)
{
    clara->addListener(this);
    
    addAndMakeVisible(serotoninChart);
    addAndMakeVisible(noraldrenalineChart);
    addAndMakeVisible(dopamineChart);
    
    cubeImage = ImageCache::getFromMemory(BinaryData::thecube_png, BinaryData::thecube_pngSize);
}

NeurotransmitterComponent::~NeurotransmitterComponent()
{
    clara->removeListener(this);
}

void NeurotransmitterComponent::paint (Graphics& g)
{
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    
    int start = noraldrenalineChart.getBottom() + 20;
    Rectangle<int> space(0, start, getWidth(), getHeight() - start);
    
    g.drawImage(cubeImage, space.getX(), space.getY(), space.getWidth(), space.getHeight(), 0, 0, cubeImage.getWidth(), cubeImage.getHeight());
    
    float originX = space.getWidth() * .2 + space.getX();
    float originY = space.getHeight() * .9 + space.getY();
    
    float seraScaler = .39 * space.getWidth();
    float noraScaler = .54 * space.getHeight();
    
    float posX = originX + seraScaler * fmin(fmax(serotoninChart.getCurrent(), 0.0), 1.0);
    float posY = originY - noraScaler * fmin(fmax(noraldrenalineChart.getCurrent(), 0.0), 1.0);
    
    float dopamine = fmin(fmax(dopamineChart.getCurrent(), 0.0), 1.0);
    float dopaXScale = .17 * space.getWidth();
    float dopaYScale = .25 * space.getHeight();
    
    posX += (dopamine * dopaXScale);
    posY -= (dopamine * dopaYScale);
    
    g.setColour(Colours::cyan);
    int circleSize = (dopamine * .5 + .5) * 12;
    g.fillEllipse(posX - circleSize / 2, posY - circleSize / 2, circleSize, circleSize);
}

void NeurotransmitterComponent::resized()
{
    int h = 80;
    serotoninChart.setBounds(20, 20, getWidth() - 40, h);
    dopamineChart.setBounds(20, serotoninChart.getBottom() + 20, getWidth() - 40, h);
    noraldrenalineChart.setBounds(20, dopamineChart.getBottom() + 20, getWidth() - 40, h);
}

void NeurotransmitterComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::SerotoninUpdateMessage* seroUpdate = dynamic_cast<Clara::SerotoninUpdateMessage*>(msg);
    if (seroUpdate != nullptr) {
        serotoninChart.addItem(seroUpdate->serotonin);
        repaint();
    }
    Clara::DopamineUpdateMessage* dopaUpdate = dynamic_cast<Clara::DopamineUpdateMessage*>(msg);
    if (dopaUpdate != nullptr) {
        dopamineChart.addItem(dopaUpdate->dopamine);
        repaint();
    }
    Clara::NoradrenalineUpdateMessage* noraUpdate = dynamic_cast<Clara::NoradrenalineUpdateMessage*>(msg);
    if (noraUpdate != nullptr) {
        noraldrenalineChart.addItem(noraUpdate->noradrelaine);
        repaint();
    }
}
