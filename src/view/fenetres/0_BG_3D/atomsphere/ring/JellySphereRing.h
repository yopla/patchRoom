#pragma once
#include "ofMain.h"
#include "JellyCreature.h"

class JellySphereRing {
public:
    void setup(float r, ofVec3f centerPos);
    void update(float localX, float localY);
    void draw();
    void loadTexture(string path);

    void mousePressed(float x, float y);
    void mouseReleased(float x, float y);
    void clearJellies();

    float radius;
    ofVec3f center;

    ofMesh mesh;
    ofFbo fbo;
    ofImage baseImage;
    
    vector<shared_ptr<JellyCreature>> jellies;
    
    bool isDrawing;
    float startX, startY;
    float currentX, currentY;
    
    int resX, resY;
    
    float lastLocalX = -1;
    float lastLocalY = -1;
};