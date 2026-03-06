#pragma once
#include "ofMain.h"

class KrakenTentacle {
public:
    void setup(float angle, float len, int numNodes);
    void update(float time, ofVec3f headPos);
    void draw();
    
private:
    vector<ofVec3f> nodes;
    ofMesh mesh;
    float angleOffset;
    float length;
    int numNodes;
};

class KrakenSystem {
public:
    void setup();
    void start(float time);
    void update(float time);
    void draw();
    
private:
    ofSpherePrimitive head;
    vector<KrakenTentacle> tentacles;
    float startTime = 0.0f;
};