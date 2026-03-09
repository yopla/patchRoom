#pragma once
#include "ofMain.h"

struct FireParticle {
    ofVec2f pos;
    ofVec2f vel;
    float scaleX, scaleY;
    float angle;
    float life;     // 0.0 to 1.0
    float decay;    // speed of life decrease
    int imgIndex;
    int type;       // 0: SmallFlame, 1: LargeFlame, 2: Smoke, 3: Explosion
    ofColor color;
    ofBlendMode blendMode;
};

class FireALayer {
public:
    void setup(float w, float h);
    void update(float mx, float my);
    void draw();
    
    void mousePressed(float x, float y);
    void mouseReleased(float x, float y);
    
    bool bActive = false;

private:
    void createFlameTextures();
    void spawnParticles(float x, float y);
    
    float simWidth, simHeight;
    vector<ofTexture> flameTextures;
    vector<FireParticle> particles;
    
    ofFbo fbo;
    bool isMousePressed = false;
    int pressTimer = 0;
    
    ofVec2f lastMouse;
};