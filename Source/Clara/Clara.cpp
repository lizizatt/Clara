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
	int size = stepRate * reader->bitsPerSample;
	samples = (float**) malloc(reader->numChannels * sizeof(float*));
	for (int i = 0; i < reader->numChannels; i++) {
		samples[i] = (float*) malloc(size);
	}
	int** intSamples = (int**)malloc(reader->numChannels * sizeof(int*));
	for (int i = 0; i < reader->numChannels; i++) {
		intSamples[i] = (int*)malloc(size);
	}

	DBG(String::formatted("Alloced %d bytes", size));
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
			bool res = reader->read(intSamples, reader->numChannels, pts, stride, false);
			jassert(res);
			pts += stride;
			numSamples = stride;

			for (int c = 0; c < reader->numChannels; c++) {
				for (int i = 0; i < stride; i++) {
					int sample = intSamples[c][i];
					float ceil = pow(2, reader->bitsPerSample);
					float val = (float) sample / ceil / reader->sampleRate;
					samples[c][i] = val;

					if (c == 0) {
						inputToFFT[i].r = 0;
						inputToFFT[i].i = 0;
					}
                    inputToFFT[i].r += val;
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

	delete intSamples;
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
    
    /*
    for (int c = 0; readyToPlayAudio && c < outputBuffer.buffer->getNumChannels(); c++) {
        for (int s = outputBuffer.startSample; s < outputBuffer.startSample + outputBuffer.numSamples; s++) {
            outputBuffer.buffer->setSample(c, s, samples[c][s]);
        }
    }
    */
	for (int i = 0; readyToPlayAudio && i < outputBuffer.buffer->getNumChannels(); i++) {
        float* typePointer = &(samples[i][outputBuffer.startSample]);
		outputBuffer.buffer->copyFrom(i, outputBuffer.startSample, typePointer, outputBuffer.numSamples);
	}
}

Array<float> Clara::getExcitementBuffer()
{
	return excitementBuffer;
}
