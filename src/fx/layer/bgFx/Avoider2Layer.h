#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

struct Avoider2Entity {
    ofVec2f pos;
    ofVec2f vel;
    float radius;
    ofColor color;
};

class Avoider2Layer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();
    
    void setCollider(shared_ptr<ColliderLayer> col) { collider = col; }
    void setScale(float s) { scale = s; }

    vector<Avoider2Entity> avoiders;

private:
    float simWidth, simHeight;
    shared_ptr<ColliderLayer> collider;
    float scale = 1.0f;
};