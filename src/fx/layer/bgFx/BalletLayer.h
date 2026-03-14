#pragma once
#include "ofMain.h"

struct BalletParticle {
    ofVec2f pos;
    ofVec2f vel;
    float life;      // Seconds remaining
    float maxLife;   // Total lifetime
    float scale;
    float startScale;
    float endScale;
    ofColor color;
};

class BalletLayer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();
    
    bool bActive = false;
    vector<BalletParticle> particles;

private:
    float simWidth, simHeight;
    
    // Emitter settings
    int emissionRate = 55; // Nombre de particules générées par frame (plus rapide)
    size_t maxParticles = 250; // Limite maximale de particules en même temps
    
    // Physics settings
    float gravityPower = 550.0f;
    float maxDist = 200.0f;
    float epsilon = 45.0f;
    
    ofImage particleImg;
    void createParticleTexture();
};
