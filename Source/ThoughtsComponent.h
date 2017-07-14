/*
  ==============================================================================

    ThoughtsComponent.h
    Created: 13 Jul 2017 3:00:52pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef THOUGHTSCOMPONENT_H_INCLUDED
#define THOUGHTSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ThoughtsComponent    : public Component
{
public:
    ThoughtsComponent();
    ~ThoughtsComponent();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThoughtsComponent)
};


#endif  // THOUGHTSCOMPONENT_H_INCLUDED
