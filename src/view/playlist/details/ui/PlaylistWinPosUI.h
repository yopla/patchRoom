#pragma once
#include "ofMain.h"
#include "ofJson.h"
#include <vector>
#include <string>

class PlaylistVisualizerApp;

struct WinPosPreset {
    std::string name;
    ofRectangle windows[7];
    ofRectangle rect;
    ofRectangle deleteBtnRect;
    ofRectangle renameBtnRect;
};

class PlaylistWinPosUI {
public:
    void setup();
    void toggle(float btnRightX, float btnY);
    void close();
    void draw();
    
    bool mousePressed(int x, int y, PlaylistVisualizerApp* app);
    bool mouseMoved(int x, int y);
    bool keyPressed(int key);

    bool isVisible() const { return bIsVisible; }
    
    bool isEditing() const { return editingPresetIndex != -1; }
    
    void addPreset(PlaylistVisualizerApp* app);
    bool applyPresetByIndex(int index, PlaylistVisualizerApp* app);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);

private:
    void updateLayout();

    bool bIsVisible = false;
    std::vector<WinPosPreset> presets;
    
    ofRectangle boxRect;
    ofRectangle addBtnRect;
    
    int hoveredIndex = -1;
    int editingPresetIndex = -1;
    float anchorX = 0, anchorY = 0;
};