/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent(Clara *clara)
	: clara(clara)
{
	startTimer(100);
    
    setAudioChannels(0, 2);
    
    addAndMakeVisible(intervalScoreComponent = new IntervalScoreComponent(clara));
    addAndMakeVisible(loudnessComponent = new LoudnessComponent(clara));
    addAndMakeVisible(emotionsComponent = new EmotionsComponent(clara));
    setSize (1200, 800);
}

MainContentComponent::~MainContentComponent()
{
    shutdownAudio();
	stopTimer();
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void MainContentComponent::resized()
{
    intervalScoreComponent->setBounds(20, 20, getWidth() / 2.0 - 20 - 5, getHeight() * 3.0 / 4.0 - 20 - 5);
    loudnessComponent->setBounds(20, intervalScoreComponent->getBottom() + 10, getWidth() / 2.0 - 20 - 5, getHeight() / 4.0 - 20 - 5);
    emotionsComponent->setBounds(intervalScoreComponent->getRight() + 10, 20, getWidth() / 2.0 - 20 - 5, getHeight() - 40);
}

void MainContentComponent::timerCallback()
{
	repaint();
}

void MainContentComponent::prepareToPlay(int samplesPerBlock, double sampleRate)
{
    DBG(String::formatted("Preparing to play, %d samples per block, sample rate %f", samplesPerBlock, sampleRate));
}

void MainContentComponent::getNextAudioBlock(const AudioSourceChannelInfo &buffer)
{
    clara->getNextAudioBlock(buffer);
}
