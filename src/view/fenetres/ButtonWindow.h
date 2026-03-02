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

    void setupWorms(int count);
    void toggleWorms() { bDrawWorms = !bDrawWorms; }
    bool bDrawWorms = true;

private:
    struct Button {
        ofRectangle rect;
        int id;
        float currentAlpha;
        float targetAlpha;
        ofColor color;
    };

    struct ButtonWorm {
        ofVec2f headPos;
        ofVec2f vel;
        vector<ofVec2f> segments;
        int numSegments;
        float speed;
        ofColor color;
        float angle;
    };

    vector<ButtonWorm> worms;
    vector<Button> buttons;
    ofImage bgImg;
    float winW, winH;
};
