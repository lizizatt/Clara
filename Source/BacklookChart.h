/*
  ==============================================================================

    BacklookChart.h
    Created: 12 Jul 2017 12:16:37pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef BACKLOOKCHART_H_INCLUDED
#define BACKLOOKCHART_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class BacklookChart    : public Component
{
public:
    BacklookChart(String name, Colour c, int maxItems, bool unitRange = false);
    ~BacklookChart();

    void paint (Graphics&);
    void resized();
    
    void addItem(float newItem);
    float getCurrent() {return items.getLast();};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BacklookChart)
    String name;
    Colour c;
    Array<float> items;
    int maxSize = 10;
    float max = .01;
    bool unitRange = false;
};


#endif  // BACKLOOKCHART_H_INCLUDED
