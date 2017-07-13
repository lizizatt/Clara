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
    
    FFT::Complex* outputFromFFT = clara->outputFromFFT;
    
    Array<float> frequencyCounts;
    
    //pass to audio nodes
    double peakFrequency = 512 * clara->sampleRate / clara->myBuffer->getNumSamples();
    for (int i = 0; i < frequencies.size(); i++) {
        //each ear node represents a base frequency
        //we want to fill it with the sum of the base frequency and all harmonics up to max frequency
        double toAdd = 0;
        double freq = frequencies[i];
        while (freq < peakFrequency) {
            int pos = (freq / peakFrequency) * clara->myBuffer->getNumSamples();
            if (pos >= 0 && pos < clara->myBuffer->getNumSamples())
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
    float maxScore = .01;
    for (int i = 0; i < frequencyCounts.size(); i++) {
        float interval = frequencies[(root + i) % frequencyCounts.size()] / frequencies[root];
        intervals.add(interval);
        float weight = frequencyCounts[(root + i) % frequencyCounts.size()];
        intervalPresenceWeights.add(weight);
        maxScore = maxScore < weight? weight : maxScore;
    }
    
    //normalize
    for (int i = 0; i < intervalPresenceWeights.size(); i++) {
        intervalPresenceWeights.set(i, pow(intervalPresenceWeights[i] / maxScore, 2));
    }
    
    if (tickCount % JUMP_COUNT == 0) {
        clara->postMessage(new Clara::IntervalGenerator::IntervalGeneratorOutput(intervals, intervalPresenceWeights, root));
    }
}

void Clara::LoudnessMetric::tick()
{
    float avgIntensity = 0;
    float maxIntensity = 0;
    float minIntensity = 0;
    float count = 0;
    
    for (int i = 0; i < clara->myBuffer->getNumChannels(); i++) {
        for (int j = 0; j < clara->myBuffer->getNumSamples(); j++) {
            float sample = clara->myBuffer->getSample(i, j);
            avgIntensity += fabs(sample);
            minIntensity = minIntensity > sample? sample : minIntensity;
            maxIntensity = maxIntensity < sample? sample : maxIntensity;
            count++;
        }
    }
    
    avgIntensity = avgIntensity / count;
    loudness = avgIntensity;
    
    if (tickCount % JUMP_COUNT == 0) {
        clara->postMessage(new Clara::LoudnessMetric::LoudnessMetricOutput(loudness));
    }
}

void Clara::RepetitivenessNode::tick()
{
    //take fft
    FFT::Complex* outputFromFFT = clara->outputFromFFT;
    
    if (outputFromFFT == nullptr) {
        return;
    }
    
    //downsample the FFT
    int lookbackSize = 50;
    float *downresFFT = (float*) malloc(sizeof(float) * lookbackSize);
    for (int i = 0 ; i < lookbackSize; i++) {
        downresFFT[i] = 0.0;
    }
    for (int i = 0; i < clara->myBuffer->getNumSamples(); i++) {
        int binIndex = clara->myBuffer->getNumSamples() / (float) lookbackSize;
        downresFFT[binIndex] = downresFFT[binIndex] + sqrt(pow(outputFromFFT[i].r, 2) + pow(outputFromFFT[i].i, 2));
    }
    for (int i = 0 ; i < lookbackSize; i++) {
        downresFFT[i] = downresFFT[i] / ((float) clara->myBuffer->getNumSamples() / (float) lookbackSize);
    }
    
    prevFFTs.add(downresFFT);
    if (prevFFTs.size() > 860 /*this is ~10 seconds worth of samples*/) {
        delete prevFFTs[0];
        prevFFTs.remove(0);
        
        
        //we now have an array of downsample FFTs
        //repetitiveness will be the average 1 / variance
        repetitiveness = 0;
        for (int i = 0; i < lookbackSize; i++) {
            //determine mean for this sample over time
            float mean = 0;
            for (int j = 0; j < prevFFTs.size(); j++) {
                float* curFFT = prevFFTs[j];
                mean += curFFT[i];
            }
            mean = mean / (float) prevFFTs.size();
            
            //determine variance for this sample over time
            float variance = 0;
            for (int j = 0; j < prevFFTs.size(); j++) {
                float* curFFT = prevFFTs[j];
                variance += pow(mean - curFFT[i], 2);
            }
            variance = variance / (float) prevFFTs.size();
            
            repetitiveness += variance;
        }
        repetitiveness = repetitiveness / (float) lookbackSize;
        repetitiveness = 1.0 / repetitiveness;
    }
    else {
        repetitiveness = .2;
    }
    
    if (tickCount % JUMP_COUNT == 0) {
        clara->postMessage(new Clara::RepetitivenessNode::RepetitivenessNodeOutput(repetitiveness));
    }
}

Clara::MusicHormoneNode::MusicHormoneNode(Clara *clara)
: clara(clara)
{
    for (int i = 0; i < 12; i++) {
        majorLearnedWeights.add(0);
        minorLearnedWeights.add(0);
    }

    //majr 3rd
    majorLearnedWeights.set(0, 1);
    majorLearnedWeights.set(4, 5);
    majorLearnedWeights.set(7, 1);

    //minor 3rd
    minorLearnedWeights.set(0, 1);
    minorLearnedWeights.set(3, 5);
    minorLearnedWeights.set(7, 1);
}

void Clara::MusicHormoneNode::tick()
{
    Array<float> intervals = clara->intervalGeneratorNode->intervals;
    Array<float> weights = clara->intervalGeneratorNode->intervalPresenceWeights;
    float loudness = clara->loudnessMetricNode->loudness;
    
    float minorWeight = 0;
    float majorWeight = 0;
    for (int i = 0; i < 12; i++) {
        minorWeight += minorLearnedWeights[i] * weights[i];
        majorWeight += majorLearnedWeights[i] * weights[i];
    }
    
    float dynamicLearningThreshold = .99;
    while ((double) rand() / (double) INT_MAX > dynamicLearningThreshold) {
        
        float max = 0;
        int item = 0;
        for (int i = 1; i < 12; i++) {
            if (weights[i] > max) {
                max = weights[i];
                item = i;
            }
        }
        
        if (item == 0) {
            break;
        }
        
        bool major = rand() > INT_MAX / 2;
        
        if (major) {
            majorLearnedWeights.set(item, minorLearnedWeights[item] + (double) rand() / (double) INT_MAX - .5);
            DBG(String::formatted("Major neuron %d mutated to new value %.2f", item, majorLearnedWeights[item]));
        }
        else {
            minorLearnedWeights.set(item, minorLearnedWeights[item] + (double) rand() / (double) INT_MAX - .5);
            DBG(String::formatted("Minor neuron %d mutated to new value %.2f", item, minorLearnedWeights[item]));
        }
    }
    
    const float loudnessWeight = 1.0;
    static float avgLoudness = 1;
    avgLoudness = avgLoudness * .9 + loudness * .1;
    float loudnessContribution = loudness - avgLoudness;
    loudnessContribution *= loudnessWeight;
    
    float repetitivenessWeight = .0025;
    float thresh = .15;
    float repetitiveness = clara->repetitivenessNode->repetitiveness;
    float repetitivenessContribution = 1.0 / clara->repetitivenessNode->repetitiveness * repetitivenessWeight;
    repetitivenessContribution *= repetitiveness > thresh? -1 : 1;
    
    //DBG(String::formatted("Loudness %f, repetitiveness %f", loudnessContribution, repetitivenessContribution));
    
    clara->deltaSerotonin += majorWeight;
    clara->deltaDopamine += minorWeight;
    clara->deltaNoradrenaline += loudnessContribution + repetitivenessContribution;
}

Clara::NeurotransmitterManagerNode::NeurotransmitterManagerNode(Clara *clara)
: clara(clara)
{
}

void Clara::NeurotransmitterManagerNode::tick()
{
    float deltaS = clara->deltaSerotonin;
    float deltaD = clara->deltaDopamine;
    float deltaN = clara->deltaNoradrenaline;
    
    clara->deltaSerotonin = 0;
    clara->deltaDopamine = 0;
    clara->deltaNoradrenaline = 0;
    
    float curS = clara->serotoninLevel;
    float curD = clara->dopamineLevel;
    float curN = clara->noradrenalineLevel;
    
    float centerline = .2;
    float diffS = centerline - curS;
    float diffD = centerline - curD;
    float diffN = centerline - curN;
    float diffWeightSD = .1;
    float diffWeightN = .01;
    
    static float deltaOverallWeightSD = .01;
    static float deltaOverallWeightN = .01;
    float deltaWeightS = fabs(deltaS) / fabs(prevDS) * deltaOverallWeightSD;
    float deltaWeightD = fabs(deltaD) / fabs(prevDD) * deltaOverallWeightSD;
    float deltaWeightN = fabs(deltaN) / fabs(prevDN) * deltaOverallWeightN;
    
    curS += diffWeightSD * diffS + deltaS * deltaWeightS;
    curD += diffWeightSD * diffD + deltaD * deltaWeightD;
    curN += diffWeightN * diffN + deltaN * deltaWeightN;
    
    clara->serotoninLevel = fmin(fmax(curS, 0.0), 1.0);
    clara->dopamineLevel = fmin(fmax(curD, 0.0), 1.0);
    clara->noradrenalineLevel = fmin(fmax(curN, 0.0), 1.0);
    
    if (tickCount % JUMP_COUNT == 0) {
        clara->notifyNeurotransmitters();
    }
    
    //maintain rolling average of previous delta values
    float lpfscaler = .95;
    prevDS = prevDS * lpfscaler + deltaS * (1.0 - lpfscaler);
    prevDD = prevDD * lpfscaler + deltaD * (1.0 - lpfscaler);
    prevDN = prevDN * lpfscaler + deltaN * (1.0 - lpfscaler);
    
    
    //DBG(String::formatted("S: %f, %f, %f", clara->serotoninLevel, deltaS, prevDS));
    //DBG(String::formatted("D: %f, %f, %f", clara->dopamineLevel, deltaD, prevDD));
    //DBG(String::formatted("N: %f, %f, %f", clara->noradrenalineLevel, deltaN, prevDN));
}
