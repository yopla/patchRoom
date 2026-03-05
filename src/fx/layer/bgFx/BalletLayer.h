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

private:
    vector<BalletParticle> particles;
    float simWidth, simHeight;
    
    // Emitter settings
    float emissionRate = 2.0f; // particles per frame (Steady(2))
    
    // Physics settings
    float gravityPower = 50.0f;
    float maxDist = 100.0f;
    float epsilon = 5.0f;
    
    ofImage particleImg;
    void createParticleTexture();
};
