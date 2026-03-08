#pragma once
#include "ofMain.h"

struct MPart {
    ofVec2f pos;
    ofVec2f vel;
};

class MongolfierLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY);
    void draw();

    float simWidth;
    float simHeight;
    
    ofVec2f center;
    vector<MPart> parts;
    
private:
    bool isWall(float x, float y);
    float getMag(float x, float y);
};