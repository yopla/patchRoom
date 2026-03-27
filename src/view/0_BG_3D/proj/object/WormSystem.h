#pragma once

#include "ofMain.h"
#include "RoomWalls.h"

// Agent representing a single worm
struct WormAgent {
    ofVec3f headPos;
    ofVec3f targetPos;
    
    vector<ofVec3f> segments;
    int numSegments;
    float speed;
    float segmentSpacing;
    ofColor color;

    // Simple state for respawning
    bool isNew = true;
};

class WormSystem {
public:
    void setup(int count);
    void update(RoomWalls& walls);
    void draw(RoomWalls& walls);

private:
    void spawnAgent(WormAgent& agent, RoomWalls& walls);

    vector<WormAgent> agents;
};