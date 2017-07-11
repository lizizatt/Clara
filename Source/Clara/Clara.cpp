/*
  ==============================================================================

    Clara.cpp
    Created: 28 Jun 2017 6:22:44pm
    Author:  Liz

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Clara.h"

const double Clara::tick_seconds = 0.1;
const int Clara::LOOKBACK_LIMIT = 50;

//==============================================================================
Clara::Clara()
	: Thread("Clara")
{
}

Clara::~Clara()
{
}

void Clara::setUpNodes()
{
    intervalGeneratorNode = new IntervalGenerator(this, myBuffer);
    loudnessMetricNode = new LoudnessMetric(this, myBuffer);
}

void Clara::run()
{
	DBG("Loading file");
	FileInputStream *stream = new FileInputStream(File("~/Clara/Resources/repost.mp3"));

    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
	ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(stream);


	nChannels = reader->numChannels;
	int stepRate = 512;
	long pts = 0;
    sampleRate = reader->sampleRate;
    
    myBuffer = new AudioSampleBuffer(nChannels, stepRate);
    myBuffer->clear();
    
    //setup
    setUpNodes();

	int nToRun = 10000;
	Time start = Time::getCurrentTime();
	Time lastCheck = start;
    
	//run
	DBG("Running");
    for (int i = 0; i < nToRun && !threadShouldExit(); i++) {
        
        //grab audio and run nodes to process it
		long currentPtsCap = fmin((Time::getCurrentTime() - start).inSeconds() * reader->sampleRate, reader->lengthInSamples) + reader->sampleRate;
		while (pts < currentPtsCap) {
            if (true) {
                ScopedLock lock(audioBufferSection);
                readyToPlayAudio = true;
                int stride = fmin(stepRate, reader->lengthInSamples - pts);
                myBuffer->clear();
                reader->read(myBuffer, 0, stride, pts, false, false);
                pts += stride;
                numSamples = stride;
            }
            
            lastCheck = Time::getCurrentTime();
            Node::runAllNodes();
        }

		wait(5);
	}
    readyToPlayAudio = false;

	delete samples;
}

void Clara::runNodeOutputs()
{
}

void Clara::getNextAudioBlock(const juce::AudioSourceChannelInfo &outputBuffer)
{
	ScopedLock lock(audioBufferSection);
    
    for (int c = 0; readyToPlayAudio && c < outputBuffer.buffer->getNumChannels(); c++) {
        outputBuffer.buffer->copyFrom(c, outputBuffer.startSample, *myBuffer, c, outputBuffer.startSample, outputBuffer.numSamples);
    }
}

Array<float> Clara::getExcitementBuffer()
{
	return excitementBuffer;
}
