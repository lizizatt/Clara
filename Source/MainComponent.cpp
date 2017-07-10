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
    setSize (600, 400);
	startTimer(100);
    
    setAudioChannels(0, 2);
}

MainContentComponent::~MainContentComponent()
{
    shutdownAudio();
	stopTimer();
}

void MainContentComponent::paint (Graphics& g)
{
	Array<float> floatArr = clara->getExcitementBuffer();

	g.fillAll(Colours::darkgrey);
	g.setColour(Colours::grey);

	g.drawRect(20, 20, getWidth() - 40, getHeight() - 40);
	
    float max = -10000;
	for (int i = 0; i < floatArr.size(); i++) {
		max = floatArr[i] > max ? floatArr[i] : max;
	}

	float hBase = getHeight() - 40;
	int bottom = getHeight() - 20;

	if (floatArr.size() > 0) {
		for (int i = 1; i < floatArr.size() + 1; i++) {
			float x = (getWidth() - 40) * (double) i / ((double)floatArr.size() + 2) + 20;
			g.setColour(Colours::red);
			float h = hBase * (floatArr[i - 1] / max);
			g.drawLine(x, bottom, x, bottom - h);
		}
	}
}

void MainContentComponent::resized()
{

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
