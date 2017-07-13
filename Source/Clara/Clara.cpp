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
const int Clara::JUMP_COUNT = 10;

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
    musicHormoneNode = new MusicHormoneNode(this);
    neurotransmitterManagerNode = new NeurotransmitterManagerNode(this);
}

void Clara::run()
{
	DBG("Loading file");
	FileInputStream *stream = new FileInputStream(File("~/Clara/Resources/imperialguard.mp3"));

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
    
    Time lastUpdate = Time::getCurrentTime();

	//run
	DBG("Running");
    for (int i = 0; !threadShouldExit(); i++) {
        //grab audio and run nodes to process it
        
        if (true) {
            ScopedLock lock(audioBufferSection);
            readyToPlayAudio = true;
            
            long stride = fmin(stepRate, reader->lengthInSamples - pts);
            myBuffer->clear();
            reader->read(myBuffer, 0, stride, pts, false, false);
            pts += stride;
            numSamples = stride;
        }
        
        Node::runAllNodes();
        
        currentSongAvgS = currentSongAvgS * .99 + serotoninLevel * .01;
        currentSongAvgD = currentSongAvgD * .99 + dopamineLevel * .01;
        currentSongAvgN = currentSongAvgN * .99 + noradrenalineLevel * .01;
        
        if (intervalGeneratorNode->tickCount % JUMP_COUNT == 0) {
            postMessage(new PTSUpdateMessage(pts, reader->lengthInSamples, reader->sampleRate));
            postMessage(new AverageNeutransmitterValues(currentSongAvgS, currentSongAvgD, currentSongAvgN));
        }
        
        
        wait(10);
	}
    
    DBG(String::formatted("Finished song with S %f, D %f, N %f", currentSongAvgS, currentSongAvgD, currentSongAvgN));
    readyToPlayAudio = false;
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

void Clara::notifyNeurotransmitters()
{
    serotoninLevel = fmax(fmin(serotoninLevel, 1.0), 0.0);
    dopamineLevel = fmax(fmin(dopamineLevel, 1.0), 0.0);
    noradrenalineLevel = fmax(fmin(noradrenalineLevel, 1.0), 0.0);
    
    postMessage(new Clara::SerotoninUpdateMessage(serotoninLevel));
    postMessage(new Clara::DopamineUpdateMessage(dopamineLevel));
    postMessage(new Clara::NoradrenalineUpdateMessage(noradrenalineLevel));
}
