#pragma once
#include "ofMain.h"

struct AutoPongBall {
    ofVec2f pos;
    ofVec2f vel;
};

struct AutoPongRange {
    float t;
    float minP;
    float maxP;
    float w;
};

struct AutoPongEvent {
    float pos;
    float w;
};

struct AutoPongRet {
    float minP;
    float maxP;
};

class AutoPongLayer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();

    float simWidth, simHeight;

private:
    vector<AutoPongBall> balls;
    float xbar, ybar, wbar, hbar, vbar;
    int frameCount = 0;

    float algo();
    AutoPongRet solvePath(float cur, float maxv, vector<AutoPongRange>& ranges);
    void appendBall();
};