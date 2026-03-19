#pragma once

#include "ofMain.h"
#include <vector>
#include <string>

class RoomApp;

class PlaylistTextureUI {
public:
    void update();
    void draw();
    
    void loadFolder(const string& path, RoomApp* roomApp);
    bool handleFolderDrop(const string& path, ofVec2f dropPos, RoomApp* roomApp);
    bool mousePressed(ofVec2f worldM, RoomApp* roomApp);
    bool mouseScrolled(ofVec2f worldM, float scrollY);
    bool keyPressed(int key, RoomApp* roomApp);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);

    ofRectangle textureDropZone;
    ofRectangle textureToggleBtn;
    
    string currentFolderPath = "";
    vector<string> textureFiles;
    int currentTextureIndex = 0;
    bool bTextureControlOn = false;
    int textureScrollOffset = 0;
    
    float lastClickTime = 0;
    int lastClickedIndex = -1;
};