#pragma once
#define NOMINMAX

#include "ofMain.h"
#include "RoomApp.h"
#include "CanvasManager.h"
#include "CreatureSystem.h"
#include "Scene2D_SIDE.h"
#include "Scene2DZenit.h"
#include "OscManager.h"

// send host (aka ip address)
#define HOST "127.0.0.1"

/// send port
#define SENDPORT 12346

#define PORT 12345

// Forward declaration pour éviter la boucle d'include
class ViewApp; 
class RoomPreview;  // <--- AJOUTE CETTE LIGNE (Forward Declaration)

class ofApp : public ofBaseApp {
public:
    // --- FLAGS D'AFFICHAGE ---
    bool bDrawRoom = true;     
    bool bDrawZenit = true;    
    bool bDrawScene2D = true;  

    OscManager oscManager;
     shared_ptr<RoomApp> roomApp; // Pointeur vers l'app 3D
		bool lastHoverState = false; // Pour détecter le changement d'état

    // focus et quoi dautre ?
    shared_ptr<ofAppBaseWindow> previewWindowPtr;
    shared_ptr<ofAppBaseWindow> roomWindowPtr;    // <--- AJOUTE ÇA
    shared_ptr<ofAppBaseWindow> scene2DWindowPtr; // <--- AJOUTE ÇA

    // AJOUT : Pointeur vers l'app Preview (tu l'as peut-être déjà ou pas, sinon ajoute-le)
    shared_ptr<RoomPreview> roomPreviewApp;

    
    void setup();
    void update();
    void draw();
void dragEvent(ofDragInfo dragInfo);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);

    // --- Modules ---
    CanvasManager canvasManager;
    CreatureSystem creatureSystem;
    shared_ptr<Scene2D_SIDE> scene2D; 
    shared_ptr<Scene2DZenit> sceneZenit; 
    
    // --- NOUVEAU : Liste des ViewApps ---
    vector<shared_ptr<ViewApp>> viewApps;

    // Méthode pour enregistrer les vues depuis le main.cpp
    void registerViewApp(shared_ptr<ViewApp> vApp);

    // --- Accès direct pour ViewApp et RoomPreview ---
    ofFbo& canvas = canvasManager.canvas; 

    // --- Navigation Master ---
    ofVec2f masterPan;      
    float masterZoom;       
    ofVec2f lastMousePos;   
    bool isSpacePressed = false; 
    bool bShowFullGab = true;

    ofVec2f getTransformedMouse();
   
};