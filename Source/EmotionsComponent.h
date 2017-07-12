/*
  ==============================================================================

    EmotionsComponent.h
    Created: 10 Jul 2017 4:25:58pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef EMOTIONSCOMPONENT_H_INCLUDED
#define EMOTIONSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara/Clara.h"
#include "BacklookChart.h"

//==============================================================================
/*
*/
class EmotionsComponent    : public Component, public MessageListener
{
public:
    EmotionsComponent(Clara *clara);
    ~EmotionsComponent();

    void paint (Graphics&);
    void resized();
    
    void handleMessage(const Message &m) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmotionsComponent)
    
    Clara *clara;
    
    BacklookChart happinessChart;
};


#endif  // EMOTIONSCOMPONENT_H_INCLUDED
