#pragma once
#include "ofMain.h"
#include "FishSchoolLayer.h" // <--- AJOUT
#include "ColliderLayer.h"   // <--- AJOUT

class Scene2DZenit : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    bool bFish = true;
    // ... (méthodes souris/clavier inchangées) ...
    void mouseScrolled(int x, int y, float scrollX, float scrollY);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void keyPressed(int key);
    // po
    bool bEnabled = false;
    void setEnabled(bool enable) { bEnabled = enable; }

    bool bPaused = false;
    void setPaused(bool paused) { bPaused = paused; }
    float localTime = 0.0f;
    void setLocalTime(float t) { localTime = t; }

    ofFbo fboJar, fboFront, fboCour, fboBack;
    ofFbo fboSol, fboTopJar, fboTopCour;

private:
    void captureView(ofFbo& fbo, ofImage& img, ofMatrix4x4 globalTransform);
    void drawDynamicElements();
    void drawLabel(string text, float x, float y);

    ofImage imgJar, imgFront, imgCour, imgBack;
    ofImage imgSol, imgTopJar, imgTopCour;
    
    bool bShowImages = true;

    // Dimensions
    const float wFront = 2400.0f, hFront = 1472.0f;
    const float wBack  = 2400.0f, hBack  = 1472.0f;
    const float wSol   = 2400.0f, hSol   = 2368.0f;
    const float wJar   = 2624.0f, hJar   = 784.0f;
    const float wCour  = 2624.0f, hCour  = 1072.0f;
    const float wTopJar = 2624.0f, hTopJar = 1600.0f;
    const float wTopCour = 2624.0f, hTopCour = 1008.0f;
    
    const float gapBack = 256.0f;

    // Navigation
    ofVec2f viewPan;
    float viewZoom;
    ofVec2f lastMouse;

    // Animation Balle
    ofVec2f ballPos;
    vector<ofVec2f> waypoints;

    // --- NOUVEAU : Layer Poissons sur le SOL ---
    FishSchoolLayer fishSol;
    shared_ptr<ColliderLayer> colliderSol;
};