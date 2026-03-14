#pragma once
#include "ofMain.h"

class WalkingSquare {
public:
    void setup(float x, float y, float s);
    void update(float dt);
    void draw();

    ofVec2f pos;
    float size;
    float phase;
    float speed;
    ofColor color;
};

class WalkingSquareLayer {
public:
    void setup(float w, float h);
    void update(float mx, float my, float time);
    void draw();

    vector<WalkingSquare> squares;
    float simWidth;
    float simHeight;
};