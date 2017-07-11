/*
  ==============================================================================

    ClaraNodes.cpp
    Created: 28 Jun 2017 11:12:21pm
    Author:  Liz

  ==============================================================================
*/

#include "Clara.h"

Array<Clara::Node*> Clara::Node::allNodes;

Clara::Node::Node()
{
	allNodes.add(this);
}

void Clara::Node::baseTick()
{
	tickCount++;
	tick();
}

void Clara::Node::runAllNodes()
{
	for (int i = 0; i < allNodes.size(); i++) {
		allNodes[i]->baseTick();
	}
}

void Clara::IntervalGenerator::tick()
{
    const Array<float> frequencies = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87};
    
    intervals.clear();
    intervalPresenceWeights.clear();
    
    FFT::Complex* inputToFFT = (FFT::Complex*)malloc(sizeof(FFT::Complex) * buffer->getNumSamples());
    FFT::Complex* outputFromFFT = (FFT::Complex*)malloc(sizeof(FFT::Complex) * buffer->getNumSamples());
    
    FFT fft(9, false);
    
    for (int c = 0; c < buffer->getNumChannels(); c++) {
        for (int i = 0; i < buffer->getNumSamples(); i++) {
            if (c == 0) {
                inputToFFT[i].r = 0;
                inputToFFT[i].i = 0;
            }
            inputToFFT[i].r += buffer->getSample(c, i);
        }
    }
    
    fft.perform(inputToFFT, outputFromFFT);
    
    Array<float> frequencyCounts;
    
    //pass to audio nodes
    double peakFrequency = 512 * clara->sampleRate / buffer->getNumSamples();
    for (int i = 0; i < frequencies.size(); i++) {
        //each ear node represents a base frequency
        //we want to fill it with the sum of the base frequency and all harmonics up to max frequency
        double toAdd = 0;
        double freq = frequencies[i];
        while (freq < peakFrequency) {
            int pos = (freq / peakFrequency) * buffer->getNumSamples();
            if (pos >= 0 && pos < buffer->getNumSamples())
            {
                toAdd += sqrt(pow(outputFromFFT[pos].r, 2) + pow(outputFromFFT[pos].i, 2));
                //DBG(String::formatted("Added value %f for base frequency %f, harmonic %f", toAdd, earNodes[i]->frequency, freq));
            }
            freq = freq * 2;
        }
        frequencyCounts.add(toAdd);
    }
    
    //now we have an intensity for each frequency in the 12 tone octave
    //pick whichegver is highest to be the root
    int root = 0;
    for (int i = 1; i < frequencies.size(); i++) {
        root = frequencyCounts[i] > frequencyCounts[root]? i : root;
    }
    
    //now score all intervals from roots
    float maxScore = 0;
    for (int i = 0; i < frequencyCounts.size(); i++) {
        float interval = frequencies[(root + i) % frequencyCounts.size()] / frequencies[root];
        intervals.add(interval);
        float weight = frequencyCounts[(root + i) % frequencyCounts.size()];
        intervalPresenceWeights.add(weight);
        maxScore = maxScore < weight? weight : maxScore;
    }
    
    //normalize
    for (int i = 0; i < intervalPresenceWeights.size(); i++) {
        intervalPresenceWeights.set(i, 1.0 - pow(intervalPresenceWeights[i] / maxScore, 2));
    }
    
    clara->postMessage(new Clara::IntervalGenerator::IntervalGeneratorOutput(intervals, intervalPresenceWeights));

    delete inputToFFT;
    delete outputFromFFT;
}

void Clara::LoudnessMetric::tick()
{
    float avgIntensity = 0;
    float maxIntensity = 0;
    float minIntensity = 0;
    float count = 0;
    
    for (int i = 0; i < buffer->getNumChannels(); i++) {
        for (int j = 0; j < buffer->getNumSamples(); j++) {
            float sample = buffer->getSample(i, j);
            avgIntensity += sample;
            minIntensity = minIntensity > sample? sample : minIntensity;
            maxIntensity = maxIntensity < sample? sample : maxIntensity;
            count++;
        }
    }
    
    avgIntensity = avgIntensity / count;
    loudness = avgIntensity;
    clara->postMessage(new Clara::LoudnessMetric::LoudnessMetricOutput(loudness));
}
