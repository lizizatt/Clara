/*
  ==============================================================================

    Clara.h
    Created: 28 Jun 2017 6:22:44pm
    Author:  Liz

  ==============================================================================
*/

#ifndef CLARA_H_INCLUDED
#define CLARA_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Clara : public Thread, public AudioSource
{
public:
    Clara();
    ~Clara();

public:
	class Node
	{
	public:
		Node();

		virtual void nodeInitialize() {}
		virtual void nodeTeardown() {}

		virtual void tick() = 0;
		void baseTick();

	public:
		static void runAllNodes();

	public:
		static Array<Node*> allNodes;
		long tickCount = 0;
	};

	class IntOutput
	{
	public:
		virtual int getIntOutput() { return output; }
		int output = 0;
	};

	class IntSumNode : public Node, public IntOutput
	{
	public:
		void tick() override;
		Array<IntOutput*> inputNodes;
	};

	class FloatOutput
	{
	public:
		virtual float getFloatOutput() { return output; }
		float output = 0.0f;
	};

	class FloatSumNode : public Node, public FloatOutput
	{
	public:
		void tick() override;
		Array<FloatOutput*> inputNodes;
	};

	class AudioFrequencySourceNode : public Node, public FloatOutput
	{
	public:
		class Song;
	public:
		AudioFrequencySourceNode(double frequency) : frequency(frequency) {}
		void tick() override;
	public:
		double frequency;
		Array<int> audioBuffer;
	};

public:
	Array<float> getExcitementBuffer();

public:
	void setUpNodes();
	void run() override;

	void prepareToPlay(int smaples, double rate) {}
	void releaseResources() {}
	void getNextAudioBlock(const AudioSourceChannelInfo &buffer);

private:
	Array<AudioFrequencySourceNode*> earNodes;
	OwnedArray<Node> nodes;
	FloatSumNode* audioSumNode;

	float **samples;
	int numSamples;
	int nChannels;
	CriticalSection audioBufferSection;

	Array<float> excitementBuffer;
};


#endif  // CLARA_H_INCLUDED
