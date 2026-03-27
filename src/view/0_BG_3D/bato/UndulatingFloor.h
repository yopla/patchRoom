#pragma once
#include "ofMain.h"

class UndulatingFloor {
public:
    void setup(float w, float zStart, float zEnd);
    void update(float time);
    void draw();

private:
    ofMesh mesh;
    int cols = 200;
    int rows = 200;
};