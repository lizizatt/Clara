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
    addAndMakeVisible(neuroComponent = new NeurotransmitterComponent(clara));
    setSize (1200, 800);
    
    clara->addListener(this);
}

MainContentComponent::~MainContentComponent()
{
    clara->removeListener(this);
    shutdownAudio();
	stopTimer();
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setColour(Colours::lightgrey);
    g.drawText(String::formatted("%.1f / %.1f seconds", pts, maxPts), getWidth() / 2.0 - 100, 5, 200, 15, Justification::centred);
}

void MainContentComponent::resized()
{
    intervalScoreComponent->setBounds(20, 20, getWidth() / 2.0 - 20 - 5, getHeight() * 3.0 / 4.0 - 20 - 5);
    loudnessComponent->setBounds(20, intervalScoreComponent->getBottom() + 10, getWidth() / 2.0 - 20 - 5, getHeight() / 4.0 - 20 - 5);
    neuroComponent->setBounds(intervalScoreComponent->getRight() + 10, 20, getWidth() / 2.0 - 20 - 5, getHeight() - 40);
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

void MainContentComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::PTSUpdateMessage *ptsMsg = dynamic_cast<Clara::PTSUpdateMessage*>(msg);
    if (ptsMsg != nullptr) {
        this->pts = (double) ptsMsg->pts / ptsMsg->sampleRate;
        this->maxPts = (double) ptsMsg->maxPts / ptsMsg->sampleRate;
        repaint();
    }
}
