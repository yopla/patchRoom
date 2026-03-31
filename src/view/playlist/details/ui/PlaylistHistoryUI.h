#pragma once
#include "ofMain.h"
#include <deque>
#include <string>

struct HistoryEvent {
    std::string description;
    ofVec2f pan;
    float zoom;
};

class PlaylistHistoryUI {
public:
    void setup();
    void toggle(ofVec2f currentPan, float currentZoom);
    void close();
    void draw();
    
    void addEvent(const std::string& desc, ofVec2f pan, float zoom);
    
    bool mousePressed(int x, int y, ofVec2f& pan, float& zoom);
    bool mouseMoved(int x, int y, ofVec2f& pan, float& zoom);
    bool mouseScrolled(int x, int y, float scrollY);
    bool keyPressed(int key, ofVec2f& pan, float& zoom);

    bool isVisible() const { return bIsVisible; }

private:
    void applyHoverView(ofVec2f& pan, float& zoom);

    bool bIsVisible = false;
    std::deque<HistoryEvent> events;
    int maxEvents = 50;
    
    ofRectangle boxRect;
    float scrollY = 0;
    float maxScroll = 0;
    
    bool bIsHoveringEvent = false;
    int hoveredIndex = -1;
    
    ofVec2f originalPan;
    float originalZoom;
    
    float itemHeight = 25.0f;
};