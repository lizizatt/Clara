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

	for (int i = 400; i < 20000; i *= 1.25) {
		AudioFrequencySourceNode *audNode = new AudioFrequencySourceNode((double) i);
		audioSumNode->inputNodes.add(audNode);
		earNodes.add(audNode);
		nodes.add(audNode);
	}
}

void Clara::run()
{
	DBG("Loading file");
	FileInputStream *stream = new FileInputStream(File("L:/Clara/Resources/repost.mp3"));
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

	AudioDeviceManager manager;
	manager.initialiseWithDefaultDevices(0, reader->numChannels);
	AudioIODevice *device = manager.getCurrentAudioDevice();
	device->open(0, reader->numChannels, reader->sampleRate, stepRate);

	AudioSourcePlayer player;
	player.setSource(this);

	device->start(&player);

	//run
	DBG("Running");
	for (int i = 0; i < nToRun && !threadShouldExit(); i++) {
		//catch up on audio
		long currentPtsCap = fmin((Time::getCurrentTime() - start).inSeconds() * reader->sampleRate, reader->lengthInSamples) + reader->sampleRate;
		while (pts < currentPtsCap) {
			ScopedLock lock(audioBufferSection);
			int stride = fmin(stepRate, reader->lengthInSamples - pts);
			bool res = reader->read(intSamples, reader->numChannels, pts, stride, false);
			jassert(res);
			pts += stride;
			numSamples = stride;

			for (int c = 0; c < reader->numChannels; c++) {
				for (int i = 0; i < stride; i++) {
					int sample = intSamples[c][i];
					float ceil = pow(2, reader->bitsPerSample);
					float val = (float)sample / ceil / reader->sampleRate;
					samples[c][i] = val;

					if (c == 0) {
						inputToFFT[i].r = val;
						inputToFFT[i].i = 0;
					}
				}
			}

			fft.perform(inputToFFT, outputFromFFT);

			//pass to audio nodes
			double peakFrequency = 512 * reader->sampleRate / (float) stepRate;
			for (int i = 0; i < earNodes.size(); i++) {
				double freq = earNodes[i]->frequency;
				int pos = (freq / peakFrequency) * 1024;
				if (pos >= 0 && pos < stepRate)
				{
					earNodes[i]->audioBuffer.add(sqrt(pow(outputFromFFT[i].r, 2) + pow(outputFromFFT[i].i, 2)));
				}
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
	
	//adding velocity estimate
	if (audioSumNode->previousValues.size() > 1) {
		float excitementVelocity = 0;
		float prev = audioSumNode->previousValues[audioSumNode->previousValues.size()];
		for (int i = audioSumNode->previousValues.size() - 2; i >= 0; i--) {
			excitementVelocity += prev - audioSumNode->previousValues[i];
		}
		excitementVelocity /= audioSumNode->previousValues.size();
		newExcitementValue += excitementVelocity * .25;
	}

	newExcitementValue = newExcitementValue * (1.0 - oldExcitementWeight) + oldExcitementWeight * oldExcitementSum / excitementLookBack;
	excitementBuffer.add(newExcitementValue);
}

void Clara::getNextAudioBlock(const juce::AudioSourceChannelInfo &outputBuffer)
{
	ScopedLock lock(audioBufferSection);
	for (int i = 0; i < outputBuffer.buffer->getNumChannels(); i++) {
		float* typePointer = &samples[i][outputBuffer.startSample];
		outputBuffer.buffer->copyFrom(i, outputBuffer.startSample, typePointer, outputBuffer.numSamples);
	}
}

Array<float> Clara::getExcitementBuffer()
{
	return excitementBuffer;
}