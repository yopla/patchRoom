#pragma once
#include "ofMain.h"
#include "CreatureSystem.h" 
#include "SauteursLayer.h"
#include "FishSchoolLayer.h"
#include "ColliderLayer.h"
#include "PoulpeLayer.h"
#include "SlimeLayer.h"
#include "WalkerLayer.h" // <--- AJOUT

#include "GearLayer.h" // <--- AJOUT
#include "LightningLayer.h" // <--- AJOUT
#include "PlantLayer.h"
#include "FlytrapLayer.h"
#include "FluidFloorLayer.h" // <--- AJOUT

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

    // --- GESTION ACTIVATION / PAUSE ---
    bool bEnabled = true;
    void setEnabled(bool enable) { bEnabled = enable; }
    
    bool bLightningHasStart = false; // Est-ce qu'on a déjà cliqué une fois ?
    ofVec2f lightningStartPos;       // Position du premier clic
    
    // Temps local pour pouvoir mettre en pause l'animation de la balle
    float localTime = 0.0f; 

    // --- INTERRUPTEURS DE LAYERS (H, J, K, L, M) ---
    bool bDrawCreatures = false; // Touche H (Jellys, Springs, Ripples)
    bool bDrawPoulpe    = false; // Touche J
    bool bDrawFish      = false; // Touche K
    bool bDrawSauteurs  = false; // Touche L
    bool bDrawSlime     = false; // Touche M
    bool bDrawWalker    = false; // <--- AJOUT Toggle (N)
    bool bDrawLightning = false; // <--- Toggle (Touche I par exemple)
    bool bDrawPlants    = false; // <--- Toggle (P)
    bool bDrawFlytraps  = false; // <--- Toggle (O)
    bool bDrawFluidFloor = false; // <--- Toggle (V)
    bool bDrawGears     = false; // <--- Toggle (E)

    // --- SYSTEMES ---
    CreatureSystem creatureSystem;
    shared_ptr<ColliderLayer> colliderLayer;
    SauteursLayer sauteursLayer;
    FishSchoolLayer fishSchoolLayer;
    PoulpeLayer poulpeLayer;
    SlimeLayer slimeLayer;
    WalkerLayer walkerLayer;    // <--- AJOUT Instance
    LightningLayer lightningLayer; // <--- Instance
    PlantLayer plantLayer;
    FlytrapLayer flytrapLayer;
    FluidFloorLayer fluidFloorLayer; // <--- AJOUT
    GearLayer gearLayer; // <--- AJOUT
    

    // FBOs de sortie
    ofFbo fboJar, fboFront, fboCour, fboBack;
    ofFbo fboSol, fboTopJar, fboTopCour;



private:
    void captureSection(ofFbo& targetFbo, float worldX, float worldTopY, ofImage& img, bool bDrawDynamics = true);
    void drawDynamicElements(); 

    ofImage imgJar, imgFront, imgCour, imgBack;
    ofImage imgSol, imgTopJar, imgTopCour;
    
    bool bShowTextures = true;
    
    // Positions et dimensions
    float srcX_Jar, srcX_Front, srcX_Cour, srcX_Back;
    const float wFront = 2400.0f;
    const float wJar   = 2624.0f;
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
