#pragma once
#include "ofMain.h"

struct HaloFly {
    glm::vec3 pos;
    float angle;
    float speed;
    float radius;
    float yOffset;
    float phase;
};

class HaloCreature {
public:
    HaloCreature(float x, float y);
    void update(float mx, float my);
    void draw(float scaleX = 1.0f, float scaleY = 1.0f);
    bool isInside(float mx, float my);

private:
    glm::vec2 pos;
    float haloRadius;
    vector<HaloFly> flies;
    bool isHovering;
    ofFbo fbo;
    float birthTime;
};