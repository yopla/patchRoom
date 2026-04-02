#pragma once
#include "ofMain.h"
#include <vector>
#include <string>
#include <algorithm>

struct SearchableButton {
    string name;
    ofRectangle* rect;
};

class PlaylistSearchBar {
public:
    void setup();
    void toggle(const vector<SearchableButton>& allButtons, ofVec2f currentPan, float currentZoom);
    void close();
    void draw();
    
    bool mousePressed(int x, int y, ofVec2f& pan, float& zoom);
    bool mouseMoved(int x, int y, ofVec2f& pan, float& zoom);
    bool keyPressed(int key, ofVec2f& pan, float& zoom);

    bool isVisible() const { return bIsVisible; }

private:
    void updateSuggestions();
    void applyHoverView(ofVec2f& pan, float& zoom);

    bool bIsVisible = false;
    string searchText = "";
    
    vector<SearchableButton> allAvailableButtons;
    vector<SearchableButton> currentSuggestions;
    
    ofRectangle searchBoxRect;
    vector<ofRectangle> suggestionRects;
    
    bool bIsHoveringSuggestion = false;
    int hoveredIndex = -1;
    
    ofVec2f originalPan;
    float originalZoom;
};