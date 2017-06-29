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

void Clara::AudioFrequencySourceNode::tick()
{
	if (audioBuffer.size() > 0) {
		if (previousAudio.size() > 5) {
			previousAudio.remove(0);
		}
		previousAudio.add(output);
		output = audioBuffer[0];
		audioBuffer.remove(0);
	}
}

void Clara::IntSumNode::tick()
{
	previousValues.add(output);
	if (previousValues.size() > Clara::LOOKBACK_LIMIT) {
		previousValues.remove(0);
	}
	output = 0;
	for (int i = 0; i < inputNodes.size(); i++) {
		output += inputNodes[i]->getIntOutput();
	}
}

void Clara::FloatSumNode::tick()
{
	previousValues.add(output);
	if (previousValues.size() > Clara::LOOKBACK_LIMIT) {
		previousValues.remove(0);
	}
	output = 0;
	for (int i = 0; i < inputNodes.size(); i++) {
		output += inputNodes[i]->getFloatOutput();
	}
}