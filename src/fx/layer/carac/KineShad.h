#pragma once
#include "ofMain.h"

class KineShad {
public:
    void setup();
    void update(const ofVec2f& mouse, float time);
    void draw();

private:
    ofVec2f posA, posB, posK;
    ofVec2f mousePos;
    float time;
};