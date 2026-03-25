#pragma once
#include "ofMain.h"

class PaperLight {
public:
    ofVec2f pos;
    float dx = 0;
    float dy = 0;
    ofFbo fbo; // Le FBO dédié pour isoler la 3D de la 2D

    void setup(float x, float y);
    void update(float mx, float my, bool spacePressed, ofVboMesh& sharedMesh, ofTexture& tex);
    void draw();
};

class PaperLightLayer {
public:
    void setup(float w, float h);
    void update(float mx, float my, bool spacePressed);
    void draw();
    void addLight(float x, float y);

    vector<shared_ptr<PaperLight>> lights;
    ofFbo texFbo;
    ofVboMesh sharedMesh;
};