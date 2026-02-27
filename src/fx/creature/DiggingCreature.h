#pragma once
#include "ofMain.h"

class DiggingCreature {
public:
    struct Agent {
        float cx, cy; // Position relative to center
        float vx, vy;
        float ang;
        int cargo; // 0: exploring, 1: returning/digging
    };

    void setup(float x, float y, ofImage& img);
    void update(float mx, float my);
    void draw();
    
    void setAccelerate(bool b) { bAccelerate = b; }
    void toggle() { bEnabled = !bEnabled; }
    bool isEnabled() { return bEnabled; }

private:
    bool isWall(float wx, float wy);
    void crackMap(float wx, float wy);

    bool bEnabled = false;
    bool bAccelerate = false;

    ofVec2f pos; // World position center
    
    // Grid settings (AS3 inspired)
    int mWidth = 60;  // Grid width
    int mHeight = 60; // Grid height
    float cw = 12.0f; // Cell width
    float ch = 12.0f; // Cell height
    
    ofPixels mapPix;
    ofTexture mapTex;
    
    vector<Agent> agents;
    ofImage particleImg;
};
