/*
  ==============================================================================

    NeurotransmitterComponent.h
    Created: 12 Jul 2017 2:03:43pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef NEUROTRANSMITTERCOMPONENT_H_INCLUDED
#define NEUROTRANSMITTERCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara.h"
#include "BacklookChart.h"

//==============================================================================
/*
*/
class NeurotransmitterComponent    : public Component, public MessageListener
{
public:
    NeurotransmitterComponent(Clara *clara);
    ~NeurotransmitterComponent();

    void paint (Graphics&);
    void resized();
    void pickEmoji();
    
    void mouseUp(const MouseEvent &me) override;
    
    void handleMessage(const Message &m) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeurotransmitterComponent)
    
    BacklookChart serotoninChart;
    BacklookChart dopamineChart;
    BacklookChart noraldrenalineChart;

    Image cubeImage;
    Clara *clara;
    
    ImageComponent emojiComponent;
    Image anger;
    Image excitement;
    Image joy;
    Image fear;
    Image distress;
    Image shame;
    Image contempt;
    Image surprise;
    Image neutral;
    
    bool showCube = true;
};


#endif  // NEUROTRANSMITTERCOMPONENT_H_INCLUDED
