#include "ofMain.h"
#include "ofApp.h"
#include "ViewApp.h"
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "RoomPreview.h" // Si vous avez gardé la preview précédente
#include "Scene2DZenit.h" // <--- AJOUT
#include "ButtonApp.h"

// Classe dérivée pour gérer l'enregistrement de la Vue 3
class RecordingViewApp : public ViewApp {
public:
    bool bRecording = false;
    string folderName;
    int frameCount = 0;
    shared_ptr<ofApp> mainApp;

    void keyPressed(int key) override {
        ViewApp::keyPressed(key);
        if(key == OF_KEY_RETURN){
            bRecording = !bRecording;
            if(bRecording) {
                folderName = "export/" + ofGetTimestampString();
                ofDirectory dir(folderName);
                dir.create(true);
                ofLogNotice() << "Start Recording View 3 to " << folderName;
            } else {
                ofLogNotice() << "Stop Recording View 3";
            }
        }
    }

    void draw() override {
        ViewApp::draw();
        if(bRecording){
            long currentFrame = frameCount;
            if(mainApp) currentFrame = (long)(mainApp->localTime * (float)APP_FPS);
            else frameCount++;
            ofSaveScreen(folderName + "/frame_" + ofToString(currentFrame, 5, '0') + ".jpg");
        }
    }
};

// Forward declaration pour la connexion
class ButtonApp;

// Classe dérivée pour gérer l'enregistrement de la RoomPreview (Vue ESC)
class RecordingRoomPreview : public RoomPreview {
public:
    shared_ptr<ButtonApp> buttonAppPtr; // Pointeur vers l'app des boutons
    bool bRecording = false;
    string folderName;
    int frameCount = 0;

    void draw() override {
        RoomPreview::draw(); // Dessin de base de la RoomPreview

        if(bRecording){
            long currentFrame = frameCount;
            if(mainApp) currentFrame = (long)(mainApp->localTime * (float)APP_FPS);
            else frameCount++;
            ofSaveScreen(folderName + "/frame_" + ofToString(currentFrame, 5, '0') + ".jpg");
        }
    }

    void keyPressed(int key) override {
        RoomPreview::keyPressed(key);
        // On utilise RETURN pour l'enregistrement (comme RecordingViewApp) pour libérer ESC
        if(key == OF_KEY_RETURN){
            bRecording = !bRecording;
            if(bRecording) {
                folderName = "export/preview_" + ofGetTimestampString();
                ofDirectory dir(folderName);
                dir.create(true);
                ofLogNotice() << "Start Recording RoomPreview to " << folderName;
            } else {
                ofLogNotice() << "Stop Recording RoomPreview";
            }
        }
        
        // On propage la pause générale sur ESC
        if(key == OF_KEY_ESC){
            if(mainApp) {
                mainApp->bGlobalPause = !mainApp->bGlobalPause;
                if(mainApp->bGlobalPause) {
                    mainApp->oscTime = mainApp->localTime;
                }
            }
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
        settings.setSize(4024, 4312); // Full Gabarit
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
    settings.shareContextWith = mainWindow; // Explicite pour la sécurité
    shared_ptr<ofAppBaseWindow> scene2DWindow = ofCreateWindow(settings);


    // Fenêtre 2D_GROUND
    settings.setSize(750, 750);
    settings.setPosition(ofVec2f(1000, 50));
    settings.resizable = true;
    settings.title = "Scene2D Zenit";
    settings.shareContextWith = mainWindow; // Explicite pour la sécurité
    shared_ptr<ofAppBaseWindow> zenitWindow = ofCreateWindow(settings);

    // Fenêtre Bouton
    settings.setSize(550, 550);
    settings.setPosition(ofVec2f(850, 50));
    settings.resizable = true;
    settings.title = "Boutons";
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> buttonWindow = ofCreateWindow(settings);



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
    shared_ptr<RecordingRoomPreview> recordingRoomPreview(new RecordingRoomPreview);
    shared_ptr<RoomPreview> roomPreview = recordingRoomPreview;

    shared_ptr<Scene2D_SIDE> scene2DApp(new Scene2D_SIDE);
    shared_ptr<Scene2DZenit> zenitApp(new Scene2DZenit);
    shared_ptr<ButtonApp> buttonApp(new ButtonApp);
    
    // ------------------------------------------------
    // 3. CONNEXIONS
    // ------------------------------------------------

    // le timer de setWindowMovement est dans ViewApp.update()
    if(bEnableView3) {
        auto recApp = dynamic_pointer_cast<RecordingViewApp>(viewApp3);
        if(recApp) recApp->mainApp = mainApp;
    }

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
    
    mainApp->buttonApp = buttonApp; // On donne l'app des boutons à ofApp
    // Connexion du buttonApp à la RoomPreview pour le dessin 3D
    recordingRoomPreview->buttonAppPtr = buttonApp;
    
    mainApp->roomPreviewApp = roomPreview; // On donne l'app à ofApp
    mainApp->previewWindowPtr = previewWindow; // On donne la fenêtre à ofApp
// --- AJOUTE CES LIGNES POUR LE FOCUS ---
    mainApp->roomWindowPtr = roomWindow;
    mainApp->scene2DWindowPtr = scene2DWindow;
    mainApp->zenitWindowPtr = zenitWindow; // Connexion de la fenêtre Zenit
    mainApp->previewWindowPtr = previewWindow; // Déjà présent
    mainApp->buttonWindowPtr = buttonWindow;

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
    ofRunApp(buttonWindow, buttonApp); // Lancement fenêtre Boutons
    
ofRunApp(mainWindow, dynamic_pointer_cast<ofBaseApp>(mainApp));
    
    ofSetFrameRate(APP_FPS);
    ofRunMainLoop();
}