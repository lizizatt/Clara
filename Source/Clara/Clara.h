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
#include "JauntMessageListenerList.h"

#ifndef MAX_LOOKBACK
#define MAX_LOOKBACK 50
#endif


//==============================================================================
/*
*/


class Clara : public Thread, public MessageListenerList
{
public:
	static const double tick_seconds ;
	static const int LOOKBACK_LIMIT;

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
    
    class LoudnessMetric : public Node
    {
    public:
        class LoudnessMetricOutput : public Message {
        public:
            float loudness;
            LoudnessMetricOutput(float loudness) : loudness(loudness) {}
        };
    public:
        LoudnessMetric(Clara* clara, AudioSampleBuffer *buffer) : clara(clara), buffer(buffer) {}
        void tick() override;
    public: //input
        AudioSampleBuffer *buffer;
        Clara* clara = nullptr;
    public: //output
        float loudness = 0;
    };
    
	class IntervalGenerator : public Node
	{
    public:
        class IntervalGeneratorOutput : public Message {
        public:
            Array<float> intervals;
            Array<float> intervalPresenceWeights;
            int rootIndex;
            IntervalGeneratorOutput(Array<float> intervals, Array<float> intervalPresenceWeights, int rootIndex) : intervals(intervals), intervalPresenceWeights(intervalPresenceWeights), rootIndex(rootIndex) {}
        };
    public:
        IntervalGenerator(Clara* clara, AudioSampleBuffer *buffer) : clara(clara), buffer(buffer) {}
		void tick() override;
    public: //input
        AudioSampleBuffer *buffer;
        Clara* clara = nullptr;
    public: //output
        Array<float> intervals;
        Array<float> intervalPresenceWeights;
	};

public:
	Array<float> getExcitementBuffer();

public:
	void setUpNodes();
	void run() override;
	void runNodeOutputs();

	void getNextAudioBlock(const AudioSourceChannelInfo &buffer);

private:
    ScopedPointer<LoudnessMetric> loudnessMetricNode;
    ScopedPointer<IntervalGenerator> intervalGeneratorNode;
    
	float **samples;
	int numSamples;
	int nChannels;
    float sampleRate;
	CriticalSection audioBufferSection;
    bool readyToPlayAudio = false;
    ScopedPointer<AudioSampleBuffer> myBuffer;
    
	Array<float> excitementBuffer;
};


#endif  // CLARA_H_INCLUDED
