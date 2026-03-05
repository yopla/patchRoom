#include "ButtonApp.h"

//--------------------------------------------------------------
void ButtonApp::setup() {
     ofSetRandomSeed(42);
    buttonWindow.setup(ofGetWidth(), ofGetHeight());
    buttonWindow.setupWorms(12);
    
    // Init Navigation
    zoom = 1.0f;
    pan.set(0, 0);
    isSpacePressed = false;
    
    // Centrage initial
    float contentW = buttonWindow.getWidth();
    float contentH = buttonWindow.getHeight();
    if(contentW > 0 && contentH > 0) {
        float scale = std::min((float)ofGetWidth()/contentW, (float)ofGetHeight()/contentH) * 0.9f;
        zoom = scale;
        pan.x = (ofGetWidth() - contentW * zoom) / 2.0f;
        pan.y = (ofGetHeight() - contentH * zoom) / 2.0f;
    }
}

//--------------------------------------------------------------
void ButtonApp::update() {
    if(!bEnabled) return;
    // On met à jour en continu pour assurer la fluidité des animations (Lerp alpha)
    // même si la souris ne bouge pas.
    ofVec2f m = getTransformedMouse(ofGetMouseX(), ofGetMouseY());
    buttonWindow.update(m.x, m.y);
}

//--------------------------------------------------------------
void ButtonApp::draw() {
    if(!bEnabled) {
        ofBackground(0);
        return;
    }

    ofBackground(20); // Fond gris foncé
    
    ofPushMatrix();
    ofTranslate(pan);
    ofScale(zoom, zoom);
    
    buttonWindow.draw();
    
    ofPopMatrix();
    
    ofSetColor(255);
    ofDrawBitmapString("ESPACE + DRAG: Pan | MOLETTE: Zoom", 10, ofGetHeight() - 10);

    if(!buttonWindow.hoveredIndices.empty()) {
        // Dimensions de la pièce (Hardcodées ici pour correspondre à RoomApp/RoomPreview)
        float rW = 2400.0f;
        float rD = 2624.0f;
        float rSolD = 2368.0f;
        
        vector<ofVec3f> p3s = buttonWindow.get3DPosForActiveButtons(rW, rSolD, rD);
        ofDrawBitmapStringHighlight("Active Buttons: " + ofToString(p3s.size()), 10, ofGetHeight() - 30);
    }
}

//--------------------------------------------------------------
ofVec2f ButtonApp::getTransformedMouse(int x, int y) {
    return ofVec2f((x - pan.x) / zoom, (y - pan.y) / zoom);
}

//--------------------------------------------------------------
void ButtonApp::mouseMoved(int x, int y) {
    // Géré dans update() pour l'animation continue
}

//--------------------------------------------------------------
void ButtonApp::mousePressed(int x, int y, int button) {
    lastMouse.set(x, y);
}

//--------------------------------------------------------------
void ButtonApp::mouseDragged(int x, int y, int button) {
    if(isSpacePressed) {
        ofVec2f currentMouse(x, y);
        pan += (currentMouse - lastMouse);
        lastMouse = currentMouse;
    }
}

void ButtonApp::mouseReleased(int x, int y, int button) {}

void ButtonApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse(x, y);
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    zoom = ofClamp(zoom * zoomFactor, 0.01f, 50.0f);
    pan.x = x - worldM.x * zoom;
    pan.y = y - worldM.y * zoom;
}

void ButtonApp::keyPressed(int key) { 
    if(key == ' ') isSpacePressed = true; 

    if(key == 'v' || key == 'V') buttonWindow.toggleWorms();

    if(key == 'r' || key == 'R') {
        float contentW = buttonWindow.getWidth();
        float contentH = buttonWindow.getHeight();
        if(contentW > 0 && contentH > 0) {
            float scale = std::min((float)ofGetWidth()/contentW, (float)ofGetHeight()/contentH) * 0.9f;
            zoom = scale;
            pan.x = (ofGetWidth() - contentW * zoom) / 2.0f;
            pan.y = (ofGetHeight() - contentH * zoom) / 2.0f;
        }
    }
}
void ButtonApp::keyReleased(int key) { if(key == ' ') isSpacePressed = false; }
