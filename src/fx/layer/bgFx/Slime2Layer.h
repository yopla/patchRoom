#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

struct Slime2Particle {
    ofVec2f pos;
    ofVec2f vel;
    ofColor color;
    float radius;
    float life;
    bool isStuck;
};

class Slime2Layer {
public:
    void setup(float w, float h, float s, shared_ptr<ColliderLayer> col);
    void update(float time);
    void draw();
    
    void explode(float x, float y);

private:
    vector<Slime2Particle> particles;
    shared_ptr<ColliderLayer> collider;
    float simWidth;
    float simHeight;
    float scale = 1.0f;
};