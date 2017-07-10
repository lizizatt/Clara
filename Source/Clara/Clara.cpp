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
const int Clara::LOOKBACK_LIMIT = 10;

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
	audioSumNode = new FloatSumNode();
	nodes.add(audioSumNode);

    Array<float> frequencies = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87};
    
    for (int i = 0; i < frequencies.size(); i++) {
		AudioFrequencySourceNode *audNode = new AudioFrequencySourceNode(frequencies[i]);
		audioSumNode->inputNodes.add(audNode);
		earNodes.add(audNode);
		nodes.add(audNode);
	}
}

void Clara::run()
{
	DBG("Loading file");
	FileInputStream *stream = new FileInputStream(File("~/Clara/Resources/repost.mp3"));

    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
	ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(stream);

	//setup
	setUpNodes();

	nChannels = reader->numChannels;
	int stepRate = 1024;
	long pts = 0;
    
    myBuffer = new AudioSampleBuffer(nChannels, stepRate);
    myBuffer->clear();

	FFT::Complex* inputToFFT = (FFT::Complex*)malloc(sizeof(FFT::Complex) * stepRate);
	FFT::Complex* outputFromFFT = (FFT::Complex*)malloc(sizeof(FFT::Complex) * stepRate);
	FFT fft(10, false);

	int nToRun = 10000;
	double length = (double) reader->lengthInSamples / (double) reader->sampleRate;
	Time start = Time::getCurrentTime();
	Time lastCheck = start;
    
	//run
	DBG("Running");
	for (int i = 0; i < nToRun && !threadShouldExit(); i++) {
		//catch up on audio
		long currentPtsCap = fmin((Time::getCurrentTime() - start).inSeconds() * reader->sampleRate, reader->lengthInSamples) + reader->sampleRate;
		while (pts < currentPtsCap) {
			ScopedLock lock(audioBufferSection);
            readyToPlayAudio = true;
			int stride = fmin(stepRate, reader->lengthInSamples - pts);
            myBuffer->clear();
			reader->read(myBuffer, 0, stride, pts, false, false);
			pts += stride;
            numSamples = stride;
            
			for (int c = 0; c < reader->numChannels; c++) {
				for (int i = 0; i < stride; i++) {
					if (c == 0) {
						inputToFFT[i].r = 0;
						inputToFFT[i].i = 0;
					}
                    inputToFFT[i].r += myBuffer->getSample(c, i);
				}
			}

			fft.perform(inputToFFT, outputFromFFT);

			//pass to audio nodes
			double peakFrequency = 512 * reader->sampleRate / (float) stepRate;
			for (int i = 0; i < earNodes.size(); i++) {
                //each ear node represents a base frequency
                //we want to fill it with the sum of the base frequency and all harmonics up to max frequency
                double toAdd = 0;
				double freq = earNodes[i]->frequency;
                while (freq < peakFrequency) {
                    int pos = (freq / peakFrequency) * stepRate;
                    if (pos >= 0 && pos < stepRate)
                    {
                        toAdd += sqrt(pow(outputFromFFT[pos].r, 2) + pow(outputFromFFT[pos].i, 2));
                        //DBG(String::formatted("Added value %f for base frequency %f, harmonic %f", toAdd, earNodes[i]->frequency, freq));
                    }
                    freq = freq * 2;
                }
                earNodes[i]->audioBuffer.add(toAdd);
			}
		}

		//run nodes if it's time
		if ((Time::getCurrentTime() - lastCheck).inSeconds() > tick_seconds) {
			lastCheck = Time::getCurrentTime();
			Node::runAllNodes();

			runNodeOutputs();
			if (excitementBuffer.size() > 100) {
				excitementBuffer.remove(0);
			}
		}
		wait(5);
	}
    readyToPlayAudio = false;

	delete samples;
}

void Clara::runNodeOutputs()
{
	//excitement, based on audio
	int excitementLookBack = LOOKBACK_LIMIT;
	float oldExcitementWeight = .5;
	float oldExcitementSum = 0;
	for (int i = fmax(0, excitementBuffer.size() - excitementLookBack - 1); i < excitementBuffer.size(); i++) {
		oldExcitementSum += excitementBuffer[i];
	}
	float newExcitementValue = audioSumNode->output;

	newExcitementValue = newExcitementValue * (1.0 - oldExcitementWeight) + oldExcitementWeight * oldExcitementSum / excitementLookBack;
	excitementBuffer.add(newExcitementValue);
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
