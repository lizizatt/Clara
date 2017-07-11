/*
  ==============================================================================

    IntervalScoreComponent.h
    Created: 10 Jul 2017 4:10:23pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef INTERVALSCORECOMPONENT_H_INCLUDED
#define INTERVALSCORECOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara/Clara.h"


//==============================================================================
/*
*/
class IntervalScoreComponent    : public Component, public MessageListener
{
public:
    IntervalScoreComponent(Clara *clara);
    ~IntervalScoreComponent();

    void paint (Graphics&);
    void resized();
    
    void handleMessage(const Message &m) override;

private:
    Clara *clara = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IntervalScoreComponent)
    
    OwnedArray<Array<float>> prevWeights;
    Array<float> intervals;
    int root;
};


#endif  // INTERVALSCORECOMPONENT_H_INCLUDED
