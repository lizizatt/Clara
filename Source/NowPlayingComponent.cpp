/*
  ==============================================================================

    NowPlayingComponent.cpp
    Created: 13 Jul 2017 1:16:56pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "NowPlayingComponent.h"

NowPlayingComponent::NowPlayingComponent(Clara *clara)
: clara(clara)
{
    currentlyListeningToLabel.setColour(Label::ColourIds::textColourId, Colours::white);
    currentlyListeningToLabel.setText("Currently listening to:", dontSendNotification);
    
    currentlyListeningToValue.setColour(Label::ColourIds::textColourId, Colours::white);
    currentlyListeningToValue.setText("Nothing", dontSendNotification);
    
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    
    addAndMakeVisible(currentlyListeningToLabel);
    addAndMakeVisible(currentlyListeningToValue);
    addAndMakeVisible(stopButton);
}

void NowPlayingComponent::paint(Graphics &g)
{
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
}

void NowPlayingComponent::resized()
{
    currentlyListeningToLabel.setBounds(20, 20, getWidth() - 40, 20);
    currentlyListeningToValue.setBounds(currentlyListeningToLabel.getX() + 20, currentlyListeningToLabel.getBottom() + 5, getWidth() - 60, 20);
    stopButton.setBounds(currentlyListeningToValue.getX(), currentlyListeningToValue.getBottom() + 5, 100, 20);
}

void NowPlayingComponent::handleMessage(const Message &m)
{
}

void NowPlayingComponent::buttonClicked(Button *b)
{
    if (b == &stopButton) {
        clara->stopSong();
    }
}
