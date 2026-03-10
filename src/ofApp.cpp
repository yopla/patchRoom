#include "ofApp.h"
#include "ViewApp.h" // INDISPENSABLE : Pour accéder à moveWindow
#include "RoomPreview.h"      // <--- INDISPENSABLE pour accéder à setPaused
#include "ButtonApp.h"        // <--- INDISPENSABLE pour accéder à setEnabled
#include "ofAppGLFWWindow.h"  // <--- INDISPENSABLE pour accéder à setVisible
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h" // <--- AJOUTEZ CETTE LIGNE

//--------------------------------------------------------------
// Enregistrement des vues pour les piloter
void ofApp::registerViewApp(shared_ptr<ViewApp> vApp){
    viewApps.push_back(vApp);
}

//--------------------------------------------------------------
void ofApp::setup(){
        geminiGen.setup("AIzaSyBKPuu6w3yjiHl_WtDN97VLxdg2RMdMR3Q");

    ofSetRandomSeed(42);
    ofSetFrameRate(APP_FPS);
    bool molo = false; // <--- PASSEZ CECI À 'false' POUR LE TEST
    ofSetVerticalSync(molo);
    ofDisableArbTex();
    
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
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    if(dragInfo.files.size() > 0){
        // On prend le premier fichier de la liste
        string file = dragInfo.files[0];
        canvasManager.loadFile(file);
    }
}
// ----------------------------------------------------
void ofApp::update(){
    // --- CORRECTION : Lier scene2D à RoomPreview si ce n'est pas fait ---
    if(roomPreviewApp && scene2D && !roomPreviewApp->sceneSide) {
        roomPreviewApp->sceneSide = scene2D;
    }

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
        ofLogNotice("ofApp") << "Nouvelle vidéo disponible, chargement dans CanvasManager...";
        canvasManager.loadFile(geminiGen.getVideoPath());
    }
    
    // --- CHECK 360 IMAGE GENERATION ---
    if(geminiGen.hasNew360Image()) {
        string path = geminiGen.get360ImagePath();
        ofLogNotice("ofApp") << "Nouvelle image 360 disponible, chargement dans RoomApp...";
        if(roomApp) roomApp->atmosphere.loadTexture(path);
    }

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
        case 0: gabAlpha = 255; break;       // 100%
        case 1: gabAlpha = 90; break;        // 33%
        case 2: gabAlpha = 42; break;        // 10%
        case 3: gabAlpha = 0; break;         // OFF
    }

    // 1. Rendu dans le FBO Géant
    canvasManager.canvas.begin();
    
    canvasManager.drawBackground(
        roomApp, 
        scene2D, 
        sceneZenit, 
        gabAlpha,
        bDrawRoom,    // W
        bDrawZenit,   // X
        bDrawScene2D  // C
    );

    creatureSystem.draw(m);
    canvasManager.canvas.end();

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
    ofSetColor(255);
    ofDrawBitmapString("ESPACE + DRAG pour bouger | MOLETTE pour zoomer", 20, 20);

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

// bool bCallFocus = false; // pour plus tard

    if(key == OF_KEY_ESC) {
        bGlobalPause = !bGlobalPause;
        if(bGlobalPause) {
            oscTime = localTime;
        }
    }

    if(key == ' ') isSpacePressed = true;
    if(key == 'g' || key == 'G') {
        gabMode++;
        if(gabMode > 3) gabMode = 0;
    }
    
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

    // Reset Navigation
    if(key == 'r' || key == 'R') {
        float scaleX = (float)ofGetWidth() / canvasManager.width;
        float scaleY = (float)ofGetHeight() / canvasManager.height;
        masterZoom = std::min(scaleX, scaleY) * 0.9f;
        masterPan.x = (ofGetWidth() - canvasManager.width * masterZoom) / 2.0;
        masterPan.y = (ofGetHeight() - canvasManager.height * masterZoom) / 2.0;
    }
     ofVec2f m = getTransformedMouse();
    // Commandes CreatureSystem
    if(key == 'a' || key == 'A') creatureSystem.addRandomCreature(m.x, m.y);
    if(key == 'd' || key == 'D') creatureSystem.removeLast();
    if(key == 'z' || key == 'Z') creatureSystem.addCreature(m.x, m.y);

    
    // Toggles Layers
    if(key == 'w' || key == 'W') {
         bDrawRoom = !bDrawRoom; 
         if(roomApp) roomApp->setEnabled(bDrawRoom); 
        if(roomWindowPtr){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(roomWindowPtr);
            if(glfwWin) {
                if(bDrawRoom) {
                    glfwShowWindow(glfwWin->getGLFWWindow());
                    glfwFocusWindow(glfwWin->getGLFWWindow());
                    auto mainWin = dynamic_pointer_cast<ofAppGLFWWindow>(ofGetCurrentWindow());
                    if(mainWin) glfwFocusWindow(mainWin->getGLFWWindow());
                } else {
                   // glfwHideWindow(glfwWin->getGLFWWindow());
                }
            }
        }
    }
   
    if(key == 'x' || key == 'X') { 
        bDrawZenit = !bDrawZenit; 
        if(sceneZenit) sceneZenit->setEnabled(bDrawZenit); 
        if(zenitWindowPtr){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(zenitWindowPtr);
            if(glfwWin) {
                if(bDrawZenit) {
                    glfwShowWindow(glfwWin->getGLFWWindow());
                    glfwFocusWindow(glfwWin->getGLFWWindow());
                    auto mainWin = dynamic_pointer_cast<ofAppGLFWWindow>(ofGetCurrentWindow());
                    if(mainWin) glfwFocusWindow(mainWin->getGLFWWindow());
                } else {
                   // glfwHideWindow(glfwWin->getGLFWWindow());
                }
            }
        }
    }
    
        if(key == 'c' || key == 'C') {
         bDrawScene2D = !bDrawScene2D; 
         if(scene2D) scene2D->setEnabled(bDrawScene2D); 
         if(scene2DWindowPtr){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(scene2DWindowPtr);
            if(glfwWin) {
                if(bDrawScene2D) {
                    glfwShowWindow(glfwWin->getGLFWWindow());
                    glfwFocusWindow(glfwWin->getGLFWWindow());
                    auto mainWin = dynamic_pointer_cast<ofAppGLFWWindow>(ofGetCurrentWindow());
                    if(mainWin) glfwFocusWindow(mainWin->getGLFWWindow());
                } else {
                    //glfwHideWindow(glfwWin->getGLFWWindow());
                }
            }
        }
        }

// TOUCHE V : PREVIEW (Celle qui crashait)
    if(key == 'v' || key == 'V'){
        if(roomPreviewApp){
            bool bShow = roomPreviewApp->bPaused; // Si c'était en pause (caché), on veut afficher
            roomPreviewApp->setPaused(!bShow);           
            
            if(previewWindowPtr){
                auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(previewWindowPtr);
                if(glfwWin) {
                    if(bShow) {
                        glfwShowWindow(glfwWin->getGLFWWindow());
                        glfwFocusWindow(glfwWin->getGLFWWindow());
                        auto mainWin = dynamic_pointer_cast<ofAppGLFWWindow>(ofGetCurrentWindow());
                        if(mainWin) glfwFocusWindow(mainWin->getGLFWWindow());
                    } else {
                        //glfwHideWindow(glfwWin->getGLFWWindow());
                    }
                }
            }
        }
    }

    if(key == 'b' || key == 'B'){
        bDrawButtons = !bDrawButtons;
        if(buttonApp) buttonApp->setEnabled(bDrawButtons);

        if(buttonWindowPtr){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(buttonWindowPtr);
            if(glfwWin) {
                if(bDrawButtons) {
                    glfwShowWindow(glfwWin->getGLFWWindow());
                    glfwFocusWindow(glfwWin->getGLFWWindow());
                    auto mainWin = dynamic_pointer_cast<ofAppGLFWWindow>(ofGetCurrentWindow());
                    if(mainWin) glfwFocusWindow(mainWin->getGLFWWindow());
                } else {
                    //glfwHideWindow(glfwWin->getGLFWWindow());
                }
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' ') isSpacePressed = false;
}