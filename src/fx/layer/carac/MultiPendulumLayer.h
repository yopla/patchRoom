#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

class PendulumUnit {
public:
    void setup(float amp, float len, ofColor col);
    void update();
    void draw(float originX, float originY);
    
    float amplitude;
    float length;
    ofColor color;
    float radian;
    float omega;
    ofVec2f pos;
};

class MultiPendulumLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
private:
    float simWidth;
    float simHeight;
    vector<PendulumUnit> pendulums;
    ofVec2f origin;
};