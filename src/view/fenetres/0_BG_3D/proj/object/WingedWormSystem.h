#pragma once
#include "ofMain.h"

struct WingedAgent {
    ofVec3f pos, vel, target;
    
    // Corps (segments qui suivent la tête)
    vector<ofVec3f> body;
    
    // Fil qui pendouille (Verlet physics)
    vector<ofVec3f> thread;
    vector<ofVec3f> threadOld; 
    
    float wingAngle = 0;
    float wingSpeed = 0.5;
    float speed = 4.0;
    ofColor color;
    
    // Paramètres
    int numBodySegments = 8;
    int numThreadSegments = 12;
};

class WingedWormSystem {
public:
    void setup(int count);
    void update(float time);
    void draw();
    
    vector<WingedAgent> agents;
};