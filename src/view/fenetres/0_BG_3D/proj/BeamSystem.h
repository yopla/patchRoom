#pragma once

#include "ofMain.h"
#include "RoomWalls.h"

class BeamSystem {
public:
    void setup();
    void update();
    void drawProjection(RoomWalls& walls, bool showRoof);
    void drawDebug(RoomWalls& walls);
    void updateTarget(const ofCamera& viewCam, RoomWalls& walls);
    void keyPressed(int key);

private:
    ofCamera projector;
    ofImage imgBeam;

    ofVec3f currentSmoothedPos;
    ofVec3f lastHitPoint;
    
    float currentDist = 1000.0f;
    float targetBeamWidth = 520.0f;
    float targetBeamHeight = 504.0f;
};