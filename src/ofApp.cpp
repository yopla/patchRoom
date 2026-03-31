#include "ofApp.h"
#include "ViewApp.h" // INDISPENSABLE : Pour accéder à moveWindow
#include "RoomPreview.h"      // <--- INDISPENSABLE pour accéder à setPaused
#include "ButtonApp.h"        // <--- INDISPENSABLE pour accéder à setEnabled
#include "ofAppGLFWWindow.h"  // <--- INDISPENSABLE pour accéder à setVisible
#include "PlaylistVisualizerApp.h"
#include "AnnexeApp.h"
#include "AnnexePlayerApp.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h" // <--- AJOUTEZ CETTE LIGNE

//--------------------------------------------------------------
// Enregistrement des vues pour les piloter
void ofApp::registerViewApp(int index, shared_ptr<ViewApp> vApp){
    if(viewApps.size() < 4) viewApps.resize(4, nullptr); // S'assure qu'on a de la place pour 4
    if(index >= 0 && index < 4) viewApps[index] = vApp;
}

//--------------------------------------------------------------
void ofApp::setup(){
        geminiGen.setup("_");

    ofSetRandomSeed(42);
    ofSetFrameRate(APP_FPS);
    bool molo = true; // <--- PASSEZ CECI À 'false' POUR LE TEST
    ofSetVerticalSync(molo);
    ofDisableArbTex();
    
     ofDisableAntiAliasing(); // <-- Désactive l'anti-aliasing globalement
     ofDisableSmoothing();    // <-- Désactive le lissage des lignes (glHint)
    
    gabMode = 2;
    
    // Setup OSC via Manager
    oscManager.setup(HOST, SENDPORT, PORT);

    // 1. Initialisation du Canvas Géant (5024x5312)
    canvasManager.setup(5024, 5312);

    // 2. Initialisation du Système de Créatures
    creatureSystem.setup();
    perceptionSystem.setup();

    // 3. Placement initial des créatures (Hardcoded selon tes anciens réglages)
    /*
    creatureSystem.addRipple(2400 * 0.66, 736);
    creatureSystem.addRipple(2400 + (2624 * 0.33), 1544); 
    float sx = 0, sy = 2944, sw = 2400, sh = 2624;
    creatureSystem.addRipple(sx + sw * 0.25, sy + sh * 0.25);
    creatureSystem.addCreature(sx + sw * 0.75, sy + sh * 0.25);
    */

    // 4. Configuration Caméra Master
    float scaleX = (float)ofGetWidth() / canvasManager.width;
    float scaleY = (float)ofGetHeight() / canvasManager.height;
    masterZoom = std::min(scaleX, scaleY) * 0.9f;
    masterPan.x = (ofGetWidth() - canvasManager.width * masterZoom) / 2.0;
    masterPan.y = (ofGetHeight() - canvasManager.height * masterZoom) / 2.0;
    
    // Ajout d'un écouteur temporaire qui s'exécutera à la toute première frame affichée
    ofAddListener(ofEvents().draw, this, &ofApp::onFirstFrameReady);

    setupConnections();
}


void ofApp::setupConnections(){
    // --- CORRECTION : Lier scene2D à RoomPreview si ce n'est pas fait ---
    if(roomPreviewApp && scene2D) {
        roomPreviewApp->sceneSide = scene2D;
    }
    
    // --- LIAISON DES FBOs DE ROOMAPP VERS SCENE2D ---
    if(roomApp && scene2D) {
        scene2D->roomFboFront   = &roomApp->fboFront;
        scene2D->roomFboBack    = &roomApp->fboBack;
        scene2D->roomFboCour    = &roomApp->fboCour;
        scene2D->roomFboJar     = &roomApp->fboJar;
        scene2D->roomFboSol     = &roomApp->fboSol;
        scene2D->roomFboTopCour = &roomApp->fboTopCour;
        scene2D->roomFboTopJar  = &roomApp->fboTopJar;
    }

    // --- LIAISON DES FBOs DE SCENE2D VERS ROOMAPP ---
    if(roomApp && scene2D) {
        roomApp->scene2DFboFront   = &scene2D->fboFront;
        roomApp->scene2DFboBack    = &scene2D->fboBack;
        roomApp->scene2DFboCour    = &scene2D->fboCour;
        roomApp->scene2DFboJar     = &scene2D->fboJar;
        roomApp->scene2DFboSol     = &scene2D->fboSol;
        roomApp->scene2DFboTopCour = &scene2D->fboTopCour;
        roomApp->scene2DFboTopJar  = &scene2D->fboTopJar;
    }
    
    // --- LIAISON DES FBOs DE ROOMAPP VERS SCENE2DZENIT ---
    if(roomApp && sceneZenit) {
        sceneZenit->roomFboFront   = &roomApp->fboFront;
        sceneZenit->roomFboBack    = &roomApp->fboBack;
        sceneZenit->roomFboCour    = &roomApp->fboCour;
        sceneZenit->roomFboJar     = &roomApp->fboJar;
        sceneZenit->roomFboSol     = &roomApp->fboSol;
        sceneZenit->roomFboTopCour = &roomApp->fboTopCour;
        sceneZenit->roomFboTopJar  = &roomApp->fboTopJar;
    }

    // --- LIAISON DES FBOs DE SCENE2DZENIT VERS ROOMAPP ---
    if(roomApp && sceneZenit) {
        roomApp->zenitFboFront   = &sceneZenit->fboFront;
        roomApp->zenitFboBack    = &sceneZenit->fboBack;
        roomApp->zenitFboCour    = &sceneZenit->fboCour;
        roomApp->zenitFboJar     = &sceneZenit->fboJar;
        roomApp->zenitFboSol     = &sceneZenit->fboSol;
        roomApp->zenitFboTopCour = &sceneZenit->fboTopCour;
        roomApp->zenitFboTopJar  = &sceneZenit->fboTopJar;
    }

    // --- LIAISON DU PLAYER VERS LE VISUALISEUR ---
    if(playlistApp && roomApp) {
        playlistApp->player = &(roomApp->scene360VideoPlayer);
        playlistApp->bDrawScene360VideoPtr = &(roomApp->bDrawScene360Video);
        playlistApp->roomApp = roomApp.get();
    }

    // --- LIAISON DE SCENE2D VERS LE VISUALISEUR ---
    if(playlistApp && scene2D) {
        playlistApp->scene2D = scene2D.get();
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    if(dragInfo.files.size() > 0){
        // On prend le premier fichier de la liste
        string file = dragInfo.files[0];
        canvasManager.loadFile(file);
    }
}

//--------------------------------------------------------------
void ofApp::onFirstFrameReady(ofEventArgs & args){
        if(playlistWindowPtr) {
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(playlistWindowPtr);
            if(glfwWin) {
                glfwShowWindow(glfwWin->getGLFWWindow());
                glfwRestoreWindow(glfwWin->getGLFWWindow());
                glfwFocusWindow(glfwWin->getGLFWWindow());
            }
        }
        
        // Rassemblement des actions initiales pour les fenêtres des Vues (V1, V2, V3, V4)
        for(int i=0; i<4; i++) {
            if(viewApps.size() > i && viewApps[i] && viewApps[i]->myWindow) {
                auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(viewApps[i]->myWindow);
                if(glfwWin) {
                    // 1. Positionnement et redimensionnement initiaux (au lieu d'attendre 3 secondes)
                    viewApps[i]->myWindow->setWindowPosition(viewApps[i]->targetPos.x, viewApps[i]->targetPos.y);
                    if(viewApps[i]->targetSize.x > 0 && viewApps[i]->targetSize.y > 0) {
                        viewApps[i]->myWindow->setWindowShape(viewApps[i]->targetSize.x, viewApps[i]->targetSize.y);
                    }
                    viewApps[i]->bMoved = true;
                    
                    // 2. Masquage et mise en sourdine directe
                    glfwHideWindow(glfwWin->getGLFWWindow());
                    viewApps[i]->bEnabled = false; // Désactive le rendu de la vue
                }
            }
        }

    // On supprime l'écouteur instantanément pour qu'il ne pollue plus la boucle principale
    ofRemoveListener(ofEvents().draw, this, &ofApp::onFirstFrameReady);
}

// Classe helper pour le filtre Caps Lock, locale à ce fichier
class CapsLockFilter {
public:
    bool onKey(ofKeyEventArgs& args) {
        if(args.keycode == GLFW_KEY_CAPS_LOCK) {
            return true; // Consomme l'événement
        }
        return false;
    }
};

void ofApp::createAnnexeWindows() {
    // Empêche la double création
    if (annexeApp) {
        ofLogWarning("ofApp") << "Les fenêtres Annexe existent déjà.";
        return;
    }

    ofLogNotice("ofApp") << "Création des fenêtres Annexe...";

    ofGLFWWindowSettings settings;
    settings.shareContextWith = mainWindowPtr;
    settings.numSamples = 0;
    settings.decorated = true;
    settings.resizable = true;

    // Création de la fenêtre Annexe
    settings.setSize(800, 600);
    settings.setPosition(ofVec2f(100, 100));
    settings.title = "Annexe Win";
    annexeWindowPtr = ofCreateWindow(settings);

    // Création de la fenêtre Annexe Player
    settings.setSize(600, 600);
    settings.setPosition(ofVec2f(950, 100));
    settings.title = "Annexe Player";
    annexePlayerWindowPtr = ofCreateWindow(settings);

    // Création des Apps
    annexeApp = make_shared<AnnexeApp>();
    annexePlayerApp = make_shared<AnnexePlayerApp>();

    // Connexions
    annexePlayerApp->mainAppPtr = this;

    // Configuration des écouteurs de touches globaux pour les nouvelles fenêtres
    auto setupGlobalKeys = [&](shared_ptr<ofAppBaseWindow> win) {
        if(win) {
            static CapsLockFilter capsFilter; // Filtre statique pour ne pas le recréer
            ofAddListener(win->events().keyPressed, &capsFilter, &CapsLockFilter::onKey, OF_EVENT_ORDER_BEFORE_APP);
            ofAddListener(win->events().keyReleased, &capsFilter, &CapsLockFilter::onKey, OF_EVENT_ORDER_BEFORE_APP);
            ofAddListener(win->events().keyPressed, this, &ofApp::globalKeyPressed);
        }
    };
    
    setupGlobalKeys(annexeWindowPtr);
    setupGlobalKeys(annexePlayerWindowPtr);

    // Lancement des Apps
    ofRunApp(annexeWindowPtr, annexeApp);
    ofRunApp(annexePlayerWindowPtr, annexePlayerApp);
}

void ofApp::createZenitWindow() {
    if (sceneZenit) {
        ofLogWarning("ofApp") << "La fenetre Zenit existe deja.";
        return;
    }

    ofLogNotice("ofApp") << "Création de la fenêtre Zenit...";

    ofGLFWWindowSettings settings;
    settings.shareContextWith = mainWindowPtr;
    settings.numSamples = 0;
    settings.decorated = true;
    settings.resizable = true;
    settings.setSize(750, 750);
    settings.setPosition(ofVec2f(1000, 50));
    settings.title = "Scene2D Zenit";
    zenitWindowPtr = ofCreateWindow(settings);

    sceneZenit = make_shared<Scene2DZenit>();

    auto setupGlobalKeys = [&](shared_ptr<ofAppBaseWindow> win) {
        if(win) {
            static CapsLockFilter capsFilter; 
            ofAddListener(win->events().keyPressed, &capsFilter, &CapsLockFilter::onKey, OF_EVENT_ORDER_BEFORE_APP);
            ofAddListener(win->events().keyReleased, &capsFilter, &CapsLockFilter::onKey, OF_EVENT_ORDER_BEFORE_APP);
            ofAddListener(win->events().keyPressed, this, &ofApp::globalKeyPressed);
        }
    };
    
    setupGlobalKeys(zenitWindowPtr);

    ofRunApp(zenitWindowPtr, sceneZenit);
}

// ----------------------------------------------------
void ofApp::update(){
    // --- GESTION OSC (Réception & Envoi Frame) ---
    oscManager.update(this);

    // --- LOGIQUE PAUSE & TEMPS ---
    bool shouldUpdate = !bGlobalPause;
    
    // Si en pause mais que le temps local est en retard sur le temps OSC, on avance
    if(bGlobalPause) {
        if(localTime < oscTime - 0.5f) { // Tolérance de 0.5 frame
            shouldUpdate = true;
        } else if(localTime > oscTime + 5.0f) { // Tolérance de 5 frames (rewind)
            // Si le temps OSC est loin derrière (Rewind), on force le temps local
            // CORRECTION POUR ENREGISTREMENT : 
            // On désactive le saut brutal (snap) qui fait sauter des frames.
            // En mode rendu/enregistrement, on veut une continuité parfaite.
            localTime = oscTime; 
        }
    }

    if(shouldUpdate) {
        localTime += 1.0f; // Incrément fixe de 1 FRAME

        // Conversion en secondes uniquement pour les systèmes physiques (Noise, Animation)
        float timeSeconds = localTime / (float)APP_FPS;

        ofVec2f m = getTransformedMouse();
        creatureSystem.update(m, timeSeconds);
        canvasManager.update();
        
        // Mise à jour centralisée de la perception (Halos vs Boutons)
        perceptionSystem.update(buttonApp, scene2D, roomApp);

        // Propagation du temps et de la pause aux sous-systèmes
        if(roomApp) {
            roomApp->setLocalTime(timeSeconds);
        }
        if(sceneZenit) {
            sceneZenit->setLocalTime(timeSeconds);
        }
    }
    
    // Mise à jour du générateur (pour le polling vidéo)
    geminiGen.update();
    
    // --- CHECK VIDEO GENERATION ---
    if(geminiGen.hasNewVideo()) {
        string path = geminiGen.getVideoPath();
        ofLogNotice("ofApp") << "Nouvelle vidéo disponible, chargement dans Atmosphere...";
        if(roomApp) {
            if(roomApp->bDrawAtmosphere) roomApp->atmosphere.loadTexture(path);
            if(roomApp->bDrawCloudRing) roomApp->cloudRing.loadTexture(path);
            if(roomApp->bDrawLiquidSphere) roomApp->liquidSphereRing.loadTexture(path);
            if(roomApp->bDrawJellySphere) roomApp->jellySphereRing.loadTexture(path);
        }
    }
    
    // --- CHECK 360 IMAGE GENERATION ---
    if(geminiGen.hasNew360Image()) {
        string path = geminiGen.get360ImagePath();
        ofLogNotice("ofApp") << "Nouvelle image 360 disponible, chargement dans RoomApp...";
        if(roomApp) {
            if(roomApp->bDrawAtmosphere) roomApp->atmosphere.loadTexture(path);
            if(roomApp->bDrawCloudRing) roomApp->cloudRing.loadTexture(path);
            if(roomApp->bDrawLiquidSphere) roomApp->liquidSphereRing.loadTexture(path);
            if(roomApp->bDrawJellySphere) roomApp->jellySphereRing.loadTexture(path);
        }
    }

    canvasManager.setPaused(!shouldUpdate);
    if(scene2D) scene2D->setPaused(!shouldUpdate);
    if(roomApp) roomApp->setPaused(!shouldUpdate);
    if(sceneZenit) sceneZenit->setPaused(!shouldUpdate);

    ofSetWindowTitle("Master View | FPS: " + ofToString(ofGetFrameRate(), 1) 
                    + " | ms: " + ofToString(ofGetLastFrameTime() * 1000.0, 2)
                     + " | Frame: " + ofToString(ofGetFrameNum())
                     + " | Local Frame: " + ofToString((int)localTime)
                    );
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofVec2f m = getTransformedMouse();

    int gabAlpha = 0;
    switch(gabMode) {
        case 0: gabAlpha = 255; break;       // 100% (Opaque)
        case 1: gabAlpha = 190; break;       // 75%
        case 2: gabAlpha = 85; break;        // 33%
        case 3: gabAlpha = 25; break;        // 10% (Très léger)
        case 4: gabAlpha = 0; break;         // OFF
    }

    // 1. Rendu dans le FBO Géant
    canvasManager.canvas.begin();
    
    canvasManager.drawBackground(
        roomApp, 
        scene2D, 
        sceneZenit, 
        gabAlpha,
        bDrawRoom && bDiffuseRoom,    // W
        bDrawZenit,   // X
        bDrawScene2D && bDiffuseScene2D  // C
    );

    // Réinitialise la couleur et l'alpha pour ne pas affecter les créatures
    ofSetColor(255, 255, 255, 255);
    creatureSystem.draw(m);
    canvasManager.canvas.end();

    // --- ENREGISTREMENT DU CANVAS PRINCIPAL ---
    if(bRecordCanvas) {
        ofPixels pix;
        canvasManager.canvas.readToPixels(pix); // Lit la texture sans l'interface
        ofSaveImage(pix, canvasRecordFolder + "/frame_" + ofToString((long)localTime, 5, '0') + "." + recordFormat, recordQuality);
    }

    if(!bDrawMain) {
        ofBackground(0);
        return;
    }

    // 2. Affichage à l'écran avec Navigation (Zoom/Pan)
    ofBackground(0);
    ofPushMatrix();
        ofTranslate(masterPan.x, masterPan.y);
        ofScale(masterZoom, masterZoom);
        
        canvasManager.canvas.draw(0, 0);
        
        // Cadre de debug
        ofNoFill();
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, canvasManager.width, canvasManager.height);
    ofPopMatrix();

    // UI
    if(isSpacePressed) {
        ofDrawBitmapStringHighlight("ESPACE + DRAG pour bouger | MOLETTE pour zoomer", 20, 20, ofColor(50, 200, 50), ofColor(0));
    } else {
        ofSetColor(255);
        ofDrawBitmapString("ESPACE + DRAG pour bouger | MOLETTE pour zoomer", 20, 20);
    }

    // --- DEBUG GEMINI ---
    // Si une image est chargée, on l'affiche en haut à gauche (taille 300x300)
    if(geminiGen.getImage().isAllocated()) {
        ofSetColor(255);
        geminiGen.getImage().draw(0, 0, 300, 300);
    }
    // Feedback visuel pendant le chargement
    if(geminiGen.isGenerating()) {
        ofDrawBitmapStringHighlight("Generation IA en cours...", 20, 50, ofColor(255, 0, 0), ofColor(255));
    }

    // Feedback visuel curseur
    if (isSpacePressed) {
        ofPushStyle();
        ofSetColor(50, 200, 50, 150);
        ofFill();
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
        ofSetColor(255);
        ofNoFill();
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
        ofPopStyle();
    }
}

//--------------------------------------------------------------
ofVec2f ofApp::getTransformedMouse(){
    float mx = (ofGetMouseX() - masterPan.x) / masterZoom;
    float my = (ofGetMouseY() - masterPan.y) / masterZoom;
    return ofVec2f(mx, my);
}


//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
    if (scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse();
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    masterZoom = ofClamp(masterZoom * zoomFactor, 0.01f, 10.0f);
    masterPan.x = x - worldM.x * masterZoom;
    masterPan.y = y - worldM.y * masterZoom;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // Interaction Image Gemini (Click to close)
    if(geminiGen.getImage().isAllocated() && x < 300 && y < 300) {
        geminiGen.clearImage();
        return;
    }

    lastMousePos.set(x, y);
    if(!isSpacePressed){
        ofVec2f m = getTransformedMouse();
        creatureSystem.onPress(m.x, m.y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    ofVec2f m = getTransformedMouse();
    creatureSystem.onRelease(m.x, m.y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(isSpacePressed){
        ofVec2f currentMouse(x, y);
        masterPan += (currentMouse - lastMousePos);
        lastMousePos = currentMouse;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // Reset Navigation
    if(key == 'r' || key == 'R') {
        float scaleX = (float)ofGetWidth() / canvasManager.width;
        float scaleY = (float)ofGetHeight() / canvasManager.height;
        masterZoom = std::min(scaleX, scaleY) * 0.9f;
        masterPan.x = (ofGetWidth() - canvasManager.width * masterZoom) / 2.0;
        masterPan.y = (ofGetHeight() - canvasManager.height * masterZoom) / 2.0;
    }
     ofVec2f m = getTransformedMouse();

    if(key == ' ') isSpacePressed = true;
    
    if(key == 'g' || key == 'G') {
        gabMode++;
        if(gabMode > 4) gabMode = 0;
    }
    
      // Commandes CreatureSystem
    if(key == 'a' || key == 'A') {
        if (playlistApp) {
            if (playlistApp->controlsUI.selectedMainBrushIndex == 0) creatureSystem.addCreature(m.x, m.y);
            else if (playlistApp->controlsUI.selectedMainBrushIndex == 1) creatureSystem.addRandomCreature(m.x, m.y);
        }
    }
    if(key == 'z' || key == 'Z') {
        if(ofGetKeyPressed(OF_KEY_SHIFT)) {
            creatureSystem.clear();
        } else {
            creatureSystem.removeLast();
        }
    }



    // bool bCallFocus = false; // pour plus tard

    if(key == OF_KEY_ESC) {
        bGlobalPause = !bGlobalPause;
        if(bGlobalPause) {
            oscTime = localTime;
        }
    }


    
    // Toggles Layers
    if(key == 'w' || key == 'W') {
         bDrawRoom = !bDrawRoom; 
         if(roomApp) roomApp->setEnabled(bDrawRoom); 
    }
   
    if(key == 'x' || key == 'X') { 
        bDrawZenit = !bDrawZenit; 
        if(sceneZenit) sceneZenit->setEnabled(bDrawZenit); 
    }
    
        if(key == 'c' || key == 'C') {
         bDrawScene2D = !bDrawScene2D; 
         if(scene2D) scene2D->setEnabled(bDrawScene2D); 
        }

    if(key == 'v' || key == 'V'){
        if(roomPreviewApp){
            bool bShow = roomPreviewApp->bPaused; // Si c'était en pause (caché), on veut afficher
            roomPreviewApp->setPaused(!bShow);           
        }
    }

    if(key == 'b' || key == 'B'){
        bDrawButtons = !bDrawButtons;
        if(buttonApp) buttonApp->setEnabled(bDrawButtons);
    }

    


    /*   
Transform this room into
vector illustration of 

a surreal jukebox music machine

(in style of day of the tentacle) 
, keeping the structure 
but changing materials and lighting 
360 Panoramic view, equirectangular projection, vr, 8k, seamless
Et retire les traits de construction
le sol doit rester une textue de sol
(ne mets rien sur le sol)

et si c'est une vidéo : 
commence bien précisment sur la 1er image,
et termine bien sur la dernière frame précisement 
(n'essaie pas de relancer un début d'autre chose, il faut de bonnes coutures)
*/




    /*
    // TOUCHE I : Générer une image IA
    if((key == 'i' || key == 'I') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        geminiGen.generateImage("A futuristic mechanical doll with neon lights");
    }
    
    // TOUCHE O : Générer une vidéo IA (Veo)
    if((key == 'o' || key == 'O') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        geminiGen.generateVideo("Panomarmic Hdri image 360° VR (Equirectangular projection) (donc avec texture bouclé) D’une bete poilu dans un marais enchanté ");
    }

    // TOUCHE P : Générer une image 360 (Shift + P)
    if((key == 'p' || key == 'P') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        geminiGen.generateImage360("Panoramic 360 degree equirectangular projection (donc avec texture bouclée) of a surreal landscape, high resolution, 8k");
    }

    // TOUCHE M : Générer une image 360 Nano (Shift + M)
    if((key == 'm' || key == 'M') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        geminiGen.generateNano360("Panoramic 360 degree equirectangular projection (donc avec texture bouclée) of a surreal landscape, high resolution, 8k");
    }
  
    // TOUCHE L : Générer une image 360 depuis l'export Room (Shift + L)
    if((key == 'l' || key == 'L') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        string theme = "a surreal jukebox music machine";
        
        if(playlistApp) {
            theme = playlistApp->geminiUI.themeText;
            if(!playlistApp->geminiUI.apiKeyText.empty()) {
                geminiGen.setApiKey(playlistApp->geminiUI.apiKeyText);
            }
        }
        
        string prompt = string("Transform this room into ") +
                        "vector illustration of " + 
                        theme + 
                        " (in style of day of the tentacle) " +
                        ", keeping the structure " + 
                        "but changing materials and lighting";
        geminiGen.generateImage360FromImage(
            prompt, 
            "export_360_room.png"
        ); // sur le 5 dans roomApp
    }

    if((key == 'k' || key == 'K') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        geminiGen.generateVideoFromImage("une bete poilu dans un marais enchanté, slow cinematic movement", "gen360_last.jpg");
    }

    // TOUCHE J : Générer une vidéo IA depuis 2 images (Shift + J)
    if((key == 'j' || key == 'J') && ofGetKeyPressed(OF_KEY_SHIFT)) {
        geminiGen.generateVideoFromDeuxImages("A cinematic, haunting surealist video.", "gen360.jpg", "gen360_last.jpg");
    }

  */
  
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' ') isSpacePressed = false;
}

//--------------------------------------------------------------
void ofApp::globalKeyPressed(ofKeyEventArgs& args){
    int key = args.key;
    
    // Bloque l'exécution des raccourcis globaux si on est en train d'écrire dans la Playlist
    if (playlistApp && playlistApp->isTyping()) {
        return;
    }

    // Raccourci global 'N' pour afficher et focus la fenêtre Playlist
    if(key == 'n' || key == 'N'){
        if(playlistWindowPtr){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(playlistWindowPtr);
            if(glfwWin) {
                GLFWwindow* nativeWin = glfwWin->getGLFWWindow();
                bool isFocused = glfwGetWindowAttrib(nativeWin, GLFW_FOCUSED) != 0;
                
                if(isFocused && bDrawPlaylist) {
                    bDrawPlaylist = false;
                    if(playlistApp) playlistApp->setEnabled(false);
                    
                    auto focusWin = [](shared_ptr<ofAppBaseWindow> win) {
                        if(win) {
                            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(win);
                            if(glfwWin) glfwFocusWindow(glfwWin->getGLFWWindow());
                        }
                    };
                    
                    if(bDrawRoom) focusWin(roomWindowPtr);
                    if(bDrawZenit) focusWin(zenitWindowPtr);
                    if(bDrawScene2D) focusWin(scene2DWindowPtr);
                    if(roomPreviewApp && !roomPreviewApp->bPaused) focusWin(previewWindowPtr);
                    if(bDrawButtons) focusWin(buttonWindowPtr);
                    for(int i=0; i<4; i++) if(viewApps.size() > i && viewApps[i] && viewApps[i]->bEnabled) focusWin(viewApps[i]->myWindow);

                    // Rend le focus à la fenêtre principale en dernier
                    if(mainWindowPtr) {
                        focusWin(mainWindowPtr);
                    }
                } else {
                    bDrawPlaylist = true;
                    if(playlistApp) playlistApp->setEnabled(true);
                    glfwRestoreWindow(nativeWin); // La ramène au premier plan si elle était minimisée
                    glfwFocusWindow(nativeWin);   // Force le focus dessus
                }
            }
        }
    }
}