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
    float loudness = 0.1;
    float maxLoudness = 0.1;
    Array<float> prevLoudness;
    Clara *clara;
};


#endif  // LOUDNESSCOMPONENT_H_INCLUDED
