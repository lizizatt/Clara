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
    static const int JUMP_COUNT;
    
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
    
    class Song
    {
    public:
        Song(File songFile);
        float getSScore();
        float getDScore();
        float getNScore();
        bool hasMeta();
    public:
        File songFile;
        File metaFile;
        float sScore = 0;
        float dScore = 0;
        float nScore = 0;
    };
    
    class PlaybackStateChanged : public Message
    {
    public:
        PlaybackStateChanged() {}
    };
    
    class MemoryChanged : public Message
    {
    public:
        MemoryChanged() {}
    };
    
    class PTSUpdateMessage : public Message
    {
    public:
        long pts;
        long maxPts;
        double sampleRate;
        PTSUpdateMessage(long pts, long maxPts, double sampleRate) : pts(pts), maxPts(maxPts), sampleRate(sampleRate) {}
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
    
    class AverageNeutransmitterValues : public Message
    {
    public:
        float nora, sero, dopa;
        AverageNeutransmitterValues(float sero, float dopa, float nora) : sero(sero), dopa(dopa), nora(nora) {}
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
        LoudnessMetric(Clara* clara) : clara(clara) {}
        void tick() override;
    public: //input
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
        IntervalGenerator(Clara* clara) : clara(clara) {}
		void tick() override;
    public: //input
        Clara* clara = nullptr;
    public: //output
        Array<float> intervals;
        Array<float> intervalPresenceWeights;
	};
    
    class RepetitivenessNode : public Node
    {
    public:
        class RepetitivenessNodeOutput : public Message
        {
        public:
            float repetitiveness;
            RepetitivenessNodeOutput(float repetitiveness) : repetitiveness(repetitiveness) {}
        };
    public:
        RepetitivenessNode(Clara *clara) : clara(clara) {}
        void tick() override;
    public:
        Clara *clara = nullptr;
    public:
        float repetitiveness = 0;
        Array<float*> prevFFTs;
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
        
        float prevDS = 0;
        float prevDD = 0;
        float prevDN = 0;
    };
    
public:
	void setUpNodes();
	void run() override;
    
    void stopSong();

	void getNextAudioBlock(const AudioSourceChannelInfo &buffer);
    void notifyNeurotransmitters();
    
    void addSongToMemory(File song, bool addToQueue = true);

    Array<Song*> getMemory();
    Song* getCurrentlyPlaying();
    Song* getUpNext();
    void setUpNext(Song* toPlay);
    void removeSong(Song* toRemove);
    
private:
    void pickNextSong();
    void playSong(Song *song);
    void loadMemory();
    File getMemoryFolder();
    void runFFT();
    
private:
    ScopedPointer<LoudnessMetric> loudnessMetricNode;
    ScopedPointer<IntervalGenerator> intervalGeneratorNode;
    ScopedPointer<RepetitivenessNode> repetitivenessNode;
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
    
    float currentSongAvgS = .5;
    float currentSongAvgD = .5;
    float currentSongAvgN = .5;
    
    //audio playback
	float **samples;
	int numSamples;
	int nChannels;
    float sampleRate;
	CriticalSection audioBufferSection;
    bool readyToPlayAudio = false;
    ScopedPointer<AudioSampleBuffer> myBuffer;
    FFT::Complex* outputFromFFT = nullptr;
    bool stopSongFlag = false;
    
    Song* currentlyPlaying = nullptr;
    Song* upNext = nullptr;
    OwnedArray<Song> memory;
};


#endif  // CLARA_H_INCLUDED
