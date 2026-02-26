#pragma once
#include "ofMain.h"

class PlantStem {
public:
    void setup(float x, float y, float h);
    void update(float time, float mx, float my);
    void draw();

    vector<ofVec2f> nodes;
    float height;
    float noiseOffset;
    ofColor color;
    float thickness;
    float currentPush;
};

class PlantLayer {
public:
    void setup(float w, float h);
    void update(float mx, float my);
    void draw();

    vector<PlantStem> stems;
};