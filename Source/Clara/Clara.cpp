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
const int Clara::LOOKBACK_LIMIT = 50;
const int Clara::JUMP_COUNT = 10;

//==============================================================================

Clara::Song::Song(File songFile)
: songFile(songFile)
{
    metaFile = songFile.getSiblingFile(songFile.getFileNameWithoutExtension() + ".txt");
    
    if (hasMeta()) {
        StringArray contents;
        metaFile.readLines(contents);
        for (int i = 0; i < contents.size(); i++) {
            StringArray split = StringArray::fromTokens(contents[i], ":", "\"");
            if (split.size() >= 2) {
                if (split[0] == "S") {
                    sScore = split[1].getFloatValue();
                }
                if (split[0] == "D") {
                    dScore = split[1].getFloatValue();
                }
                if (split[0] == "N") {
                    nScore = split[1].getFloatValue();
                }
            }
        }
    }
}

float Clara::Song::getDScore()
{
    return dScore;
}

float Clara::Song::getNScore()
{
    return nScore;
}

float Clara::Song::getSScore()
{
    return sScore;
}

bool Clara::Song::hasMeta()
{
    return metaFile.existsAsFile();
}


Clara::Clara()
	: Thread("Clara")
{
}

Clara::~Clara()
{
}

void Clara::setUpNodes()
{
    //these will run in order of initialization
    intervalGeneratorNode = new IntervalGenerator(this);
    loudnessMetricNode = new LoudnessMetric(this);
    repetitivenessNode = new RepetitivenessNode(this);
    musicHormoneNode = new MusicHormoneNode(this);
    neurotransmitterManagerNode = new NeurotransmitterManagerNode(this);
}

void Clara::run()
{
    //setup
    setUpNodes();
    loadMemory();
    
    for (;!threadShouldExit();) {
        if (upNext != nullptr) {
            Song *toPlay = upNext;
            upNext = nullptr;
            playSong(toPlay);
        }
        else {
            pickNextSong();
        }
        wait(10);
    }
}

void Clara::setUpNext(Clara::Song *toPlay)
{
    upNext = toPlay;
}

void Clara::stopSong()
{
    stopSongFlag = true;
}

void Clara::pickNextSong()
{
    Array<Song*> options;
    for (int i = 0; i < memory.size(); i++) {
        if (!memory[i]->hasMeta()) {
            upNext = memory[i];
            return;
        }
        options.add(memory[i]);
    }
    if (options.size() > 0) {
        upNext = options[rand() % options.size()];
    }
}

Array<Clara::Song*> Clara::getMemory()
{
    Array<Song*> toRet;
    for (int i = 0; i < memory.size(); i++) {
        toRet.add(memory[i]);
    }
    return toRet;
}

Clara::Song* Clara::getUpNext()
{
    return upNext;
}

File Clara::getMemoryFolder()
{
    File toRet = File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory).getChildFile("Clara").getChildFile("Memory");
    if (!toRet.exists()) {
        toRet.createDirectory();
    }
    return toRet;
}

void Clara::loadMemory()
{
    Array<File> children;
    getMemoryFolder().findChildFiles(children, File::findFiles, false);
    
    for (int i = 0; i < children.size(); i++) {
        if (children[i].hasFileExtension("mp3")) {
            addSongToMemory(children[i], false);
        }
    }
    postMessage(new MemoryChanged());
}

void Clara::addSongToMemory(File song, bool playNow)
{
    if (song.existsAsFile()) {
        if (song.getParentDirectory().getFileName() == "Memory"
            && song.getParentDirectory().getParentDirectory().getFileName() == "Clara") {
            //loading from existing clara memory on startup
            memory.add(new Song(song));
        }
        else {
            File newSongLoc = getMemoryFolder().getChildFile(song.getFileName());
            song.copyFileTo(newSongLoc);
            memory.add(new Song(newSongLoc));
        }
        postMessage(new MemoryChanged());
    }
}

Clara::Song* Clara::getCurrentlyPlaying()
{
    return currentlyPlaying;
}

void Clara::playSong(Song *toPlay)
{
    currentlyPlaying = toPlay;
    FileInputStream *stream = new FileInputStream(toPlay->songFile);
    
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(stream);
    
    
    nChannels = reader->numChannels;
    int stepRate = 512;
    long pts = 0;
    sampleRate = reader->sampleRate;
    
    myBuffer = new AudioSampleBuffer(nChannels, stepRate);
    myBuffer->clear();
    
    Time lastUpdate = Time::getCurrentTime();
    
    postMessage(new PlaybackStateChanged());
    //run
    DBG("Running");
    for (int i = 0; !stopSongFlag && !threadShouldExit() && pts < reader->lengthInSamples; i++) {
        //grab audio and run nodes to process it
        
        if (true) {
            ScopedLock lock(audioBufferSection);
            readyToPlayAudio = true;
            
            long stride = fmin(stepRate, reader->lengthInSamples - pts);
            myBuffer->clear();
            reader->read(myBuffer, 0, stride, pts, false, false);
            pts += stride;
            numSamples = stride;
        }
        
        runFFT();
        
        Node::runAllNodes();
        
        currentSongAvgS = currentSongAvgS * .99 + serotoninLevel * .01;
        currentSongAvgD = currentSongAvgD * .99 + dopamineLevel * .01;
        currentSongAvgN = currentSongAvgN * .99 + noradrenalineLevel * .01;
        
        if (intervalGeneratorNode->tickCount % JUMP_COUNT == 0) {
            postMessage(new PTSUpdateMessage(pts, reader->lengthInSamples, reader->sampleRate));
            postMessage(new AverageNeutransmitterValues(currentSongAvgS, currentSongAvgD, currentSongAvgN));
        }
        
        
        wait(10);
    }
    
    stopSongFlag = false;
    DBG(String::formatted("Finished song with S %f, D %f, N %f", currentSongAvgS, currentSongAvgD, currentSongAvgN));
    readyToPlayAudio = false;
    
    File metaFile = toPlay->metaFile;
    if (!metaFile.existsAsFile()) {
        metaFile.create();
    }
    StringArray toWrite;
    toWrite.add(String::formatted("S:%f", currentSongAvgS));
    toWrite.add(String::formatted("D:%f", currentSongAvgD));
    toWrite.add(String::formatted("N:%f", currentSongAvgN));
    metaFile.replaceWithText(toWrite.joinIntoString("\n"));
    currentlyPlaying->sScore = currentSongAvgS;
    currentlyPlaying->dScore = currentSongAvgD;
    currentlyPlaying->nScore = currentSongAvgN;
    
    currentlyPlaying = nullptr;
    
    if (outputFromFFT != nullptr) {
        delete outputFromFFT;
        outputFromFFT = nullptr;
    }
    
    postMessage(new MemoryChanged());
    postMessage(new PlaybackStateChanged());
}

void Clara::runFFT()
{
    FFT::Complex* inputToFFT = (FFT::Complex*)malloc(sizeof(FFT::Complex) * myBuffer->getNumSamples());
    if (outputFromFFT != nullptr) {
        delete outputFromFFT;
    }
    outputFromFFT = (FFT::Complex*)malloc(sizeof(FFT::Complex) * myBuffer->getNumSamples());
    
    FFT fft(9, false);
    
    for (int c = 0; c < myBuffer->getNumChannels(); c++) {
        for (int i = 0; i < myBuffer->getNumSamples(); i++) {
            if (c == 0) {
                inputToFFT[i].r = 0;
                inputToFFT[i].i = 0;
            }
            inputToFFT[i].r += myBuffer->getSample(c, i);
        }
    }
    
    fft.perform(inputToFFT, outputFromFFT);
    
    delete inputToFFT;
}

void Clara::getNextAudioBlock(const juce::AudioSourceChannelInfo &outputBuffer)
{
	ScopedLock lock(audioBufferSection);
    
    for (int c = 0; readyToPlayAudio && c < outputBuffer.buffer->getNumChannels(); c++) {
        outputBuffer.buffer->copyFrom(c, outputBuffer.startSample, *myBuffer, c, outputBuffer.startSample, outputBuffer.numSamples);
    }
}

void Clara::notifyNeurotransmitters()
{
    serotoninLevel = fmax(fmin(serotoninLevel, 1.0), 0.0);
    dopamineLevel = fmax(fmin(dopamineLevel, 1.0), 0.0);
    noradrenalineLevel = fmax(fmin(noradrenalineLevel, 1.0), 0.0);
    
    postMessage(new Clara::SerotoninUpdateMessage(serotoninLevel));
    postMessage(new Clara::DopamineUpdateMessage(dopamineLevel));
    postMessage(new Clara::NoradrenalineUpdateMessage(noradrenalineLevel));
}
