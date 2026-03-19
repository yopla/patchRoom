#pragma once
#include "ofMain.h"

class PlaylistVisualFrame {
public:
    PlaylistVisualFrame(ofVec2f pos);
    
    void draw(bool bIsSelected);
    bool isEdgeHit(ofVec2f pos, float tolerance = 10.0f);
    bool isResizeHit(ofVec2f pos, float tolerance = 15.0f);

    ofRectangle rect;
};