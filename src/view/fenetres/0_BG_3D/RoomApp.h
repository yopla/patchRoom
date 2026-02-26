#pragma once
#include "ofMain.h"

// On inclut les futurs fichiers (pense à les créer)
#include "RoomWalls.h"
#include "ProjectionSystem.h"
#include "AtmosphereSystem.h"
#include "PosterSystem.h"
#include "CursorSquareSystem.h" // <--- AJOUT
#include "RippleSystem.h" // <--- AJOUT 1

class RoomApp : public ofBaseApp {

public:

    void setup();
    void update();
    void draw();
    void dragEvent(ofDragInfo dragInfo);
    bool gabAlpha = true;

    CursorSquareSystem cursorSquare; // <--- AJOUT
    PosterSystem poster;
    ofImage posterImg;
    RippleSystem ripples;

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
    void drawSceneContent(bool showAtmosphere = true); // Ajoute le paramètre ici
    void keyPressed(int key);
    
    bool bEnabled = true;
    void setEnabled(bool enable) { bEnabled = enable; }

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
    bool bShowRoof = true;
    float wallAlpha = 100.0f;


private:
    bool respire = true;
    bool bDrawRipples = false; // Optionnel : pour activer/désactiver
    // Caméras et Navigation
    ofCamera camFront, camBack, camCour, camJar, camSol, camTopCour, camTopJar;
    ofEasyCam camGlobal;
    ofVec3f rigPosition;
    
    // Flags de contrôle
    bool bDrawWalls = true;
    bool bDrawBeam = true; 
    bool bUseTexture = true;
    bool bDrawAtmosphere = true; // Pour contrôler l'atmosphère de l'ui
};