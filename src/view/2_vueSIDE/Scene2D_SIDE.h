#pragma once
#include "ofMain.h"
#include "Scene2DLayerManager.h"
#include "Scene2DSamController.h"

class Scene2D_SIDE : public ofBaseApp {
public:
    void setup();
    void update();
    void draw(); 
    
    // Inputs standard OF
    void mouseScrolled(int x, int y, float scrollX, float scrollY);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void keyPressed(int key);
    void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);

    // --- GESTION ACTIVATION / PAUSE ---
    bool bEnabled = true;
    void setEnabled(bool enable) { bEnabled = enable; }

    bool bPaused = false;
    void setPaused(bool paused) { bPaused = paused; }
    
    void exportFullScene();
    void exportColliders();
    void exportEatMap();
    void export7Murs();
    void generateColliderFromOverlay();
    void generateColliderFromAI();
    void generateColliderFromSAM();
    void generateColliderFromDexined();
    void generateColliderFromDepthAnything();
    
    Scene2DSamController samController;

    void toggleSamControl();

    // Temps local pour pouvoir mettre en pause l'animation de la balle
    float localTime = 0.0f; 

    // --- MANAGER ---
    Scene2DLayerManager layerManager;
    
    // FBOs de sortie
    ofFbo fboJar, fboFront, fboCour, fboBack;
    ofFbo fboSol, fboTopJar, fboTopCour;

    // Pointeurs vers les FBOs de la RoomApp (Mode 1)
    ofFbo* roomFboFront = nullptr;
    ofFbo* roomFboBack = nullptr;
    ofFbo* roomFboCour = nullptr;
    ofFbo* roomFboJar = nullptr;
    ofFbo* roomFboSol = nullptr;
    ofFbo* roomFboTopCour = nullptr;
    ofFbo* roomFboTopJar = nullptr;

    ofVec3f get3DPos(float x, float y);
    
    // Public dimensions needed by other systems
    const float wFront = 2400.0f;
    const float wJar   = 2624.0f;
    
    int bgDisplayMode = 1; // 0: JPGs Opaque, 1: JPGs Transparent, 2: Room FBOs, 3: Rien
    
    ofImage overlayImg;
    int overlayMode = 0; // 0: OFF, 1: 33%, 2: 75%, 3: 100%

    string warningMessage = "";
    float warningEndTime = 0.0f;

private:
    void captureSection(ofFbo& targetFbo, float worldX, float worldTopY, ofImage& img, ofFbo* roomFbo = nullptr, bool bDrawDynamics = true);
    void drawDynamicElements(); 
    ofImage imgJar, imgFront, imgCour, imgBack;
    ofImage imgSol, imgTopJar, imgTopCour;
    
    // Positions et dimensions
    float srcX_Jar, srcX_Front, srcX_Cour, srcX_Back;
    const float hMax   = 1472.0f; 
    const float wSol = 2400.0f;
    const float hSol = 2368.0f;
    const float wTopJar = 2624.0f;
    const float hTopJar = 1600.0f;
    const float wTopCour = 2624.0f;
    const float hTopCour = 1008.0f;
    float totalSceneWidth;

    // Navigation
    float viewZoom = 1.0f;
    ofVec2f viewPan = ofVec2f(0, 0);
    ofVec2f lastMouse;
    bool isSpacePressed = false; 

    // Balle
    ofVec2f ballPos;
    vector<ofVec2f> waypoints;   
    
    ofVec2f getTransformedMouse();
};
