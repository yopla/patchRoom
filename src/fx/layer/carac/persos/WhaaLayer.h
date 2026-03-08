#pragma once
#include "ofMain.h"

struct WhaaBall {
    ofVec2f pos;
    ofVec2f vel;
    float radius = 25.0f;
    float mass = 1.0f;
    ofColor color;
    WhaaBall* target = nullptr; // La balle que celle-ci doit suivre
};

class WhaaLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY);
    void draw();
    
    void mousePressed(float x, float y);
    void mouseReleased(float x, float y);
    
    vector<shared_ptr<WhaaBall>> balls;
    float simWidth, simHeight;
    
    // Paramètres physiques
    float springK = 0.05f;    // Force du ressort
    float friction = 0.92f;   // Frottement de l'air
    
    // Interaction
    WhaaBall* draggedBall = nullptr;
};