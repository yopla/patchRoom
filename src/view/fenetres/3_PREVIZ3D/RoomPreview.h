#pragma once
#include "ofMain.h"

// Forward declaration pour dire que ofApp existe
class ofApp; 
class Scene2D_SIDE;

#include "RoomWalls.h"
#include "CursorSquareSystem.h"
#include "InteractionVisualizer.h"

class RoomPreview : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
        void drawRoomGeometry();
        void keyPressed(int key); // <--- AJOUTER CETTE LIGNE
        void mousePressed(int x, int y, int button);
        bool bPaused = false;
        void setPaused(bool paused) { bPaused = paused; }
        bool bDrawInteraction = true;
        bool bShowCursor = true;
        
        // Lien vers l'app principale pour lire le Canvas
        shared_ptr<ofApp> mainApp;
        shared_ptr<Scene2D_SIDE> sceneSide;
        
        RoomWalls walls;
        CursorSquareSystem cursorSquare;
        InteractionVisualizer interactionVisualizer;

        // --- Dimensions de la pièce (Identiques à RoomApp) ---
        const float roomWidth = 2400.0f;
        const float roomDepth = 2624.0f;
        const float heightFrontBack = 1472.0f;
        const float heightCour = 1072.0f; 
        const float heightJar = 784.0f;   
        const float heightTopCour = 1008.0f;
        const float heightTopJar = 1600.0f;
        const float roomSolDepth = 2368.0f; // <--- AJOUTER CETTE LIGNE

        // --- Coordonnées de CROP sur le Canvas géant ---
        // (Doivent correspondre à celles de ofApp::drawScene)
        float cropX_Front, cropY_Front;
        float cropX_Sol,   cropY_Sol;
        float cropX_Back,  cropY_Back;
        float cropX_Jar,   cropY_Jar;
        float cropX_Cour,  cropY_Cour;
        float cropX_TopJar, cropY_TopJar;
        float cropX_TopCour, cropY_TopCour;
        
        // --- Textures dynamiques (FBOs locaux) ---
        // On utilise des FBOs ici pour stocker les crops
        ofFbo texFront, texBack, texCour, texJar, texSol, texTopCour, texTopJar;
        
    private:
        float peakX, peakY;
        ofEasyCam camGlobal;
        ofVec3f rigPosition;
        
        // Géométrie
        vector<ofVec3f> pFront, pBack, pCour, pJar;
        ofVec3f pTopCourTL, pTopCourBL, pTopCourBR;
        ofVec3f pTopJarTL, pTopJarBL, pTopJarBR;
        ofVec3f pSolTL, pSolBL, pSolBR;

        ofMesh meshTopCour, meshTopJar, meshSol;
        //bool bShowRoof = true;
};