#pragma once
#include "ofMain.h"
#include <vector>
#include <deque>

class AutoSnakeBox {
public:
    void setup(float w, float h, float d);
    void reset();
    void update();
    void draw();

    ofBoxPrimitive box;
    ofFbo fbo;
    
    // Résolution de la grille (Format croix 4x3)
    int gridW = 64;
    int gridH = 48; 
    float cellW = 16.0f; // 64*16 = 1024, 48*16 = 768 (Résolution du FBO)
    
    int frameCount = 0;
    int speed = 3; // Update toutes les 3 frames (20 FPS interne)
    
    std::deque<ofVec2f> snake;
    ofVec2f point;
    bool bPaused = false;
    
    void drawToFbo();
    void spawnPoint();
    void setTargetFromRay(ofVec3f rayOrigin, ofVec3f rayDir);
    ofVec2f getNext(int x, int y, int dx, int dy);
    std::vector<ofVec2f> findPath();
    ofVec2f fallbackMove();
};