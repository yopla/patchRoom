#include "Scene2D_SIDE.h"
#include "Scene2DLayerManager.h"
#include "ofApp.h"

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
    

    ofPopMatrix();

    if (layerManager.bDrawPuyo) {
        int folded = 0;
        int complete = 0;
        layerManager.puyoLayer.getStats(folded, complete);
        ofDrawBitmapStringHighlight("Puyos: " + ofToString(complete) + " OK / " + ofToString(folded) + " Folded", 20, ofGetHeight() - 50);
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
    
    if(!isSpacePressed) {
        ofVec2f m = getTransformedMouse();
        layerManager.mousePressed(m, button);
    }
}

void Scene2D_SIDE::mouseReleased(int x, int y, int button) {
    ofVec2f m = getTransformedMouse();
    layerManager.mouseReleased(m, button);
}

void Scene2D_SIDE::mouseDragged(int x, int y, int button) {
    if (isSpacePressed) {
        viewPan += (ofVec2f(x, y) - lastMouse);
    }
    lastMouse.set(x, y);
}

void Scene2D_SIDE::keyPressed(int key) {
    if (key == ' ') isSpacePressed = true; 
    if (key == 'g' || key == 'G') {
        bgDisplayMode = (bgDisplayMode + 1) % 4;
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
        ofImage fullImg;
        if (fullImg.load(file)) {
            // On vérifie que l'image est bien celle d'un export complet (ou au moins assez grande)
            if (fullImg.getWidth() >= totalSceneWidth && fullImg.getHeight() >= 4752) {
                ofLogNotice("Scene2D_SIDE") << "Chargement de la nouvelle image de fond (GAB) : " << file;
                
                ofPixels& pix = fullImg.getPixels();
                ofPixels pFront, pBack, pJar, pCour, pSol, pTopJar, pTopCour;
                
                // Découpage selon les offsets de l'export (Y a été décalé de +912 à l'export)
                pix.cropTo(pFront, srcX_Front, 912, wFront, 1472);
                imgFront.setFromPixels(pFront);

                pix.cropTo(pBack, srcX_Back, 912, wFront, 1472);
                imgBack.setFromPixels(pBack);

                pix.cropTo(pJar, srcX_Jar, 1600, wJar, 784);
                imgJar.setFromPixels(pJar);

                pix.cropTo(pCour, srcX_Cour, 1312, wJar, 1072);
                imgCour.setFromPixels(pCour);

                pix.cropTo(pSol, srcX_Front, 2384, wSol, hSol);
                imgSol.setFromPixels(pSol);

                pix.cropTo(pTopJar, srcX_Jar, 0, wTopJar, hTopJar);
                imgTopJar.setFromPixels(pTopJar);

                pix.cropTo(pTopCour, srcX_Cour, 304, wTopCour, hTopCour);
                imgTopCour.setFromPixels(pTopCour);
                
                bgDisplayMode = 0; // On force le mode 0 pour bien afficher ces images jpg/png
            } else {
                ofLogWarning("Scene2D_SIDE") << "L'image glissée est trop petite ! Dimension minimale attendue : " << totalSceneWidth << "x4752";
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