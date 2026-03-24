#pragma once
#include "ofMain.h"

struct Avoider {
    ofVec2f pos;
    ofVec2f vel;
    ofVec2f acc;
    float maxSpeed;
    float maxForce;
    ofColor color;
    float radius;

    void applyForce(ofVec2f force) {
        acc += force;
    }

    void update() {
        vel += acc;
        vel.limit(maxSpeed);
        pos += vel;
        acc.set(0, 0);
    }
};

class AvoiderLayer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();

    vector<Avoider> avoiders;
    float simWidth, simHeight;
};