#pragma once
#include "ofMain.h"
#include "FluidFloorLayer.h" // Assurez-vous que ce chemin est accessible via vos include paths

class FluidRing {
public:
    struct Level { float y; float v; float alpha; float rScale; };

    void setup(float radius, float topHeight, float bottomExtension);
    void update();
    void draw();

    void setTargetAlpha(float target);

    ofMesh mesh;
    FluidFloorLayer fluid;
    
    float radius;
    float height;
    float bottomExt;

    int resolution;
    float globalAlpha;
    float targetAlpha;
    vector<Level> baseLevels;
};
