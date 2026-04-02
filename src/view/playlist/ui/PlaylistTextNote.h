#pragma once
#include "ofMain.h"

class PlaylistTextNote {
public:
    PlaylistTextNote(ofVec2f pos);
    void draw();
    void updateSize();
    
    string text;
    ofColor textColor;
    ofRectangle rect;
    bool bIsEditing;
};