/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara/Clara.h"

#include "IntervalScoreComponent.h"
#include "LoudnessComponent.h"
#include "EmotionsComponent.h"
#include "NeurotransmitterComponent.h"


class MainContentComponent   : public AudioAppComponent, public Timer, public MessageListener
{
public:
    //==============================================================================
    MainContentComponent(Clara *clara);
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void prepareToPlay(int samplesPerBlock, double sampleRate) override;
    void releaseResources() override {};
    void getNextAudioBlock(const AudioSourceChannelInfo &buffer);
    void handleMessage(const Message &m) override;

	void timerCallback() override;

private:
    ScopedPointer<IntervalScoreComponent> intervalScoreComponent;
    ScopedPointer<LoudnessComponent> loudnessComponent;
    ScopedPointer<EmotionsComponent> emotionsComponent;
    ScopedPointer<NeurotransmitterComponent> neuroComponent;
    
    double pts;
    double maxPts;
    
	Clara *clara = nullptr;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
