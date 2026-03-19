#pragma once

#include "ofMain.h"
#include "ofJson.h"
#include "PlaylistTooltipManager.h"
#include <string>
#include <vector>

class ofApp;

class PlaylistWindowControlsUI {
public:
    void setup();
    void draw(ofApp* mainAppPtr);
    bool mousePressed(ofVec2f worldM, ofApp* mainAppPtr);
    string getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);

    ofRectangle viewBtns[4];
    bool viewHidden[4] = {false, false, false, false};
    ofRectangle moveV3Btn;
    
    ofRectangle wxcvbBtns[6];
    string wxcvbNames[6] = {"MAIN", "W:ROOM", "X:ZENI", "C:SCEN", "V:PREV", "B:BTNS"};
    int wxcvbKeys[6] = {0, 'w', 'x', 'c', 'v', 'b'}; 
    
    ofRectangle focusBtns[6];
    string focusNames[6] = {"->MAIN", "->W", "->X", "->C", "->V", "->B"};
    
    ofRectangle gabBtns[4];
};