#pragma once
#include "ofMain.h"

class ExternalTentacle {
public:
    void setup(int index, float angle, float len, int numNodes);
    void update(float time, ofVec3f headPos, float growth);
    void draw();
    
private:
    vector<ofVec3f> nodes;
    ofMesh mesh;
    float angleOffset;
    float length;
    int numNodes;
    ofVec3f targetPoint;
};

class ExternalKrakenSystem {
public:
    void setup();
    void start(float time);
    void update(float time);
    void draw();
    
private:
    ofSpherePrimitive head;
    vector<ExternalTentacle> tentacles;
    ofVec3f headStartPos, headTargetPos;
    float startTime = 0.0f;
};