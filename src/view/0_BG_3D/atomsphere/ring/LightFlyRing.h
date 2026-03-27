#pragma once
#include "ofMain.h"
#include "HaloCreature.h"

class LightFlyRing {
public:
    void setup(float r, float topH, float botExt);
    void update(float time);
    void draw();
    
    // Interaction methods
    void addLightAt(float u, float v);
    void clearLights();
    void setInteraction(float u, float v);

    ofMesh mesh;
    ofFbo fbo;
    
    vector<shared_ptr<HaloCreature>> creatures;
    
    float radius;
    float height;
    float bottomExt;
    
    ofVec2f interactPos;
    bool isInteracting;
};
