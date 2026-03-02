#include "ofApp.h"
#include "ViewApp.h" // INDISPENSABLE : Pour accéder à moveWindow
#include "RoomPreview.h"      // <--- INDISPENSABLE pour accéder à setPaused
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
    
    ofSetRandomSeed(42);
    ofSetFrameRate(60);
    bool molo = true;
    ofSetVerticalSync(molo);
    ofDisableArbTex();
    
    // Setup OSC via Manager
    oscManager.setup(HOST, SENDPORT, PORT);

    // 1. Initialisation du Canvas Géant (5024x5312)
    canvasManager.setup(5024, 5312);

    // 2. Initialisation du Système de Créatures
    creatureSystem.setup();

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

    ofVec2f m = getTransformedMouse();
    creatureSystem.update(m);
    canvasManager.update();

    // --- GESTION OSC (Réception & Envoi Frame) ---
    oscManager.update(this);

    ofSetWindowTitle("Master View | FPS: " + ofToString(ofGetFrameRate(), 1) 
                    + " | ms: " + ofToString(ofGetLastFrameTime() * 1000.0, 2)
                     + " | Frame: " + ofToString(ofGetFrameNum())
                    );
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofVec2f m = getTransformedMouse();

    // 1. Rendu dans le FBO Géant
    canvasManager.canvas.begin();
    
    canvasManager.drawBackground(
        roomApp, 
        scene2D, 
        sceneZenit, 
        bShowFullGab,
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

bool bCallFocus = false; // pour plus tard

    if(key == ' ') isSpacePressed = true;
    if(key == 'g' || key == 'G') bShowFullGab = !bShowFullGab;
    
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
        if(roomWindowPtr && bDrawRoom && bCallFocus){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(roomWindowPtr);
            if(glfwWin) glfwFocusWindow(glfwWin->getGLFWWindow());
        }
    }
   
    if(key == 'x' || key == 'X') { 
        bDrawZenit = !bDrawZenit; 
        if(sceneZenit) sceneZenit->setEnabled(bDrawZenit); 
        if(scene2DWindowPtr && sceneZenit && bCallFocus){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(scene2DWindowPtr);
            if(glfwWin) glfwFocusWindow(glfwWin->getGLFWWindow());
        }
    }
    
        if(key == 'c' || key == 'C') {
         bDrawScene2D = !bDrawScene2D; 
         if(scene2D) scene2D->setEnabled(bDrawScene2D); 
         if(previewWindowPtr && bDrawScene2D && bCallFocus){
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(previewWindowPtr);
            if(glfwWin) {
                glfwFocusWindow(glfwWin->getGLFWWindow());
            }
        }
        }

// TOUCHE V : PREVIEW (Celle qui crashait)
    if(key == 'v' || key == 'V'){
        if(roomPreviewApp){
            bool newState = !roomPreviewApp->bPaused;
            roomPreviewApp->setPaused(newState);           
            
            if(previewWindowPtr && !newState){
                auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(previewWindowPtr);
                if(glfwWin) {
                    glfwFocusWindow(glfwWin->getGLFWWindow());
                }
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' ') isSpacePressed = false;
}