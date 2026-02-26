#include "ofMain.h"
#include "ofApp.h"
#include "ViewApp.h"
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "RoomPreview.h" // Si vous avez gardé la preview précédente
#include "Scene2DZenit.h" // <--- AJOUT

// Classe dérivée pour gérer l'enregistrement de la Vue 3
class RecordingViewApp : public ViewApp {
public:
    bool bRecording = false;
    string folderName;
    int frameCount = 0;

    void keyPressed(int key) override {
        ViewApp::keyPressed(key);
        if(key == OF_KEY_RETURN){
            bRecording = !bRecording;
            if(bRecording) {
                folderName = "export/" + ofGetTimestampString();
                ofDirectory dir(folderName);
                dir.create(true);
                frameCount = ofGetFrameNum();
                ofLogNotice() << "Start Recording View 3 to " << folderName;
            } else {
                ofLogNotice() << "Stop Recording View 3";
            }
        }
    }

    void draw() override {
        ViewApp::draw();
        if(bRecording){
            ofSaveScreen(folderName + "/frame_" + ofToString(frameCount, 5, '0') + ".jpg");
            frameCount++;
        }
    }
};

// Classe dérivée pour gérer l'enregistrement de la RoomPreview (Vue ESC)
class RecordingRoomPreview : public RoomPreview {
public:
    bool bRecording = false;
    string folderName;
    int frameCount = 0;

    void keyPressed(int key) override {
        RoomPreview::keyPressed(key);
        if(key == OF_KEY_ESC){
            bRecording = !bRecording;
            if(bRecording) {
                folderName = "export/preview_" + ofGetTimestampString();
                ofDirectory dir(folderName);
                dir.create(true);
                frameCount = ofGetFrameNum();
                ofLogNotice() << "Start Recording RoomPreview to " << folderName;
            } else {
                ofLogNotice() << "Stop Recording RoomPreview";
            }
        }
    }

    void draw() override {
        RoomPreview::draw();
        if(bRecording){
            ofSaveScreen(folderName + "/frame_" + ofToString(frameCount, 5, '0') + ".jpg");
            frameCount++;
        }
    }
};

int main( ){
    // Initialisation de la graine aléatoire pour garantir le déterminisme de toute l'application
    

    // ------------------------------------------------
    // 1. CONFIGURATION DES FENETRES
    // ------------------------------------------------
    ofGLFWWindowSettings settings;
    //settings.setGLVersion(3, 2);
    ofSetEscapeQuitsApp(false);
    // Fenêtre Maître (Canvas)
    settings.setSize(800, 600); 
    settings.resizable = true;
    settings.setPosition(ofVec2f(50, 150));
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    // --- TOGGLES VUES ---
    bool bEnableView1 = false;
    bool bEnableView2 = false;
    bool bEnableView3 = true;
    bool bEnableView4 = false;

    shared_ptr<ofAppBaseWindow> viewWindow1;
    shared_ptr<ofAppBaseWindow> viewWindow2;
    shared_ptr<ofAppBaseWindow> viewWindow3;
    shared_ptr<ofAppBaseWindow> viewWindow4;

    // Fenêtre Vue 1
    bool deco = false;
    settings.decorated = deco;
	settings.windowMode = OF_WINDOW;
    settings.shareContextWith = mainWindow;
    
    if(bEnableView1) {
        settings.setSize(4096, 2160); 
        settings.resizable = false;
        settings.setPosition(ofVec2f(900, 150));
        viewWindow1 = ofCreateWindow(settings);
    }

    // Fenêtre Vue 2
    if(bEnableView2) {
        settings.setSize(4096, 2160); 
        settings.resizable = deco;
        settings.setPosition(ofVec2f(900, 150));
        viewWindow2 = ofCreateWindow(settings);
    }

    // Fenêtre Vue 3 ---
    if(bEnableView3) {
        settings.setSize(5024, 5312); // Full Gabarit
        settings.resizable = false;
        settings.decorated = true;
        settings.setPosition(ofVec2f(900, 150));
        viewWindow3 = ofCreateWindow(settings);
    }

    //  Fenêtre Vue 4 ---
    if(bEnableView4) {
        settings.setSize(4096, 2160); 
        settings.resizable = deco;
        settings.setPosition(ofVec2f(900, 150));
        viewWindow4 = ofCreateWindow(settings);
    }

    // Fenêtre Room 3D
    settings.setSize(800, 600); 
    settings.decorated = true;
    settings.setPosition(ofVec2f(50, 700));
    settings.resizable = true;
    settings.shareContextWith = mainWindow; 
    shared_ptr<ofAppBaseWindow> roomWindow = ofCreateWindow(settings);

    // Fenêtre Room Preview3D (Recomposition)
    settings.setSize(800, 600); 
    settings.setPosition(ofVec2f(700, 700)); 
    settings.resizable = true;
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> previewWindow = ofCreateWindow(settings);

    // Fenêtre 2D_SIDE
    settings.setSize(750, 400);
    settings.setPosition(ofVec2f(700, 300));
    settings.resizable = true;
    settings.title = "Scene2D";
    shared_ptr<ofAppBaseWindow> scene2DWindow = ofCreateWindow(settings);


    // Fenêtre 2D_GROUND
    settings.setSize(750, 750);
    settings.setPosition(ofVec2f(1000, 50));
    settings.resizable = true;
    settings.title = "Scene2D Zenit";
    shared_ptr<ofAppBaseWindow> zenitWindow = ofCreateWindow(settings);



    // ------------------------------------------------
    // 2. CREATION DES APPS
    // ------------------------------------------------ 
    shared_ptr<ofApp> mainApp(new ofApp);
    
    shared_ptr<ViewApp> viewApp1;
    if(bEnableView1) viewApp1 = make_shared<ViewApp>();
    shared_ptr<ViewApp> viewApp2;
    if(bEnableView2) viewApp2 = make_shared<ViewApp>();
    shared_ptr<ViewApp> viewApp3;
    if(bEnableView3) viewApp3 = make_shared<RecordingViewApp>();
    shared_ptr<ViewApp> viewApp4;
    if(bEnableView4) viewApp4 = make_shared<ViewApp>();

    shared_ptr<RoomApp> roomApp(new RoomApp);
    shared_ptr<RoomPreview> roomPreview(new RecordingRoomPreview);

    shared_ptr<Scene2D_SIDE> scene2DApp(new Scene2D_SIDE);
    shared_ptr<Scene2DZenit> zenitApp(new Scene2DZenit);

    // ------------------------------------------------
    // 3. CONNEXIONS
    // ------------------------------------------------

    // le timer de setWindowMovement est dans ViewApp.update()
    if(bEnableView1) {
        viewApp1->setupView(mainApp);
        viewApp1->setWindowMovement(viewWindow1, 1300, 100, 4096, 2160); 
        mainApp->registerViewApp(viewApp1);
    }
    if(bEnableView2) {
        viewApp2->setupView(mainApp);
        viewApp2->setWindowMovement(viewWindow2, 1300, 100, 4096, 2160); 
        mainApp->registerViewApp(viewApp2);
    }
    if(bEnableView3) {
        viewApp3->setupView(mainApp);
        viewApp3->setWindowMovement(viewWindow3, 1300, 100, 5024, 5312); 
        mainApp->registerViewApp(viewApp3);
    }
    if(bEnableView4) {
        viewApp4->setupView(mainApp);
        viewApp4->setWindowMovement(viewWindow4, 1300, 100, 4096, 2160); 
        mainApp->registerViewApp(viewApp4);
    }


    mainApp->roomApp = roomApp;
    mainApp->sceneZenit = zenitApp; // On connecte Zenit au MainApp
    mainApp->scene2D = scene2DApp; // Passer la référence à ofApp
    roomPreview->mainApp = mainApp;       // (Déjà existant)
    
    mainApp->roomPreviewApp = roomPreview; // On donne l'app à ofApp
    mainApp->previewWindowPtr = previewWindow; // On donne la fenêtre à ofApp
// --- AJOUTE CES LIGNES POUR LE FOCUS ---
    mainApp->roomWindowPtr = roomWindow;
    mainApp->scene2DWindowPtr = scene2DWindow;
    mainApp->previewWindowPtr = previewWindow; // Déjà présent

    // ------------------------------------------------
    // 4. CONFIGURATION DES CROPS (LAYERS)
    // ------------------------------------------------

    if(bEnableView1) {
        viewApp1->layerManager.addLayer("Layer A", 0, 0, 2400, 1472, 10, 10);
    }
    if(bEnableView2) {
        viewApp2->layerManager.addLayer("Vue Unique", 0, 0, 5024, 5312, 0, 0);
    }
    if(bEnableView3) {
        viewApp3->layerManager.addLayer("Full Canvas", 0, 0, 5024, 5312, 0, 0); 
    }
    if(bEnableView4) {
        viewApp4->layerManager.addLayer("Vue Globale", 0, 0, 5024, 5312, 300, 200, true);
        viewApp4->layerManager.layers[0].scale = 0.1;
    }
    
    
    // ------------------------------------------------
    // 5. LANCEMENT
    // ------------------------------------------------
    if(bEnableView1) ofRunApp(viewWindow1, viewApp1);
    if(bEnableView2) ofRunApp(viewWindow2, viewApp2);
    if(bEnableView3) ofRunApp(viewWindow3, viewApp3);
    if(bEnableView4) ofRunApp(viewWindow4, viewApp4);
    
    ofRunApp(roomWindow, roomApp);
    ofRunApp(previewWindow, roomPreview);
    ofRunApp(scene2DWindow, scene2DApp);
    ofRunApp(zenitWindow, zenitApp); // Lancement fenêtre Zenit
    
ofRunApp(mainWindow, dynamic_pointer_cast<ofBaseApp>(mainApp));
    
    ofSetFrameRate(60);
    ofRunMainLoop();
}