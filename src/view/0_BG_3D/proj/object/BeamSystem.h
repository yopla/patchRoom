#pragma once

#include "ofMain.h"
#include "RoomWalls.h"

class BeamSystem {
public:
    void setup(string imgName = "IMG_REF/iconeRZEF.png", float w = 520.0f, float h = 504.0f);
    void update();
    void drawProjection(RoomWalls& walls, bool showRoof, float depthBias = -1.0f, float alpha = 255.0f);
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