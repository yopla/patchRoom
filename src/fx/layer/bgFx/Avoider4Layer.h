#pragma once
#include "ofMain.h"

struct Av4Bullet {
    ofVec2f pos;
    ofVec2f vel;
    float radius;
};

struct Av4Player {
    ofVec2f pos;
    float radius;
};

class Avoider4Layer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();

    float simWidth, simHeight;
private:
    vector<Av4Bullet> bullets;
    Av4Player player;
    int frameCount = 0;

    float calcSurvivalTime(float px, float py, float vx, float vy, int delay, int depth);
    int getBestMove();
    
    float getMint(float rx, float ry, float rvx, float rvy);
    float getMind2(float rx, float ry, float rvx, float rvy, float t);
};