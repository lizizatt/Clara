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
    
    enum Emotion {
        shame = 0,
        distress,
        terror,
        anger,
        disgust,
        surprise,
        joy,
        excitement,
        null
    };
    
public:
    Clara();
    ~Clara();

public:
    
    class HormoneUpdateMessage : public Message
    {
    public:
        Array<float> currentEmotionMap;
        HormoneUpdateMessage(Array<float> currentEmotionMap) : currentEmotionMap(currentEmotionMap) {}
    };
    
    class SerotoninUpdateMessage : public Message
    {
    public:
        float serotonin;
        SerotoninUpdateMessage(float serotonin) : serotonin(serotonin) {}
    };
    
    class DopamineUpdateMessage : public Message
    {
    public:
        float dopamine;
        DopamineUpdateMessage(float dopamine) : dopamine(dopamine) {}
    };
    
    class NoradrenalineUpdateMessage : public Message
    {
    public:
        float noradrelaine;
        NoradrenalineUpdateMessage(float noradrelaine) : noradrelaine(noradrelaine) {}
    };
    
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
    
    class MusicHormoneNode : public Node
    {
    public:
        MusicHormoneNode(Clara* clara);
        void tick() override;
    public:
        Clara *clara = nullptr;
    public:
        float happiness = 0;
        Array<float> majorLearnedWeights;
        Array<float> minorLearnedWeights;
    };
    
    class NeurotransmitterManagerNode : public Node
    {
    public:
        NeurotransmitterManagerNode(Clara* clara);
        void tick() override;
    public:
        Clara *clara = nullptr;
    };
    
public:
	void setUpNodes();
	void run() override;
	void runNodeOutputs();

	void getNextAudioBlock(const AudioSourceChannelInfo &buffer);
    
    void notifyNeurotransmitters();

private:
    ScopedPointer<LoudnessMetric> loudnessMetricNode;
    ScopedPointer<IntervalGenerator> intervalGeneratorNode;
    ScopedPointer<MusicHormoneNode> musicHormoneNode;
    ScopedPointer<NeurotransmitterManagerNode> neurotransmitterManagerNode;
    
    //hormones
    float serotoninLevel = 0.5;
    float dopamineLevel = 0.5;
    float noradrenalineLevel = 0.5;
    
    float deltaSerotonin = 0;
    float deltaDopamine = 0;
    float deltaNoradrenaline = 0;
    
    //emotions
    HashMap<Emotion, float> currentEmotionMap;
    
    //audio playback
	float **samples;
	int numSamples;
	int nChannels;
    float sampleRate;
	CriticalSection audioBufferSection;
    bool readyToPlayAudio = false;
    ScopedPointer<AudioSampleBuffer> myBuffer;
};


#endif  // CLARA_H_INCLUDED
