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
    void update(float time);
    void draw(float scaleX = 1.0f, float scaleY = 1.0f);
    bool isInside(float mx, float my);
    void setHovering(bool hovering);
    glm::vec2 pos;

private:
  
    float haloRadius;
    vector<HaloFly> flies;
    bool isHovering = false;
    ofFbo fbo;
    float birthTime = -1.0f;
    float animTime = 0.0f;
};