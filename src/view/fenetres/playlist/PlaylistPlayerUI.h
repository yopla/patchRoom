#pragma once

#include "ofMain.h"
#include "ofJson.h"
#include "Scene360VideoPlayer.h"
#include "PlaylistTooltipManager.h"
#include "PlaylistNodeGraph.h"

class PlaylistPlayerUI {
public:
    void setup();
    void updateLayout(float startX, float startY);
    void update();
    void draw(Scene360VideoPlayer* player, const PlaylistNodeGraph& nodeGraph);
    
    bool mousePressed(ofVec2f worldM, Scene360VideoPlayer* player, bool* bDrawScene360VideoPtr);
    string getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);

    ofRectangle loopButtonRect;
    ofRectangle toggleButtonRect;
    ofRectangle simButtonRect;
    ofRectangle doubleSpeedBtnRect;
    ofRectangle muteBtnRect;
    ofRectangle crop106BtnRect;
    ofRectangle infinitePauseBtnRect;
    ofRectangle videoInfoBox;

    ofRectangle pauseAccordionBtn;
    bool bPauseAccordionOpen = false;
    vector<int> pauseOptions = {0, 30, 60, 150, 300, 600};
    vector<ofRectangle> pauseOptionRects;
};