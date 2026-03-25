#pragma once
#include "ofMain.h"

class Crayon {
public:
    void setup();
    void drawUI(float x, float y);
    void drawCursor(ofVec2f worldPos);
    bool mousePressedScreen(float x, float y, float uiX, float uiY);

    float radius = 40.0f;
    int colorType = 1; // 0: Gomme (Alpha 0), 1: Blanc, 2: Noir
    
    ofRectangle uiRect;
    ofRectangle btnMinus, btnPlus;
    ofRectangle btnWhite, btnBlack, btnEraser;
};