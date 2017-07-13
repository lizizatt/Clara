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
    addAndMakeVisible(nowPlayingComponent = new NowPlayingComponent(clara));
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
    g.drawText(String::formatted("%.1f / %.1f seconds      S %.2f; D %.2f; N %.2f", pts, maxPts, avgS, avgD, avgN), getWidth() / 2.0 - 150, 5, 300, 15, Justification::centred);
}

void MainContentComponent::resized()
{
    int leftCol = 300;
    nowPlayingComponent->setBounds(20, 20, leftCol, getHeight() - 40);
    
    Rectangle<int> mainPane(40 + leftCol, 20, getWidth() - 60 - leftCol, getHeight() - 40);
    intervalScoreComponent->setBounds(mainPane.getX() + 20, 20, mainPane.getWidth() / 2.0 - 20 - 5, mainPane.getHeight() * 3.0 / 4.0 - 20 - 5);
    loudnessComponent->setBounds(mainPane.getX() + 20, intervalScoreComponent->getBottom() + 10, mainPane.getWidth() / 2.0 - 20 - 5, mainPane.getHeight() / 4.0 - 20 - 5);
    neuroComponent->setBounds(intervalScoreComponent->getRight() + 10, 20, mainPane.getWidth() / 2.0 - 20 - 5, mainPane.getHeight() - 40);
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
    
    Clara::AverageNeutransmitterValues *overallUpdate = dynamic_cast<Clara::AverageNeutransmitterValues*>(msg);
    if (overallUpdate) {
        avgS = overallUpdate->sero;
        avgD = overallUpdate->dopa;
        avgN = overallUpdate->nora;
        repaint();
    }
}
