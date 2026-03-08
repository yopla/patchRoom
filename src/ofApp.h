#pragma once
#define NOMINMAX

#include "ofMain.h"
#include "RoomApp.h"
#include "CanvasManager.h"
#include "CreatureSystem.h"
#include "Scene2D_SIDE.h"
#include "Scene2DZenit.h"
#include "OscManager.h"
#include "PerceptionSystem.h"
#include "GeminiImageGenerator.h"


#define HOST "127.0.0.1"
#define SENDPORT 12346
#define PORT 12345
#define APP_FPS 75

class ViewApp; 
class RoomPreview; 
class ButtonApp;

class ofApp : public ofBaseApp {
public:
GeminiImageGenerator geminiGen;


    // --- FLAGS D'AFFICHAGE ---
    bool bDrawRoom = true;     
    bool bDrawZenit = true;    
    bool bDrawScene2D = true;  
    bool bDrawButtons = true;
    int gabMode = 0; // 0:100%, 1:75%, 2:33%, 3:10%, 4:OFF

    // --- PAUSE & TIME ---
    bool bGlobalPause = false; // Pause active par défaut
    float localTime = 0.0f;
    float oscTime = 0.0f;

    OscManager oscManager;
     shared_ptr<RoomApp> roomApp; // Pointeur vers l'app 3D

    // focus et quoi dautre ?
    shared_ptr<ofAppBaseWindow> previewWindowPtr;
    shared_ptr<ofAppBaseWindow> roomWindowPtr;    // <--- AJOUTE ÇA
    shared_ptr<ofAppBaseWindow> scene2DWindowPtr; // <--- AJOUTE ÇA
    shared_ptr<ofAppBaseWindow> zenitWindowPtr;   // <--- AJOUT POUR ZENIT
    shared_ptr<ofAppBaseWindow> buttonWindowPtr;  // <--- AJOUT POUR BUTTONS

    // AJOUT : Pointeur vers l'app Preview (tu l'as peut-être déjà ou pas, sinon ajoute-le)
    shared_ptr<RoomPreview> roomPreviewApp;
    shared_ptr<ButtonApp> buttonApp;

    
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
    PerceptionSystem perceptionSystem;
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
    

    ofVec2f getTransformedMouse();
   
};