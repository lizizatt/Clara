/*
  ==============================================================================

    NowPlayingComponent.h
    Created: 13 Jul 2017 1:16:56pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef NOWPLAYINGCOMPONENT_H_INCLUDED
#define NOWPLAYINGCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara/Clara.h"

class NowPlayingComponent : public Component, public MessageListener, public ButtonListener
{
public:
    NowPlayingComponent(Clara *clara);

    void paint(Graphics &g) override;
    void resized() override;
    
    void handleMessage(const Message &m) override;
    void buttonClicked(Button *b) override;
    
private:
    
    Label currentlyListeningToLabel;
    Label currentlyListeningToValue;
    TextButton stopButton;
    
    Clara *clara;
};


#endif  // NOWPLAYINGCOMPONENT_H_INCLUDED
