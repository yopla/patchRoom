#pragma once
#include "ofMain.h"

struct ColorCop {
    ofVec2f pos;
    ofVec2f targetPoint; // Point à détruire
    ofVec2f firingPos;   // Position de tir
    float rotation;
    int state;           // 0: Idle, 1: Moving, 2: Shooting
    float stateTimer;
    float scale;
    bool hasTarget;
};

class ColorCopRing {
public:
    void setup(float r, ofVec3f centerPos, int numCops = 30);
    void update(float dt);
    void draw();
    
    void loadTexture(string path);
    void resetTexture(); // Remet la texture au complet
    void mousePressed(float u, float v, float squareSize3D);

    float radius;
    ofVec3f center;

    ofMesh mesh;
    ofFbo fboTexture; // Texture de base avec l'alpha perforé
    ofFbo fboDisplay; // Rendu final (Texture + Lasers + Cops)
    ofImage baseImage;

    int resX, resY;

    vector<ColorCop> cops;
    vector<ofVec2f> pointsToDestroy;
};