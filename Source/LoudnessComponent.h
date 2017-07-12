/*
  ==============================================================================

    LoudnessComponent.h
    Created: 10 Jul 2017 4:25:50pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef LOUDNESSCOMPONENT_H_INCLUDED
#define LOUDNESSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara/Clara.h"
#include "BacklookChart.h"

//==============================================================================
/*
*/
class LoudnessComponent    : public Component, public MessageListener
{
public:
    LoudnessComponent(Clara *clara);
    ~LoudnessComponent();

    void paint (Graphics&);
    void resized();
    
    void handleMessage(const Message &m) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessComponent)
    BacklookChart loudnessChart;
    Clara *clara;
};


#endif  // LOUDNESSCOMPONENT_H_INCLUDED
