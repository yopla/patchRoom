#pragma once
#include "ofMain.h"

// On inclut les futurs fichiers (pense à les créer)
#include "RoomWalls.h"
#include "ProjectionSystem.h"
#include "AtmosphereSystem.h"
#include "PosterSystem.h"
#include "WormSystem.h"       // <--- AJOUT
#include "WingedWormSystem.h" // <--- AJOUT NOUVEAU
#include "CursorSquareSystem.h" // <--- AJOUT
#include "RippleSystem.h" // <--- AJOUT 1
#include "FluidRing.h"
#include "RoomInputHandler.h"
#include "LightFlyRing.h"
#include "UndulatingFloor.h"
#include "KrakenSystem.h" // <--- AJOUT
#include "ExternalKrakenSystem.h" // <--- AJOUT
#include "CloudRippleRing.h" // <--- AJOUT
#include "LiquidSphereRing.h" // <--- AJOUT
#include "JellySphereRing.h" // <--- AJOUT
#include "Scene360VideoPlayer.h" // <--- AJOUT

class RoomApp : public ofBaseApp {

public:

    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    bool gabAlpha = true;

    CursorSquareSystem cursorSquare; // <--- AJOUT
    PosterSystem poster;
    ofImage posterImg;
    RippleSystem ripples; 
    FluidRing fluidRing;
    LightFlyRing lightFlyRing;
    UndulatingFloor undulatingFloor;
    KrakenSystem kraken; // <--- AJOUT
    ExternalKrakenSystem externalKraken; // <--- AJOUT

    CloudRippleRing cloudRing;
    LiquidSphereRing liquidSphereRing; // <--- AJOUT
    JellySphereRing jellySphereRing; // <--- AJOUT
    Scene360VideoPlayer scene360VideoPlayer; // <--- AJOUT

    // Position X globale sur le périmètre (de 0 à TotalPerimeter)
    float posterGlobalX; 
    float posterY;       // Hauteur
    
    // Dimensions
    const float posterW = 600.0f;
    const float posterH = 450.0f;
    float posterSpeed = 15.0f;

    // Périmètre total de la pièce
    float totalPerimeter;

    
    // La fonction qui centralise le rendu pour tous les FBOs
    void drawSceneContent(bool showAtmosphere = true, bool isGlobalView = false); 
    void generateEquirectangularImage(); // <--- AJOUT

    // Oscillation de la Room (Effet Bateau)
   
    ofVec3f roomPosOffset;
    ofVec3f roomRotOffset;
    void applyRoomTransform();
    void applyInverseRoomTransform();
    
    bool bEnabled = true;
    void setEnabled(bool enable) { bEnabled = enable; }

    bool bPaused = false;
    void setPaused(bool paused) { 
        bPaused = paused; 
        if (atmosphere.bIsVideo && atmosphere.video360.isLoaded()) {
            atmosphere.video360.setPaused(paused);
        }
    }
    float localTime = 0.0f;
    void setLocalTime(float t) { localTime = t; }

    // --- Dimensions de la pièce (références pour les caméras) ---
    const float roomWidth = 2400.0f;
    const float roomDepth = 2624.0f;
    const float roomSolDepth = 2368.0f;
    const float heightFrontBack = 1472.0f;
    const float heightCour = 1072.0f;
    const float heightJar = 784.0f;
    const float heightTopCour = 1008.0f;
    const float heightTopJar = 1600.0f;
    
    // --- FBOs (Accessibles par ofApp et RoomPreview) ---
    ofFbo fboFront, fboBack, fboCour, fboJar, fboSol;
    ofFbo fboTopCour, fboTopJar;

    // --- Les 3 Nouveaux Modules ---
    RoomWalls walls;
    ProjectionSystem projection;
    AtmosphereSystem atmosphere;
    WormSystem wormSystem;
    WingedWormSystem wingedWormSystem;

    // --- Caméras et Navigation ---
    ofEasyCam camGlobal;

    // --- Flags de contrôle ---
    bool bShowRoof = true;
    float wallAlpha = 100.0f;
    bool respire = false;
    bool bDrawRipples = false; // Optionnel : pour activer/désactiver
    bool bDrawGab = true; // Remplaçant de bDrawWalls pour la texture "Gab"
    bool bDrawBeam = true; 
    bool bDrawWorms = false;
    bool bDrawWingedWorms = false; // Activé par touche 'W'
    bool bUseTexture = true;
    bool bDrawAtmosphere = true; // Pour contrôler l'atmosphère de l'ui
    bool bFluidRingEnabled = false;
    bool bLightFlyRingEnabled = true;
    bool bDrawUndulatingFloor = false;
    bool bDrawKraken = false; // <--- AJOUT
    bool bDrawExternalKraken = false; // <--- AJOUT
    bool bDrawCloudRing = false; // <--- AJOUT
    bool bDrawLiquidSphere = false; // <--- AJOUT
    bool bDrawJellySphere = false; // <--- AJOUT
    bool bDrawScene360Video = false; // <--- AJOUT
    bool bLockCameraCenter = false; // <--- AJOUT
   
    bool bOscillateRoom = false;

private:
    RoomInputHandler inputHandler;
    ofCamera camFront, camBack, camCour, camJar, camSol, camTopCour, camTopJar;
    ofVec3f rigPosition;
};