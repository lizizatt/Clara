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
    
    anger = ImageCache::getFromMemory(BinaryData::anger_png, BinaryData::anger_pngSize);
    excitement = ImageCache::getFromMemory(BinaryData::excitement_png, BinaryData::excitement_pngSize);
    joy = ImageCache::getFromMemory(BinaryData::joy_png, BinaryData::joy_pngSize);
    fear = ImageCache::getFromMemory(BinaryData::scared_png, BinaryData::scared_pngSize);
    shame = ImageCache::getFromMemory(BinaryData::shame_png, BinaryData::shame_pngSize);
    distress = ImageCache::getFromMemory(BinaryData::distress_png, BinaryData::distress_pngSize);
    contempt = ImageCache::getFromMemory(BinaryData::disgust_png, BinaryData::disgust_pngSize);
    surprise = ImageCache::getFromMemory(BinaryData::surprised_png, BinaryData::surprised_pngSize);
    neutral = ImageCache::getFromMemory(BinaryData::neutral_png, BinaryData::neutral_pngSize);
    addAndMakeVisible(emojiComponent);
    emojiComponent.setImage(neutral);
    
    cubeImage = ImageCache::getFromMemory(BinaryData::thecube_png, BinaryData::thecube_pngSize);
    
    addMouseListener(this, true);
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
    
    
    if (showCube) {
        g.drawImage(cubeImage, space.getX(), space.getY(), space.getWidth(), space.getHeight(), 0, 0, cubeImage.getWidth(), cubeImage.getHeight());
        
        Colour r = Colours::red;
        Colour gre = Colours::green;
        Colour b = Colours::blue;
        //todo rgb coloring of dot (plus alpha)
        
        g.setColour(Colours::cyan.withAlpha((float) fmax(.5, dopamineChart.getCurrent())));
        
        Colour emotionalColor = Colour::fromFloatRGBA(noraldrenalineChart.getCurrent(), serotoninChart.getCurrent(), dopamineChart.getCurrent(), 1.0);
        int circleSize = 12;
        g.setColour(Colours::white);
        g.fillEllipse(posX - circleSize / 2, posY - circleSize / 2, circleSize, circleSize);
        g.setColour(emotionalColor);
        g.fillEllipse(posX - circleSize / 2 + 1, posY - circleSize / 2 + 1, circleSize - 2, circleSize - 2);
    }
}

void NeurotransmitterComponent::resized()
{
    int h = 80;
    serotoninChart.setBounds(20, 20, getWidth() - 40, h);
    dopamineChart.setBounds(20, serotoninChart.getBottom() + 20, getWidth() - 40, h);
    noraldrenalineChart.setBounds(20, dopamineChart.getBottom() + 20, getWidth() - 40, h);
    if (showCube) {
        emojiComponent.setBounds(20, noraldrenalineChart.getBottom() + 20, 30, 30);
    }
    else {
        emojiComponent.setBounds(20, noraldrenalineChart.getBottom() + 20, getWidth() - 40, getWidth() - 40);
    }
}

void NeurotransmitterComponent::mouseUp(const MouseEvent &me)
{
    showCube = !showCube;
    repaint();
    resized();
}

void NeurotransmitterComponent::pickEmoji()
{
    float s = serotoninChart.getCurrent();
    float d = dopamineChart.getCurrent();
    float n = noraldrenalineChart.getCurrent();
    
    Image pick;
    if (.6 > s && s > .4
        && .6 > d && d > .4
        && .5 > n && n > .15) {
        pick = neutral;
    }
    else if (s > .5) {
        if (d > .5) {
            if (n > .5) {
                pick = excitement;
            }
            else {
                pick = joy;
            }
        }
        else {
            if (n > .5) {
                pick = surprise;
            }
            else {
                pick = contempt;
            }
        }
    }
    else {
        if (d > .5) {
            if (n > .5) {
                pick = anger;
            }
            else {
                pick = fear;
            }
        }
        else {
            if (n > .5) {
                pick = distress;
            }
            else {
                pick = shame;
            }
        }
    }
    assert(pick.isValid());
    emojiComponent.setImage(pick);
}

void NeurotransmitterComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    bool needsRepaint = false;
    
    Clara::SerotoninUpdateMessage* seroUpdate = dynamic_cast<Clara::SerotoninUpdateMessage*>(msg);
    if (seroUpdate != nullptr) {
        serotoninChart.addItem(seroUpdate->serotonin);
        needsRepaint = true;
    }
    Clara::DopamineUpdateMessage* dopaUpdate = dynamic_cast<Clara::DopamineUpdateMessage*>(msg);
    if (dopaUpdate != nullptr) {
        dopamineChart.addItem(dopaUpdate->dopamine);
        needsRepaint = true;
    }
    Clara::NoradrenalineUpdateMessage* noraUpdate = dynamic_cast<Clara::NoradrenalineUpdateMessage*>(msg);
    if (noraUpdate != nullptr) {
        noraldrenalineChart.addItem(noraUpdate->noradrelaine);
        needsRepaint = true;
    }
    
    if (needsRepaint) {
        pickEmoji();
        repaint();
    }
}
