#pragma once
#include "ofMain.h"
#include "FluidFloorLayer.h"

class LiquidSphereRing {
public:
    void setup(float r, ofVec3f centerPos);
    void draw();
    void loadTexture(string path);

    float radius;
    ofVec3f center;

    ofMesh mesh;
    FluidFloorLayer fluid;
    
    int resX;
    int resY;
};