#pragma once
#include "ofMain.h"
#include "ofJson.h"
#include <vector>
#include <string>

struct ViewPreset {
    std::string name;
    ofVec2f pan;
    float zoom;
    ofRectangle rect;
    ofRectangle deleteBtnRect;
    ofRectangle renameBtnRect;
};

class PlaylistViewsUI {
public:
    void setup();
    void toggle(ofVec2f currentPan, float currentZoom, float btnRightX, float btnY);
    void close();
    void draw();
    
    bool mousePressed(int x, int y, ofVec2f& pan, float& zoom);
    bool mouseMoved(int x, int y, ofVec2f& pan, float& zoom);
    bool keyPressed(int key, ofVec2f& pan, float& zoom);

    bool isVisible() const { return bIsVisible; }
    
    bool isEditing() const { return editingPresetIndex != -1; }
    
    void addPreset(ofVec2f pan, float zoom);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    bool applyPresetByIndex(int index, ofVec2f& pan, float& zoom);

private:
    void applyHoverView(ofVec2f& pan, float& zoom);
    void updateLayout();

    bool bIsVisible = false;
    std::vector<ViewPreset> presets;
    
    ofRectangle boxRect;
    ofRectangle addBtnRect;
    
    bool bIsHoveringPreset = false;
    int hoveredIndex = -1;
    int editingPresetIndex = -1;
    
    ofVec2f originalPan;
    float originalZoom;
    
    float anchorX = 0, anchorY = 0;
};