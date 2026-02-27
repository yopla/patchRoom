#pragma once
#include "ofMain.h"
#include "FluidFloorLayer.h" // Assurez-vous que ce chemin est accessible via vos include paths

class FluidRing {
public:
    void setup(float radius, float topHeight, float bottomExtension);
    void update();
    void draw();

    ofMesh mesh;
    FluidFloorLayer fluid;
    
    float radius;
    float height;
    float bottomExt;
};
