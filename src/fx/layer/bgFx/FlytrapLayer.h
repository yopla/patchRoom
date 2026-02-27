#pragma once
#include "ofMain.h"

class Flytrap {
public:
    void setup(float x, float y);
    void update(float mx, float my);
    void draw();

    ofVec2f basePos;
    vector<ofVec2f> nodes;
    
    float stemLength;
    float segmentLength;
    float noiseOffset;
    
    // Logique de la tête
    ofVec2f headPos;
    float headAngle;
    float mouthOpenness; // 0.0 (fermé) à 1.0 (ouvert)
    bool isBiting;
    float biteTimer;
    
    ofColor color;
};

class FlytrapLayer {
public:
    void setup(float w, float h);
    void update(float mx, float my);
    void draw();

    vector<Flytrap> traps;
};