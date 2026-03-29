#pragma once
#include "ofMain.h"

class RippleController {
public:
    void setup(const ofImage& img);
    void update(const ofImage& img);
    void draw(const ofImage& img, float x, float y, float w, float h);
    void createRippleAt(int localX, int localY);
    
    bool bActive = false;

private:
    void processRipples();
    void renderRipples(const ofImage& img);

    ofImage outputImage;
    vector<int> buffer1;
    vector<int> buffer2;
    int cols = 0;
    int rows = 0;
    float bufferScale = 0.5f;
    int rippleSize = 5;
};