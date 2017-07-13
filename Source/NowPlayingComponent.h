/*
  ==============================================================================

    NowPlayingComponent.h
    Created: 13 Jul 2017 1:16:56pm
    Author:  Liz Izatt

  ==============================================================================
*/

#ifndef NOWPLAYINGCOMPONENT_H_INCLUDED
#define NOWPLAYINGCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Clara/Clara.h"

class NowPlayingComponent : public Component, public MessageListener, public ButtonListener, public TableListBoxModel, public FileDragAndDropTarget
{
public:
    NowPlayingComponent(Clara *clara);
    ~NowPlayingComponent();

    void paint(Graphics &g) override;
    void resized() override;
    
    void handleMessage(const Message &m) override;
    void buttonClicked(Button *b) override;
    
    int getNumRows() override;
    void paintRowBackground(Graphics &g, int row, int w, int h, bool selected) override;
    void paintCell(Graphics &g, int row, int col, int w, int h, bool selected) override;
    Component *refreshComponentForCell(int row, int col, bool selected, Component *existing) override;
    
    bool isInterestedInFileDrag(const StringArray &files) override;
    void filesDropped(const StringArray &files, int x, int y) override;
    
private:
    
    Label currentlyListeningToLabel;
    Label currentlyListeningToValue;
    TextButton stopButton;
    Label upNextLabel;
    Label upNextValue;
    
    Label memoryLabel;
    TableListBox memoryListBox;
    TextButton addSong;
    
    Array<Clara::Song*> memory;
    
    Clara *clara;
};


#endif  // NOWPLAYINGCOMPONENT_H_INCLUDED
