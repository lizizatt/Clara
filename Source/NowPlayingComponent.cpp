/*
  ==============================================================================

    NowPlayingComponent.cpp
    Created: 13 Jul 2017 1:16:56pm
    Author:  Liz Izatt

  ==============================================================================
*/

#include "NowPlayingComponent.h"

NowPlayingComponent::NowPlayingComponent(Clara *clara)
: clara(clara)
{
    currentlyListeningToLabel.setColour(Label::ColourIds::textColourId, Colours::white);
    currentlyListeningToLabel.setText("Currently listening to:", dontSendNotification);
    
    currentlyListeningToValue.setColour(Label::ColourIds::textColourId, Colours::white);
    currentlyListeningToValue.setText("Nothing", dontSendNotification);
    
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    
    upNextLabel.setColour(Label::ColourIds::textColourId, Colours::white);
    upNextLabel.setText("Up next:", dontSendNotification);
    
    upNextValue.setColour(Label::ColourIds::textColourId, Colours::white);
    upNextValue.setText("Nothing", dontSendNotification);
    
    memoryListBox.setModel(this);
    memoryListBox.getHeader().addColumn("Song", 1, 150);
    memoryListBox.getHeader().addColumn("Score", 2, 70);
    memoryListBox.getHeader().addColumn("", 3, 40);
    
    addSong.setButtonText("Add Song");
    addSong.addListener(this);
    
    addAndMakeVisible(currentlyListeningToLabel);
    addAndMakeVisible(currentlyListeningToValue);
    addAndMakeVisible(upNextLabel);
    addAndMakeVisible(upNextValue);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(memoryListBox);
    addAndMakeVisible(addSong);
    
    clara->addListener(this);
}

NowPlayingComponent::~NowPlayingComponent()
{
    clara->removeListener(this);
}

void NowPlayingComponent::paint(Graphics &g)
{
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
}

void NowPlayingComponent::resized()
{
    currentlyListeningToLabel.setBounds(20, 20, getWidth() - 40, 20);
    currentlyListeningToValue.setBounds(currentlyListeningToLabel.getX() + 20, currentlyListeningToLabel.getBottom() + 5, getWidth() - 60, 20);
    stopButton.setBounds(currentlyListeningToValue.getX(), currentlyListeningToValue.getBottom() + 5, 100, 20);
    
    upNextLabel.setBounds(20, stopButton.getBottom() + 20, getWidth() - 40, 20);
    upNextValue.setBounds(upNextLabel.getX() + 20, upNextLabel.getBottom() + 5, getWidth() - 60, 20);
    
    memoryListBox.setBounds(10, upNextValue.getBottom() + 20, getWidth() - 20, 200);
    addSong.setBounds(10, memoryListBox.getBottom() + 5, getWidth() - 20, 20);
}

void NowPlayingComponent::handleMessage(const Message &m)
{
    Message *msg = const_cast<Message*>(&m);
    
    Clara::MemoryChanged* memChanged = dynamic_cast<Clara::MemoryChanged*>(msg);
    if (memChanged != nullptr) {
        memory = clara->getMemory();
        memoryListBox.updateContent();
    }
    
    Clara::PlaybackStateChanged* playbackChange = dynamic_cast<Clara::PlaybackStateChanged*>(msg);
    if (playbackChange != nullptr) {
        Clara::Song* playing = clara->getCurrentlyPlaying();
        Clara::Song* upNext = clara->getUpNext();
        
        if (playing == nullptr) {
            currentlyListeningToValue.setText("Nothing", dontSendNotification);
        }
        else {
            currentlyListeningToValue.setText(playing->songFile.getFileNameWithoutExtension(), dontSendNotification);
        }
        
        if (upNext == nullptr) {
            upNextValue.setText("Nothing", dontSendNotification);
        }
        else {
            upNextValue.setText(upNext->songFile.getFileNameWithoutExtension(), dontSendNotification);
        }
    }
}

void NowPlayingComponent::buttonClicked(Button *b)
{
    if (b == &stopButton) {
        clara->stopSong();
    }
    
    if (b == &addSong) {
        //todo add file browser
    }
    
    if (b->getName().startsWith("TB")) {
        int num = b->getName().fromFirstOccurrenceOf("TB", false, false).getIntValue();
        if (num >= 0 && num < memory.size()) {
            clara->setUpNext(memory[num]);
            clara->stopSong();
        }
    }
}

int NowPlayingComponent::getNumRows()
{
    return memory.size();
}


void NowPlayingComponent::paintRowBackground(Graphics &g, int row, int w, int h, bool selected)
{
}

void NowPlayingComponent::paintCell(Graphics &g, int row, int col, int w, int h, bool selected)
{
}

Component *NowPlayingComponent::refreshComponentForCell(int row, int col, bool selected, Component *existing)
{
    if (existing != nullptr) {
        delete existing;
    }
    if (col == 1) {
        Label *toRet = new Label("labelname", String::formatted(memory[row]->songFile.getFileNameWithoutExtension()));
        return toRet;
    }
    if (col == 2) {
        Label *toRet = new Label("labelname", memory[row]->hasMeta()? String::formatted("%.1f:%.1f:%.1f", memory[row]->getSScore(), memory[row]->getDScore(), memory[row]->getNScore()) : "N/A");
        return toRet;
    }
    if (col == 3) {
        TextButton *tb = new TextButton(String::formatted("TB%d", row));
        tb->setButtonText("Play");
        tb->addListener(this);
        return tb;
    }
    return new Label();
}

bool NowPlayingComponent::isInterestedInFileDrag(const StringArray &files)
{
    for (int i = 0; i < files.size(); i++) {
        if (File(files[i]).hasFileExtension("mp3")) {
            continue;
        }
        return false;
    }
    return true;
}

void NowPlayingComponent::filesDropped(const StringArray &files, int x, int y)
{
    for (int i = 0; i < files.size(); i++) {
        if (File(files[i]).hasFileExtension("mp3")) {
            clara->addSongToMemory(File(files[i]));
        }
    }
}
