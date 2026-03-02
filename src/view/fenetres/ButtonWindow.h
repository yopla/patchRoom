#pragma once
#include "ofMain.h"

class ButtonWindow {
public:
    void setup(float w, float h);
    void update(float mx, float my);
    void draw();
    
    // Draws a copy of the window content in 3D space (e.g. for RoomPreview)
    // x,y,z: Center position in 3D
    // w,h: Dimensions in 3D
    void drawPreview(float x, float y, float z, float w, float h);

    float getWidth() { return winW; }
    float getHeight() { return winH; }

    vector<int> hoveredIndices;
    vector<ofVec3f> get3DPosForActiveButtons(float roomW, float roomSolD, float roomDepth);

private:
    struct Button {
        ofRectangle rect;
        int id;
        float currentAlpha;
        float targetAlpha;
        ofColor color;
    };

    vector<Button> buttons;
    ofImage bgImg;
    float winW, winH;
};
