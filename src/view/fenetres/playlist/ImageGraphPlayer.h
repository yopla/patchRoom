#pragma once

#include "ofMain.h"
#include "ofJson.h"
#include "AtmosphereSystem.h"

struct ImageGraphNode {
    string path;
    string name;
    ofVec2f pos;
    vector<int> edges;
};

class ImageGraphPlayer {
public:
    void setup(AtmosphereSystem* atm);
    void update();
    void draw(const ofRectangle& dropZone);
    void loadFolder(const string& folderPath, const ofRectangle& bounds);
    
    bool mousePressed(ofVec2f worldM);
    void planPath(int targetIndex);
    
    void generateGraph(const ofRectangle& bounds);
    bool lineIntersectsLine(ofVec2f A, ofVec2f B, ofVec2f C, ofVec2f D);
    bool ccw(ofVec2f A, ofVec2f B, ofVec2f C);

    AtmosphereSystem* atmosphere = nullptr;
    vector<ImageGraphNode> nodes;
    
    int currentIndex = -1;
    vector<int> plannedPath;
    
    int state = 0; // 0 = pause, 1 = fade
    int stateTimer = 0;
    int nextIndex = -1;
    
    int pauseDuration = 60;
    int fadeDuration = 30;
    bool isPlaying = true;
    
    ofRectangle playBtn, pauseBtn, fadeBtn;
    ofImage currentImg, nextImg;
    ofFbo crossfadeFbo;
    ofRectangle bounds;
    string currentFolderPath = "";
    
    bool bPauseAccordionOpen = false;
    vector<int> pauseOptions = {0, 30, 60, 120, 300, 600};
    vector<ofRectangle> pauseOptionRects;

    bool bFadeAccordionOpen = false;
    vector<int> fadeOptions = {0, 15, 30, 60, 120};
    vector<ofRectangle> fadeOptionRects;
    
    void updateAtmosphere(ofImage& img, float alpha);
    void updateAtmosphereFbo(ofFbo& fbo);
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt, const ofRectangle& bnds);
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);
};