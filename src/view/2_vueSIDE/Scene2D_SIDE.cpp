#include "Scene2D_SIDE.h"
#include "Scene2DLayerManager.h"
#include "ofApp.h"
#include "ColliderGenerator.h"
#include "ofxOpenCv.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

//--------------------------------------------------------------
void Scene2D_SIDE::setup() {
    localTime = 0.0f;
    
    // Chargement des images de fond
    imgJar.load("GAB1/JAR.jpg"); imgFront.load("GAB1/FRONT.jpg"); imgCour.load("GAB1/COUR.jpg"); imgBack.load("GAB1/BACK.jpg");
    imgSol.load("GAB1/SOL.jpg"); imgTopJar.load("GAB1/TOP_JAR.jpg"); imgTopCour.load("GAB1/TOP_COUR.jpg");

    // Optimisation : Redimensionner les images à la taille des FBO pour alléger le rendu
    if(imgJar.isAllocated()) imgJar.resize(wJar, 784);
    if(imgFront.isAllocated()) imgFront.resize(wFront, 1472);
    if(imgCour.isAllocated()) imgCour.resize(wJar, 1072);
    if(imgBack.isAllocated()) imgBack.resize(wFront, 1472);
    if(imgSol.isAllocated()) imgSol.resize(wSol, hSol);
    if(imgTopJar.isAllocated()) imgTopJar.resize(wTopJar, hTopJar);
    if(imgTopCour.isAllocated()) imgTopCour.resize(wTopCour, hTopCour);

    // Calcul des positions X
    srcX_Jar   = 0;
    srcX_Front = wJar;
    srcX_Cour  = wJar + wFront;
    srcX_Back  = wJar + wFront + wJar;

    totalSceneWidth = srcX_Back + wFront; 

    // --- SETUP LAYER MANAGER ---
    layerManager.setup(totalSceneWidth, wJar, srcX_Jar, wFront, srcX_Front);

    // Allocation FBOs
    fboJar.allocate(wJar, 784, GL_RGBA);
    fboFront.allocate(wFront, 1472, GL_RGBA);
    fboCour.allocate(wJar, 1072, GL_RGBA);
    fboBack.allocate(wFront, 1472, GL_RGBA);
    fboSol.allocate(wSol, hSol, GL_RGBA); 
    fboTopJar.allocate(wTopJar, hTopJar, GL_RGBA);
    fboTopCour.allocate(wTopCour, hTopCour, GL_RGBA);

    // View Navigation Init
    viewZoom = (float)ofGetWidth() / totalSceneWidth * 0.95f;
    viewPan.x = (ofGetWidth() - totalSceneWidth * viewZoom) / 2.0f;
    viewPan.y = (ofGetHeight() - hMax * viewZoom) / 2.0f;

    // --- Waypoints Balle ---
    float xFront = srcX_Front + wFront/2.0f; 
    float xCour  = srcX_Cour  + wJar/2.0f;   
    float xBack  = srcX_Back  + wFront/2.0f; 
    float xJar   = srcX_Jar   + wJar/2.0f;   

    ofVec2f pTopJar(xJar, -112);        
    ofVec2f pJar(xJar, 1080);           
    ofVec2f pFront(xFront, 736);        
    ofVec2f pSol(xFront, 2656);         
    ofVec2f pCour(xCour, 936);          
    ofVec2f pTopCour(xCour, -104);      
    ofVec2f pBack(xBack, 736);

    waypoints.clear();
    waypoints.push_back(pTopJar); waypoints.push_back(pJar);    
    waypoints.push_back(pFront);  waypoints.push_back(pSol);    
    waypoints.push_back(pFront);  waypoints.push_back(pCour);   
    waypoints.push_back(pTopCour); waypoints.push_back(pCour);   
    waypoints.push_back(pBack);   
}

//--------------------------------------------------------------
void Scene2D_SIDE::update() {
    // 1. HARD PAUSE : Si désactivé, on coupe tout calcul CPU
    if(!bEnabled) return;
    if(bPaused) return; // Pause générale (image figée)

    // 2. Gestion du Temps Local (Pour reprendre l'animation exactement où elle était)
    localTime += 1.0f / (float)APP_FPS;

    ofVec2f m = getTransformedMouse();

    layerManager.update(m, localTime, isSpacePressed);

    // 4. Animation Balle (Toujours active sauf si app désactivée)
    if (waypoints.size() > 1) {
        float totalDuration = 12.0f; 
        
        // UTILISATION DU TEMPS LOCAL
        float time = localTime; 
        
        float cycleTime = fmod(time, totalDuration * 2.0f);
        float val = (cycleTime < totalDuration) ? cycleTime : (2.0f * totalDuration - cycleTime);
        float progress = val / totalDuration;
        
        float scaledProgress = progress * (waypoints.size() - 1);
        int idx = (int)scaledProgress; 
        int nextIdx = idx + 1;         
        if (nextIdx >= waypoints.size()) nextIdx = waypoints.size() - 1;
        
        ballPos = waypoints[idx].getInterpolated(waypoints[nextIdx], scaledProgress - idx);
    }

    // 5. Captures FBO (Toujours actif pour le décor de fond)
    captureSection(fboFront, srcX_Front, 0, imgFront, roomFboFront, true);
    captureSection(fboBack,  srcX_Back,  0, imgBack,  roomFboBack, true);
    captureSection(fboJar,   srcX_Jar, 688, imgJar, roomFboJar, true);
    captureSection(fboCour,  srcX_Cour, 400, imgCour, roomFboCour, true);

    // TOP JAR
    captureSection(fboSol, srcX_Front, 1472, imgSol, roomFboSol, true);
    captureSection(fboTopJar, srcX_Jar, -912, imgTopJar, roomFboTopJar, true);
    captureSection(fboTopCour, srcX_Cour, -608, imgTopCour, roomFboTopCour, true);
}

//--------------------------------------------------------------
void Scene2D_SIDE::drawDynamicElements() {
    ofVec2f m = getTransformedMouse(); 

    layerManager.draw(m);

    // 6. BALLE (Toujours visible)g
    bool lastDebug = false;
    if (lastDebug) {
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofDrawCircle(ballPos.x, ballPos.y, 80); 
    ofPopStyle();
    }

}

//--------------------------------------------------------------
void Scene2D_SIDE::draw() {
    if(!bEnabled) {
        ofBackground(0);
        return;
    }

    ofBackground(0);
    ofPushMatrix();
    ofTranslate(viewPan.x, viewPan.y);
    ofScale(viewZoom);
    ofSetColor(255);
    
    // Pinceau curseur en direct dans le monde
    if (layerManager.bDrawCrayon) {
        ofVec2f m = getTransformedMouse();
        layerManager.crayon.drawCursor(m);
    }

    // Dessin debug de tous les FBOs assemblés
    fboJar.draw(srcX_Jar, hMax - 784);
    fboFront.draw(srcX_Front, hMax - 1472);
    fboCour.draw(srcX_Cour, hMax - 1072);
    fboBack.draw(srcX_Back, hMax - 1472);

    fboTopJar.draw(srcX_Jar, hMax - 784 - 1600); 
    fboSol.draw(srcX_Front, hMax); 
    fboTopCour.draw(srcX_Cour, hMax - 1072 - 1008);

    ofSetColor(255, 255, 255, 50);
    ofDrawLine(0, hMax, totalSceneWidth, hMax);

    // Affichage des coordonnées 3D pour les Halos
    bool drawCoordDebug = false;
    if (drawCoordDebug) {   
        for(auto& h : layerManager.halos) {
            ofVec3f p3 = get3DPos(h->pos.x, h->pos.y);
            ofDrawBitmapStringHighlight("3D: " + ofToString(p3), h->pos.x, h->pos.y);
        }
    }
    
    // --- SAM INTERACTIVE PREVIEW ---
    if (bSamControlActive && overlayImg.isAllocated()) {
        ofPushMatrix();
        ofTranslate(0, -912); // overlayImg coordinates start at -912
        
        if (bSamMaskGenerated && samPreviewMask.isAllocated()) {
            ofPushStyle();
            ofSetColor(255, 0, 0, 165);
            samPreviewMask.draw(0, 0);
            ofPopStyle();
        }

        for (size_t i = 0; i < samPoints.size(); i++) {
            if (samLabels[i] != 0 && samLabels[i] != 1) continue;
            ofPushStyle();
            if (samLabels[i] == 1) ofSetColor(0, 255, 0);
            else ofSetColor(255, 0, 0);
            ofFill();
            ofDrawCircle(samPoints[i].x, samPoints[i].y, 5 / viewZoom);
            ofPopStyle();
        }
        
        for (size_t i = 0; i < samPoints.size(); ++i) {
            if (samLabels[i] == 2 && i + 1 < samPoints.size() && samLabels[i+1] == 3) {
                ofVec2f p1 = samPoints[i];
                ofVec2f p2 = samPoints[i+1];
                ofPushStyle();
                ofNoFill();
                ofSetColor(0, 0, 255);
                ofSetLineWidth(2 / viewZoom);
                ofDrawRectangle(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
                ofPopStyle();
                i++;
            }
        }
        
        if (bSamIsDragging) {
            ofVec2f worldM = getTransformedMouse();
            float currentImgX = worldM.x;
            float currentImgY = worldM.y + 912;
            ofPushStyle();
            ofNoFill();
            ofSetColor(0, 0, 255, 150);
            ofSetLineWidth(1 / viewZoom);
            ofDrawRectangle(samDragStart.x, samDragStart.y, currentImgX - samDragStart.x, currentImgY - samDragStart.y);
            ofPopStyle();
        }
        
        ofPopMatrix();
    }

    ofPopMatrix();

    if (layerManager.bDrawPuyo) {
        int folded = 0;
        int complete = 0;
        layerManager.puyoLayer.getStats(folded, complete);
        ofDrawBitmapStringHighlight("Puyos: " + ofToString(complete) + " OK / " + ofToString(folded) + " Folded", 20, ofGetHeight() - 50);
    }

    // UI Pinceau statique par dessus le canevas
    if (layerManager.bDrawCrayon) {
        layerManager.crayon.drawUI(20, 20);
    }

    /*
    // Stats
    int nSardines = layerManager.getSardineCount();
    string stats = "ECOSYSTEME (Toggle H, J, K, L, M):\n";
    stats += "Creatures [H]: " + ofToString(layerManager.bDrawCreatures) + "\n";
    stats += "Poulpe    [J]: " + ofToString(layerManager.bDrawPoulpe) + "\n";
    stats += "Poissons  [K]: " + ofToString(layerManager.bDrawFish) + " (" + ofToString(nSardines) + ")\n";
    stats += "Sauteurs  [L]: " + ofToString(layerManager.bDrawSauteurs) + "\n";
    stats += "Slime     [M]: " + ofToString(layerManager.bDrawSlime) + "\n";
    stats += "Plantes   [P]: " + ofToString(layerManager.bDrawPlants);
    stats += "\nFlytraps  [O]: " + ofToString(layerManager.bDrawFlytraps);
    stats += "\nFluidFloor[V]: " + ofToString(layerManager.bDrawFluidFloor);
    stats += "\nMachine   [U]: " + ofToString(layerManager.bDrawMachine); 
    stats += "\nGears     [E]: " + ofToString(layerManager.bDrawGears);
    stats += "\nDigging   [T]: " + ofToString(layerManager.bDrawDigging);
    stats += "\nAutoMach  [X]: " + ofToString(layerManager.bDrawMachineAuto);
    stats += "\nCurtain   [1]: " + ofToString(layerManager.bDrawCurtain);
    stats += "\nColliders [5]: " + ofToString(layerManager.bDrawColliders);

    ofDrawBitmapStringHighlight(stats, 20, 30); 
    */

    // Message d'avertissement clignotant
    if (ofGetElapsedTimef() < warningEndTime) {
        if (sin(ofGetElapsedTimef() * 15.0f) > 0) { // Clignotement rapide
            ofPushStyle();
            ofSetColor(255);
            ofDrawBitmapStringHighlight(warningMessage, ofGetWidth() / 2.0f - warningMessage.length() * 4.0f, 50, ofColor(255, 0, 0), ofColor(255));
            ofPopStyle();
        }
    }
    
    if (bSamControlActive) {
        ofPushStyle();
        ofDrawBitmapStringHighlight("SAM CONTROL ACTIVE - FG: Clic | BG: TAB+Clic | BOX: Drag", 20, 100, ofColor(0), ofColor(255));
        ofPopStyle();
    }
}


//--------------------------------------------------------------
void Scene2D_SIDE::captureSection(ofFbo& targetFbo, float worldX, float worldTopY, ofImage& img, ofFbo* roomFbo, bool bDrawDynamics) {
    targetFbo.begin();
        ofClear(0, 0, 0, 0);
        
        if (bgDisplayMode == 0 && img.isAllocated()) {
            ofSetColor(255, 255, 255, 255);
            img.draw(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
        } else if (bgDisplayMode == 1 && img.isAllocated()) {
            ofSetColor(255, 255, 255, 180);
            img.draw(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
        } else if (bgDisplayMode == 2 && roomFbo != nullptr && roomFbo->isAllocated()) {
            ofSetColor(255, 255, 255, 255);
            roomFbo->draw(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
        } else {
             bool lastDebug = false;
        if (lastDebug) {
            ofNoFill(); ofSetColor(100);
            ofDrawRectangle(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
            ofFill();
        }
    }


        // L'overlay est maintenant injecté DIRECTEMENT dans les FBOs
        if (overlayMode > 0 && overlayImg.isAllocated()) {
            ofPushStyle();
            ofEnableAlphaBlending();
            if (overlayMode == 1) ofSetColor(255, 255, 255, 84);
            else if (overlayMode == 2) ofSetColor(255, 255, 255, 191);
            else if (overlayMode == 3) ofSetColor(255, 255, 255, 255);
            
            ofPushMatrix();
            ofTranslate(-worldX, -worldTopY);
            overlayImg.draw(0, -912);
            ofPopMatrix();
            ofPopStyle();
        }
        
        if (bDrawDynamics) {
            ofSetColor(255);
            ofPushMatrix();
                ofTranslate(-worldX, -worldTopY);
                drawDynamicElements(); // This now calls layerManager.draw()
            ofPopMatrix();
        }
        
    targetFbo.end();
}

//--------------------------------------------------------------
// INPUTS
//--------------------------------------------------------------
void Scene2D_SIDE::mouseScrolled(int x, int y, float sx, float sy) {
    ofVec2f mouseBeforeZoom = getTransformedMouse();
    viewZoom = ofClamp(viewZoom + sy * 0.01, 0.01, 2.0);
    ofVec2f mouseAfterZoom = (ofVec2f(x, y) - viewPan) / viewZoom;
    viewPan += (mouseAfterZoom - mouseBeforeZoom) * viewZoom;
}

void Scene2D_SIDE::mousePressed(int x, int y, int button) { 
    lastMouse.set(x, y); 
    
    if (layerManager.bDrawCrayon) {
        if (layerManager.crayon.mousePressedScreen(x, y, 20, 20)) {
            return; // Clic capturé par l'interface du crayon
        }
    }
    
    if (bSamControlActive && !isSpacePressed) {
        ofVec2f worldM = getTransformedMouse();
        float imgX = worldM.x;
        float imgY = worldM.y + 912;
        if (overlayImg.isAllocated() && imgX >= 0 && imgX < overlayImg.getWidth() && imgY >= 0 && imgY < overlayImg.getHeight()) {
            samMousePressTime = ofGetElapsedTimef();
            samDragStart = ofVec2f(imgX, imgY);
            bSamIsDragging = true;
        }
        return; 
    }

    if(!isSpacePressed) {
        ofVec2f m = getTransformedMouse();
        layerManager.mousePressed(m, button);
    }
}

void Scene2D_SIDE::mouseReleased(int x, int y, int button) {
    if (bSamControlActive && bSamIsDragging) {
        bSamIsDragging = false;
        ofVec2f worldM = getTransformedMouse();
        float imgX = worldM.x;
        float imgY = worldM.y + 912;

        float dragDist = samDragStart.distance(ofVec2f(imgX, imgY));
        
        if (dragDist < 10) { // click
            if (!ofGetKeyPressed(OF_KEY_TAB)) { // foreground
                samPoints.push_back(ofVec2f(imgX, imgY));
                samLabels.push_back(1);
            } else { // background
                samPoints.push_back(ofVec2f(imgX, imgY));
                samLabels.push_back(0);
            }
        } else { // drag box
            float x1 = std::min(samDragStart.x, imgX);
            float y1 = std::min(samDragStart.y, imgY);
            float x2 = std::max(samDragStart.x, imgX);
            float y2 = std::max(samDragStart.y, imgY);
            samPoints.push_back(ofVec2f(x1, y1));
            samLabels.push_back(2);
            samPoints.push_back(ofVec2f(x2, y2));
            samLabels.push_back(3);
        }
        runSamInference();
        return;
    }

    ofVec2f m = getTransformedMouse();
    layerManager.mouseReleased(m, button);
}

void Scene2D_SIDE::mouseDragged(int x, int y, int button) {
    if (bSamControlActive && bSamIsDragging) {
        lastMouse.set(x, y);
        return;
    }
    if (isSpacePressed) {
        viewPan += (ofVec2f(x, y) - lastMouse);
    } else if (layerManager.bDrawCrayon) {
        ofVec2f m = getTransformedMouse();
        layerManager.mouseDragged(m, button);
    } else if (!ofGetKeyPressed(OF_KEY_SHIFT)) {
        viewPan += (ofVec2f(x, y) - lastMouse);
    }
    lastMouse.set(x, y);
}

void Scene2D_SIDE::keyPressed(int key) {
    if (key == ' ') isSpacePressed = true; 
    if (key == 'g' || key == 'G') {
        bgDisplayMode = (bgDisplayMode + 1) % 4;
    }
    
    if (key == 'h' || key == 'H') {
        overlayMode = (overlayMode + 1) % 4;
    }
    
    if (key == 'r' || key == 'R') {
        viewZoom = (float)ofGetWidth() / totalSceneWidth * 0.95f;
        viewPan.x = (ofGetWidth() - totalSceneWidth * viewZoom) / 2.0f;
        viewPan.y = (ofGetHeight() - hMax * viewZoom) / 2.0f;
    }
    
    if (key == OF_KEY_RETURN) {
        if (ofGetKeyPressed(OF_KEY_SHIFT)) {
            exportFullScene();
        } else {
            exportColliders();
        }
    }
    
    layerManager.keyPressed(key, getTransformedMouse());
}

void Scene2D_SIDE::keyReleased(int key) {
    if (key == ' ') isSpacePressed = false; 
}

ofVec2f Scene2D_SIDE::getTransformedMouse() {
    float mx = (ofGetMouseX() - viewPan.x) / viewZoom;
    float my = (ofGetMouseY() - viewPan.y) / viewZoom;
    return ofVec2f(mx, my);
}

//--------------------------------------------------------------
void Scene2D_SIDE::dragEvent(ofDragInfo dragInfo) {
    if (dragInfo.files.size() > 0) {
        string file = dragInfo.files[0];
        
        ofImage checkImg;
        if (checkImg.load(file)) {
            float imgRatio = checkImg.getWidth() / checkImg.getHeight();
            
            // Verification des proportions attendues (10048/1472 = ~6.82, 10048/4752 = ~2.11)
            if (abs(imgRatio - 6.826f) > 0.2f && abs(imgRatio - 2.114f) > 0.2f) {
                warningMessage = "ATTENTION : Fichier aux mauvaises proportions ! (Ratio: " + ofToString(imgRatio, 2) + ")";
                warningEndTime = ofGetElapsedTimef() + 4.0f; // Affiche pendant 4 secondes
                ofLogWarning("Scene2D_SIDE") << warningMessage;
            }

            if (layerManager.bDrawColliders && layerManager.colliderLayer) {
                layerManager.colliderLayer->loadMap(file);
                ofLogNotice("Scene2D_SIDE") << "Collider map image chargee : " << file;
            } else if (layerManager.bDrawEatMap && layerManager.eatMapLayer) {
                layerManager.eatMapLayer->loadMap(file);
                ofLogNotice("Scene2D_SIDE") << "EatMap image chargee : " << file;
            } else if (layerManager.bDrawSurSauteurs) {
                layerManager.surSauteurLayer.loadTexture(file);
                ofLogNotice("Scene2D_SIDE") << "SurSauteur texture chargee : " << file;
            } else {
                float exportHeight = 912 + 1472 + 2368; // 4752
                checkImg.resize(totalSceneWidth, exportHeight); // Redimensionnement pour correspondre a la scene
                overlayImg = checkImg;
                overlayMode = 3;
                ofLogNotice("Scene2D_SIDE") << "Overlay image chargee et redimensionnee : " << file;
            }
        }
    }
}

//--------------------------------------------------------------
ofVec3f Scene2D_SIDE::get3DPos(float x, float y) {
    float w2 = 1200.0f; // roomWidth/2
    float d2 = 1312.0f; // roomDepth/2
    
    // JAR (Mur Gauche)
    if (x >= srcX_Jar && x < srcX_Jar + wJar) {
        if (y >= 688 && y <= 688 + 784) { // Mur Vertical
            float u = (x - srcX_Jar) / wJar;
            float v = (y - 688) / 784.0f;
            float z = d2 - u * (2*d2); // Z va de d2 à -d2
            float y3d = 784.0f * (1.0f - v);
            return ofVec3f(-w2, y3d, z);
        }
        else if (y < 688) { // Toit Jar
            float u = (x - srcX_Jar) / wJar;
            float v = (y - (-912)) / 1600.0f;
            ofVec3f pBack(252, 1452, 1312); 
            ofVec3f pFront(252, 1452, -1312); 
            ofVec3f wJB(-1200, 784, 1312);
            ofVec3f wJF(-1200, 784, -1312);
            ofVec3f top = pBack.getInterpolated(pFront, u);
            ofVec3f bot = wJB.getInterpolated(wJF, u);
            return top.getInterpolated(bot, v);
        }
    }
    
    // FRONT (Mur Fond)
    if (x >= srcX_Front && x < srcX_Front + wFront) {
        if (y >= 0 && y <= 1472) { // Mur Vertical
            float u = (x - srcX_Front) / wFront;
            float v = y / 1472.0f;
            float x3d = -w2 + u * (2*w2);
            float y3d = 1472.0f * (1.0f - v);
            return ofVec3f(x3d, y3d, -d2);
        }
        else if (y > 1472) { // Sol
            float u = (x - srcX_Front) / wFront;
            float v = (y - 1472) / 2368.0f;
            float x3d = -w2 + u * (2*w2);
            float z3d = -d2 + v * 2368.0f;
            return ofVec3f(x3d, 0, z3d);
        }
    }
    
    // COUR (Mur Droit)
    if (x >= srcX_Cour && x < srcX_Cour + wJar) {
         if (y >= 400 && y <= 400 + 1072) { // Mur Vertical
            float u = (x - srcX_Cour) / wJar;
            float v = (y - 400) / 1072.0f;
            float z = -d2 + u * (2*d2); // Z va de -d2 à d2
            float y3d = 1072.0f * (1.0f - v);
            return ofVec3f(w2, y3d, z);
         }
         else if (y < 400) { // Toit Cour
            float u = (x - srcX_Cour) / wJar;
            float v = (y - (-608)) / 1008.0f;
            ofVec3f pFront(252, 1452, -1312);
            ofVec3f pBack(252, 1452, 1312);
            ofVec3f wCF(1200, 1072, -1312);
            ofVec3f wCB(1200, 1072, 1312);
            ofVec3f top = pFront.getInterpolated(pBack, u);
            ofVec3f bot = wCF.getInterpolated(wCB, u);
            return top.getInterpolated(bot, v);
         }
    }
    
    // BACK (Mur Arrière)
    if (x >= srcX_Back && x < srcX_Back + wFront) {
        float u = (x - srcX_Back) / wFront;
        float v = y / 1472.0f;
        float x3d = w2 - u * (2*w2); // X va de w2 à -w2
        float y3d = 1472.0f * (1.0f - v);
        return ofVec3f(x3d, y3d, d2);
    }

    return ofVec3f(0,0,0);
}

//--------------------------------------------------------------
void Scene2D_SIDE::exportFullScene() {
    // EXPORT FULL SCENE 2D (Shift + Return)
    float exportHeight = 912 + 1472 + 2368; // top = -912, centre = 1472, sol = +2368
    ofFbo fboExp;
    fboExp.allocate(totalSceneWidth, exportHeight, GL_RGBA);
    fboExp.begin();
    ofClear(0, 0, 0, 255); // Fond noir opaque
    ofPushMatrix();
    ofTranslate(0, 912); // Décaler vers le bas pour capturer les éléments en Y négatif (TopJar, TopCour)
    
    ofSetColor(255);
    fboJar.draw(srcX_Jar, hMax - 784);
    fboFront.draw(srcX_Front, hMax - 1472);
    fboCour.draw(srcX_Cour, hMax - 1072);
    fboBack.draw(srcX_Back, hMax - 1472);

    fboTopJar.draw(srcX_Jar, hMax - 784 - 1600); 
    fboSol.draw(srcX_Front, hMax); 
    fboTopCour.draw(srcX_Cour, hMax - 1072 - 1008);
    
    ofPopMatrix();
    fboExp.end();
    ofPixels pix;
    fboExp.readToPixels(pix);
    ofSaveImage(pix, "scene2D_full_export_" + ofGetTimestampString() + ".png");
    ofLogNotice("Scene2D_SIDE") << "Export complet sauvegarde: scene2D_full_export_...";
}

//--------------------------------------------------------------
void Scene2D_SIDE::exportColliders() {
    // EXPORT COLLIDERS (Return simple)
    float exportHeight = 912 + 1472 + 2368; // 4752
    ofFbo fboExp;
    fboExp.allocate(totalSceneWidth, exportHeight, GL_RGBA);
    fboExp.begin();
    ofClear(0, 0, 0, 0);
    ofPushMatrix();
    ofTranslate(0, 912); // Décalage pour inclure les toits
    if(layerManager.colliderLayer) {
        layerManager.colliderLayer->draw();
    }
    ofPopMatrix();
    fboExp.end();
    ofPixels pix;
    fboExp.readToPixels(pix);
    ofSaveImage(pix, "colliders_export_" + ofGetTimestampString() + ".png");
    ofLogNotice("Scene2D_SIDE") << "Export colliders (10048x4752) sauvegarde: colliders_export_...";
}

//--------------------------------------------------------------
void Scene2D_SIDE::generateColliderFromOverlay() {
    if (overlayImg.isAllocated()) {
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        string path = "export/collider_generated_" + ofGetTimestampString() + ".png";
        ColliderGenerator::generateAndSave(overlayImg, path);
        ofLogNotice("Scene2D_SIDE") << "Collider genere et sauvegarde : " << path;
    } else {
        ofLogWarning("Scene2D_SIDE") << "Pas d'overlay charge pour generer le collider.";
    }
}

//--------------------------------------------------------------
void Scene2D_SIDE::generateColliderFromAI() {
    if (overlayImg.isAllocated()) {
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        string path = "export/collider_AI_generated_" + ofGetTimestampString() + ".png";
        ColliderGenerator::generateWithAI(overlayImg, path);
    } else {
        ofLogWarning("Scene2D_SIDE") << "Pas d'overlay charge pour generer le collider via l'IA.";
    }
}

//--------------------------------------------------------------
void Scene2D_SIDE::generateColliderFromSAM() {
    if (overlayImg.isAllocated()) {
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        string path = "export/collider_SAM_generated_" + ofGetTimestampString() + ".png";
        ColliderGenerator::generateWithSAM(overlayImg, path);
    } else {
        ofLogWarning("Scene2D_SIDE") << "Pas d'overlay charge pour generer le collider via SAM.";
    }
}

//--------------------------------------------------------------
void Scene2D_SIDE::generateColliderFromDexined() {
    if (overlayImg.isAllocated()) {
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        string path = "export/collider_DEX_generated_" + ofGetTimestampString() + ".png";
        ColliderGenerator::generateWithDexined(overlayImg, path);
    } else {
        ofLogWarning("Scene2D_SIDE") << "Pas d'overlay charge pour generer le collider via Dexined.";
    }
}

//--------------------------------------------------------------
void Scene2D_SIDE::generateColliderFromDepthAnything() {
    if (overlayImg.isAllocated()) {
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        string path = "export/scene2d_depthanything_" + ofGetTimestampString() + ".png";
        ColliderGenerator::generateWithDepthAnything(overlayImg, path);
    } else {
        ofLogWarning("Scene2D_SIDE") << "Pas d'overlay charge pour generer la depth map via DepthAnything.";
    }
}

//--------------------------------------------------------------
void Scene2D_SIDE::exportEatMap() {
    // EXPORT EATMAP
    float exportHeight = 912 + 1472 + 2368; // 4752
    ofFbo fboExp;
    fboExp.allocate(totalSceneWidth, exportHeight, GL_RGBA);
    fboExp.begin();
    ofClear(0, 0, 0, 0);
    ofPushMatrix();
    ofTranslate(0, 912); // Décalage pour inclure les toits
    if(layerManager.eatMapLayer && layerManager.eatMapLayer->bHasMap) {
        ofSetColor(255, 255, 255, 255); // Dessin opaque pour récupérer la vraie couche (sans la transparence d'affichage)
        layerManager.eatMapLayer->mapImage.draw(
            0, 
            -layerManager.eatMapLayer->mapSimOffsetY * layerManager.eatMapLayer->scale, 
            layerManager.eatMapLayer->simWidth * layerManager.eatMapLayer->scale, 
            layerManager.eatMapLayer->mapH * layerManager.eatMapLayer->scale
        );
    }
    ofPopMatrix();
    fboExp.end();
    ofPixels pix;
    fboExp.readToPixels(pix);
    ofSaveImage(pix, "eatmap_export_" + ofGetTimestampString() + ".png");
    ofLogNotice("Scene2D_SIDE") << "Export eatmap (10048x4752) sauvegarde: eatmap_export_...";
}

//--------------------------------------------------------------
void Scene2D_SIDE::export7Murs() {
    ofDirectory dir("export/murs2D");
    if(!dir.exists()){
        dir.create(true);
    }

    // Assemblage de la scène courante dans un FBO (comme l'export global)
    float exportHeight = 912 + 1472 + 2368; // 4752
    ofFbo fboExp;
    fboExp.allocate(totalSceneWidth, exportHeight, GL_RGBA);
    fboExp.begin();
    ofClear(0, 0, 0, 0); // Fond transparent pour les PNGs
    ofPushMatrix();
    ofTranslate(0, 912); 
    
    ofSetColor(255);
    fboJar.draw(srcX_Jar, hMax - 784);
    fboFront.draw(srcX_Front, hMax - 1472);
    fboCour.draw(srcX_Cour, hMax - 1072);
    fboBack.draw(srcX_Back, hMax - 1472);

    fboTopJar.draw(srcX_Jar, hMax - 784 - 1600); 
    fboSol.draw(srcX_Front, hMax); 
    fboTopCour.draw(srcX_Cour, hMax - 1072 - 1008);

    ofPopMatrix();
    fboExp.end();

    ofPixels pix;
    fboExp.readToPixels(pix);
    
    ofPixels pFront, pBack, pJar, pCour, pSol, pTopJar, pTopCour;

    // Découpage selon les offsets exacts du template
    pix.cropTo(pFront, srcX_Front, 912, wFront, 1472);
    pix.cropTo(pBack, srcX_Back, 912, wFront, 1472);
    pix.cropTo(pJar, srcX_Jar, 1600, wJar, 784);
    pix.cropTo(pCour, srcX_Cour, 1312, wJar, 1072);
    pix.cropTo(pSol, srcX_Front, 2384, wSol, hSol);
    pix.cropTo(pTopJar, srcX_Jar, 0, wTopJar, hTopJar);
    pix.cropTo(pTopCour, srcX_Cour, 304, wTopCour, hTopCour);

    string ts = ofGetTimestampString();
    ofSaveImage(pFront, "export/murs2D/FRONT_" + ts + ".png");
    ofSaveImage(pBack, "export/murs2D/BACK_" + ts + ".png");
    ofSaveImage(pJar, "export/murs2D/JAR_" + ts + ".png");
    ofSaveImage(pCour, "export/murs2D/COUR_" + ts + ".png");
    ofSaveImage(pSol, "export/murs2D/SOL_" + ts + ".png");
    ofSaveImage(pTopJar, "export/murs2D/TOP_JAR_" + ts + ".png");
    ofSaveImage(pTopCour, "export/murs2D/TOP_COUR_" + ts + ".png");

    ofLogNotice("Scene2D_SIDE") << "Export des 7 murs effectue avec succes dans export/murs2D/";
}

//--------------------------------------------------------------
void Scene2D_SIDE::toggleSamControl() {
    bSamControlActive = !bSamControlActive;
    if (bSamControlActive) {
        if (!overlayImg.isAllocated()) {
            warningMessage = "Chargez un overlay (drag&drop) d'abord !";
            warningEndTime = ofGetElapsedTimef() + 3.0f;
            bSamControlActive = false;
            return;
        }
        resetSamSelection();
        warningMessage = "Mode Controle SAM active";
        warningEndTime = ofGetElapsedTimef() + 2.0f;
    } else {
        warningMessage = "Mode Controle SAM desactive";
        warningEndTime = ofGetElapsedTimef() + 2.0f;
    }
}

void Scene2D_SIDE::resetSamSelection() {
    samPoints.clear();
    samLabels.clear();
    samPreviewMask.clear();
    bSamMaskGenerated = false;
}

void Scene2D_SIDE::saveSamSegmentation() {
    if (!bSamMaskGenerated || !samPreviewMask.isAllocated()) {
        warningMessage = "Aucun masque a sauvegarder !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }
    ofDirectory dir("export");
    if(!dir.exists()) dir.create(true);
    string filename = "export/scene2d_sam_interactive_" + ofGetTimestampString() + ".png";
    
    ofPixels finalPix;
    finalPix.allocate(samPreviewMask.getWidth(), samPreviewMask.getHeight(), OF_PIXELS_RGBA);
    for(int i=0; i<samPreviewMask.getPixels().size(); i++){
        unsigned char val = samPreviewMask.getPixels()[i];
        finalPix[i*4+0] = 255;
        finalPix[i*4+1] = 255;
        finalPix[i*4+2] = 255;
        finalPix[i*4+3] = val;
    }
    ofSaveImage(finalPix, filename);
    
    warningMessage = "Masque SAM sauvegarde : " + filename;
    warningEndTime = ofGetElapsedTimef() + 3.0f;
    ofLogNotice("Scene2D_SIDE") << "Masque SAM interactif sauvegarde : " << filename;
}

void Scene2D_SIDE::runSamInference() {
    if (!overlayImg.isAllocated() || samPoints.empty()) {
        samPreviewMask.clear();
        bSamMaskGenerated = false;
        return;
    }

    string modelPath = ofToDataPath("models/SAM/image_segmentation_efficientsam_ti_2025april_int8.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("Scene2D_SIDE") << "Modele IA introuvable : " << modelPath;
        warningMessage = "Modele SAM introuvable !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = overlayImg.getWidth();
        int h = overlayImg.getHeight();

        ofPixels rgbPixels = overlayImg.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR);
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        cv::Mat image_blob = cv::dnn::blobFromImage(cvImg, 1.0 / 255.0, cv::Size(1024, 1024), cv::Scalar(0, 0, 0), false, false);
        
        const int max_points = 6;
        int shape_pts[4] = {1, 1, max_points, 2};
        cv::Mat points_blob(4, shape_pts, CV_32F, cv::Scalar(0));
        
        int shape_lbls[4] = {1, 1, max_points, 1};
        cv::Mat labels_blob(4, shape_lbls, CV_32F, cv::Scalar(-1));
        
        float* points_ptr = points_blob.ptr<float>();
        float* labels_ptr = labels_blob.ptr<float>();

        vector<ofVec2f> backgroundPoints;
        int fg_point_idx = 0;
        for (size_t i = 0; i < samPoints.size(); ++i) {
            if (samLabels[i] == 0) { // Background point
                backgroundPoints.push_back(samPoints[i]);
            } else { // Foreground or box point
                if (fg_point_idx < max_points) {
                    float scaled_x = samPoints[i].x * 1024.0f / w;
                    float scaled_y = samPoints[i].y * 1024.0f / h;
                    points_ptr[fg_point_idx * 2 + 0] = scaled_x;
                    points_ptr[fg_point_idx * 2 + 1] = scaled_y;
                    labels_ptr[fg_point_idx] = (float)samLabels[i];
                    fg_point_idx++;
                }
            }
        }

        if (fg_point_idx == 0) {
            samPreviewMask.clear();
            bSamMaskGenerated = false;
            return;
        }

        net.setInput(image_blob, "batched_images");
        net.setInput(points_blob, "batched_point_coords");
        net.setInput(labels_blob, "batched_point_labels");

        std::vector<cv::String> outNames = {"output_masks", "iou_predictions"};
        std::vector<cv::Mat> outputs;
        net.forward(outputs, outNames);

        if(outputs.size() < 2 || outputs[0].empty() || outputs[1].empty()) {
            ofLogError("Scene2D_SIDE") << "Erreur: Le modele SAM n'a pas retourne les masques attendus.";
            return;
        }

        cv::Mat outputBlob = outputs[0];
        cv::Mat outputIou = outputs[1];

        vector<pair<float, int>> sorted_ious;
        const float* iou_ptr = outputIou.ptr<float>();
        for(int i = 0; i < outputIou.total(); ++i) {
            sorted_ious.push_back({iou_ptr[i], i});
        }
        std::sort(sorted_ious.rbegin(), sorted_ious.rend());

        int outH = outputBlob.size[outputBlob.dims - 2];
        int outW = outputBlob.size[outputBlob.dims - 1];
        cv::Mat bestMask;

        for (const auto& iou_pair : sorted_ious) {
            int mask_idx = iou_pair.second;
            const float* mask_ptr = outputBlob.ptr<float>() + mask_idx * (outH * outW);
            cv::Mat maskMap(outH, outW, CV_32F, (void*)mask_ptr);
            
            cv::Mat mask8U;
            cv::threshold(maskMap, mask8U, 0.0, 255.0, cv::THRESH_BINARY);
            mask8U.convertTo(mask8U, CV_8U);
            cv::resize(mask8U, mask8U, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);

            bool contains_bg = false;
            for (const auto& bg_pt : backgroundPoints) {
                if (mask8U.at<unsigned char>((int)bg_pt.y, (int)bg_pt.x) > 0) {
                    contains_bg = true;
                    break;
                }
            }

            if (!contains_bg) {
                bestMask = mask8U;
                break;
            }
        }

        if (bestMask.empty() && !sorted_ious.empty()) {
            int best_mask_idx = sorted_ious[0].second;
            const float* mask_ptr = outputBlob.ptr<float>() + best_mask_idx * (outH * outW);
            cv::Mat maskMap(outH, outW, CV_32F, (void*)mask_ptr);
            cv::threshold(maskMap, bestMask, 0.0, 255.0, cv::THRESH_BINARY);
            bestMask.convertTo(bestMask, CV_8U);
            cv::resize(bestMask, bestMask, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);
        }

        if (!bestMask.empty()) {
            samPreviewMask.getPixels().setFromExternalPixels(bestMask.data, w, h, 1);
            samPreviewMask.update();
            bSamMaskGenerated = true;
        }

    } catch(const cv::Exception& e) {
        ofLogError("Scene2D_SIDE") << "Erreur inference SAM DNN : " << e.what();
        warningMessage = "Erreur IA (SAM DNN)";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}