#include "AnnexeApp.h"
#include "SamController.h"

void AnnexeApp::setup() {
    ofSetBackgroundColor(30);
    
    // Centrage et dézoom initial pour voir le canevas en entier
    viewZoom = std::min((float)ofGetWidth() / targetWidth, (float)ofGetHeight() / targetHeight) * 0.95f;
    viewPan.x = (ofGetWidth() - targetWidth * viewZoom) / 2.0f;
    viewPan.y = (ofGetHeight() - targetHeight * viewZoom) / 2.0f;

    // SAM UI
    samController.setup();
}

void AnnexeApp::update() {
    rippleController.update(img);
    string samWarning = samController.getWarningMessage();
    if (!samWarning.empty()) {
        warningMessage = samWarning;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        samController.clearWarningMessage();
    }
}

void AnnexeApp::draw() {
    if (!bEnabled) {
        ofBackground(30);
        return;
    }

    ofBackground(30);
    
    ofPushMatrix();
    ofTranslate(viewPan.x, viewPan.y);
    ofScale(viewZoom, viewZoom);
    
    // Fond gris délimitant la zone ciblée
    ofSetColor(50);
    ofDrawRectangle(0, 0, targetWidth, targetHeight);
    
    if (img.isAllocated()) {
        ofSetColor(255);
        rippleController.draw(img, 0, 0, targetWidth, targetHeight);

        if (samController.isActive()) {
            samController.draw(targetWidth, targetHeight, viewZoom, img.getWidth(), img.getHeight());
        }

    } else {
        // Cadre rouge pour bien voir la délimitation quand c'est vide
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, targetWidth, targetHeight);
        ofFill();
    }
    
    // Rendu du layer Patteu
    patteuLayer.draw(targetWidth, targetHeight);
    deuPatteuLayer.draw(targetWidth, targetHeight);

    volumManager.draw(targetWidth, targetHeight);
    
    ofPopMatrix();

    // --- SAM Control UI ---
    if (samController.isActive()) {
        samController.drawUI();
        if (samController.isDragging()) {
            ofVec2f currentMouse = getTransformedMouse(); // in target space
            float imgMouseX = ofMap(currentMouse.x, 0, targetWidth, 0, img.getWidth());
            float imgMouseY = ofMap(currentMouse.y, 0, targetHeight, 0, img.getHeight());
            samController.drawDragFeedback({imgMouseX, imgMouseY}, targetWidth, targetHeight, img.getWidth(), img.getHeight(), viewZoom);
        }
    }
    
    // --- Recording logic ---
    if (bIsRecording) {
        saveRecordedFrame();
        // Visual feedback for recording
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofFill();
        ofDrawCircle(20, 20, 10); // Red dot in top-left corner
        ofPopStyle();
    }

    // --- Previsualisation de la brush Patteu ---
    if (patteuLayer.bActive) {
        ofVec2f worldM = getTransformedMouse();
        if (worldM.x >= 0 && worldM.x < targetWidth && worldM.y >= 0 && worldM.y < targetHeight) {
            ofPushStyle();
            ofNoFill();
            ofSetColor(255, 150); ofDrawCircle(ofGetMouseX(), ofGetMouseY(), patteuLayer.brushSize * viewZoom);
            ofSetColor(255, 50);  ofDrawCircle(ofGetMouseX(), ofGetMouseY(), patteuLayer.brushSize * patteuLayer.hardness * viewZoom);
            ofPopStyle();
        }
    }
    if (deuPatteuLayer.bActive) {
        ofVec2f worldM = getTransformedMouse();
        if (worldM.x >= 0 && worldM.x < targetWidth && worldM.y >= 0 && worldM.y < targetHeight) {
            ofPushStyle();
            ofNoFill();
            ofSetColor(255, 100, 255, 150); ofDrawCircle(ofGetMouseX(), ofGetMouseY(), deuPatteuLayer.brushSize * viewZoom);
            ofSetColor(255, 100, 255, 50);  ofDrawCircle(ofGetMouseX(), ofGetMouseY(), deuPatteuLayer.brushSize * deuPatteuLayer.hardness * viewZoom);
            ofPopStyle();
        }
    }

    // Message d'avertissement clignotant
    if (ofGetElapsedTimef() < warningEndTime) {
        if (sin(ofGetElapsedTimef() * 15.0f) > 0) { // Clignotement
            ofPushStyle();
            ofDrawBitmapStringHighlight(warningMessage, ofGetWidth() / 2.0f - warningMessage.length() * 4.0f, 50, ofColor(255, 0, 0), ofColor(255));
            ofPopStyle();
        }
    }
    
    // HUD Infos
    if (isSpacePressed) {
        ofDrawBitmapStringHighlight("ESPACE + DRAG: Pan | MOLETTE: Zoom | SHIFT: Force interaction | R: Reset", 10, ofGetHeight() - 20, ofColor(50, 200, 50), ofColor(0));
    } else {
        ofSetColor(255);
        ofDrawBitmapStringHighlight("ESPACE + DRAG: Pan | MOLETTE: Zoom | SHIFT: Force interaction | R: Reset", 10, ofGetHeight() - 20, ofColor(30, 30, 30, 200), ofColor(255));
    }
}

void AnnexeApp::saveHighResFrame() {
    ofFbo fbo;
    fbo.allocate(targetWidth, targetHeight, GL_RGB);
    
    fbo.begin();
    ofClear(50, 50, 50, 255); // Fond gris
    if (img.isAllocated()) {
        ofSetColor(255);
        rippleController.draw(img, 0, 0, targetWidth, targetHeight);
    } else {
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, targetWidth, targetHeight);
        ofFill();
    }
    patteuLayer.draw(targetWidth, targetHeight);
    deuPatteuLayer.draw(targetWidth, targetHeight);

    volumManager.draw(targetWidth, targetHeight);
    fbo.end();

    ofPixels pix;
    fbo.readToPixels(pix);
    
    ofDirectory dir("export");
    if(!dir.exists()) dir.create(true);
    
    string filename = "export/annexe_frame_" + ofGetTimestampString() + ".jpg";
    ofSaveImage(pix, filename, OF_IMAGE_QUALITY_HIGH);
    ofLogNotice("AnnexeApp") << "Frame haute resolution sauvegardee : " << filename;
    
    warningMessage = "Image sauvegardee : " + filename;
    warningEndTime = ofGetElapsedTimef() + 3.0f; // Feedback visuel temporaire
}

void AnnexeApp::toggleRecording() {
    bIsRecording = !bIsRecording;
    if (bIsRecording) {
        recordFrameCount = 0;
        recordFolder = "export/annexe_rec_" + ofGetTimestampString();
        ofDirectory dir(recordFolder);
        if (!dir.exists()) {
            dir.create(true);
        }
        ofLogNotice("AnnexeApp") << "Debut de l'enregistrement video dans : " << recordFolder;
        warningMessage = "REC ON: " + recordFolder;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    } else {
        ofLogNotice("AnnexeApp") << "Fin de l'enregistrement video. " << recordFrameCount << " frames sauvegardees.";
        warningMessage = "REC OFF: " + ofToString(recordFrameCount) + " frames";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::saveRecordedFrame() {
    ofFbo fbo;
    fbo.allocate(targetWidth, targetHeight, GL_RGB);
    
    fbo.begin();
    ofClear(50, 50, 50, 255);
    if (img.isAllocated()) {
        ofSetColor(255);
        rippleController.draw(img, 0, 0, targetWidth, targetHeight);
    } else {
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, targetWidth, targetHeight);
        ofFill();
    }
    volumManager.draw(targetWidth, targetHeight);
    fbo.end();

    ofPixels pix;
    fbo.readToPixels(pix);
    
    string filename = recordFolder + "/frame_" + ofToString(recordFrameCount, 6, '0') + ".jpg";
    ofSaveImage(pix, filename, OF_IMAGE_QUALITY_HIGH);
    
    recordFrameCount++;
}

void AnnexeApp::generateDepthMapAI() {
    string msg = aiManager.generateDepthMapAI(img);
    if (!msg.empty()) {
        warningMessage = msg;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::generateDepthMapDepthAnything() {
    string msg = aiManager.generateDepthMapDepthAnything(img);
    if (!msg.empty()) {
        warningMessage = msg;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::generateSAMCollider() {
    string msg = aiManager.generateSAMCollider(img);
    if (!msg.empty()) {
        warningMessage = msg;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::toggleSamControl() {
    bSamControlActive = !bSamControlActive;
    if (bSamControlActive) {
        if (!img.isAllocated()) {
            bSamControlActive = false; // Can't activate without an image
        }
        samController.activate(img);
    } else {
        samController.deactivate();
    }
}

void AnnexeApp::dragEvent(ofDragInfo dragInfo) {
    if (dragInfo.files.size() > 0) {
        string file = dragInfo.files[0];
        
        if (volumManager.bDepthMapActive) {
            string msg = volumManager.loadDepthMap(file, img, targetWidth, targetHeight);
            if (!msg.empty()) {
                warningMessage = msg;
                warningEndTime = ofGetElapsedTimef() + 4.0f;
            }
            return;
        } else if (volumManager.bLayerVolumActive) {
            volumManager.loadLayerVolum(file, img, targetWidth, targetHeight);
            return;
        }
        
        ofImage checkImg;
        if (checkImg.load(file)) {
            float imgRatio = checkImg.getWidth() / (float)checkImg.getHeight();
            float targetRatio = targetWidth / targetHeight;
            
            // Tolérance de 5% sur le ratio
            if (abs(imgRatio - targetRatio) > 0.05f) {
                warningMessage = "ATTENTION : Fichier aux mauvaises proportions ! (Ratio image: " + ofToString(imgRatio, 2) + " vs cible: " + ofToString(targetRatio, 2) + ")";
                warningEndTime = ofGetElapsedTimef() + 4.0f; // Affiche pendant 4 secondes
                ofLogWarning("AnnexeApp") << warningMessage;
            }
            
            img = checkImg;
            rippleController.setup(img);
            ofLogNotice("AnnexeApp") << "Image chargée dans l'annexe : " << file;
        }
    }
}

ofVec2f AnnexeApp::getTransformedMouse() {
    float mx = (ofGetMouseX() - viewPan.x) / viewZoom;
    float my = (ofGetMouseY() - viewPan.y) / viewZoom;
    return ofVec2f(mx, my);
}

void AnnexeApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (scrollY == 0) return;
    

    ofVec2f worldM = getTransformedMouse();
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    viewZoom = ofClamp(viewZoom * zoomFactor, 0.01f, 10.0f);
    viewPan.x = x - worldM.x * viewZoom;
    viewPan.y = y - worldM.y * viewZoom;
}

void AnnexeApp::mousePressed(int x, int y, int button) {
    lastMouse.set(x, y); // Toujours mettre a jour pour eviter les sauts de vue au drag
    ofVec2f worldM = getTransformedMouse();
    lastPatteuMouse = worldM;

    if (samController.isActive() && !isSpacePressed) {
        if (worldM.x >= 0 && worldM.x < targetWidth && worldM.y >= 0 && worldM.y < targetHeight) {
            float imgMouseX = ofMap(worldM.x, 0, targetWidth, 0, img.getWidth());
            float imgMouseY = ofMap(worldM.y, 0, targetHeight, 0, img.getHeight());
            if (samController.mousePressed(x, y, {imgMouseX, imgMouseY})) {
                return; // Événement consommé par le contrôleur
            }
        } else { // Clic en dehors de l'image, on gère les boutons quand même
            if (samController.mousePressed(x, y, {-1, -1}))
                return;
        }
        return; // Consume mouse press
    }

    if (rippleController.bActive && img.isAllocated() && !isSpacePressed) {
        // Check if click is inside the image area
        if (worldM.x >= 0 && worldM.x < targetWidth && worldM.y >= 0 && worldM.y < targetHeight) {
            int imgX = ofMap(worldM.x, 0, targetWidth, 0, img.getWidth());
            int imgY = ofMap(worldM.y, 0, targetHeight, 0, img.getHeight());
            rippleController.createRippleAt(imgX, imgY);
        }
    }
}

void AnnexeApp::mouseDragged(int x, int y, int button) {
    if (samController.isDragging()) {
        ofVec2f worldM = getTransformedMouse();
        float imgMouseX = ofMap(worldM.x, 0, targetWidth, 0, img.getWidth());
        float imgMouseY = ofMap(worldM.y, 0, targetHeight, 0, img.getHeight());
        samController.mouseDragged({imgMouseX, imgMouseY});
        return;
    }
    
    ofVec2f worldM = getTransformedMouse();

    // Logique d'estompe Patteu (on bloque le Pan si SHIFT est pressé)
    if (patteuLayer.bActive && (!isSpacePressed || isShiftPressed)) {
        patteuLayer.smudge(worldM, lastPatteuMouse, targetWidth, targetHeight);
        lastPatteuMouse = worldM;
        return;
    }
    if (deuPatteuLayer.bActive && (!isSpacePressed || isShiftPressed)) {
        deuPatteuLayer.smudge(worldM, lastPatteuMouse, targetWidth, targetHeight);
        lastPatteuMouse = worldM;
        return;
    }

    if (isSpacePressed || !isShiftPressed) {
        ofVec2f currentMouse(x, y);
        viewPan += (currentMouse - lastMouse);
    }
    lastMouse.set(x, y);
}
void AnnexeApp::mouseReleased(int x, int y, int button) {
    if (samController.isDragging()) {
        ofVec2f worldM = getTransformedMouse();
        float imgMouseX = ofMap(worldM.x, 0, targetWidth, 0, img.getWidth());
        float imgMouseY = ofMap(worldM.y, 0, targetHeight, 0, img.getHeight());
        samController.mouseReleased({imgMouseX, imgMouseY}, img);
    }
}

void AnnexeApp::keyPressed(int key) {
    if (key == ' ') isSpacePressed = true;
    if (key == OF_KEY_SHIFT) isShiftPressed = true;
    if (key == 'r' || key == 'R') setup(); // Reset vue
}

void AnnexeApp::keyReleased(int key) {
    if (key == ' ') isSpacePressed = false;
    if (key == OF_KEY_SHIFT) isShiftPressed = false;
}

void AnnexeApp::resetDepthMap() {
    volumManager.reset();
    warningMessage = "LayerVolum remis a plat";
    warningEndTime = ofGetElapsedTimef() + 2.0f;
}