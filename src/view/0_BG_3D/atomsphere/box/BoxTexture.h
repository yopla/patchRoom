#pragma once
#include "ofMain.h"

class BoxTexture {
public:
    void setup(float w, float h, float d);
    void draw();

    ofBoxPrimitive box;
    ofTexture textureDamier;
};