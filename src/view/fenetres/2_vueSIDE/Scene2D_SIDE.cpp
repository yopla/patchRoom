#include "Scene2D_SIDE.h"
#include "Scene2DLayerManager.h"

//--------------------------------------------------------------
void Scene2D_SIDE::setup() {
    localTime = 0.0f;
    
    // Chargement des images de fond
    imgJar.load("JAR.jpg"); imgFront.load("FRONT.jpg"); imgCour.load("COUR.jpg"); imgBack.load("BACK.jpg");
    imgSol.load("SOL.jpg"); imgTopJar.load("TOP_JAR.jpg"); imgTopCour.load("TOP_COUR.jpg");

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
    viewZoom = (float)ofGetWidth() / totalSceneWidth * 0.5f;
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

    // 2. Gestion du Temps Local (Pour reprendre l'animation exactement où elle était)
    float fpsRec = 60.0f;
    localTime += 1.0f / fpsRec;

    ofVec2f m = getTransformedMouse();

    layerManager.update(m);

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
    captureSection(fboFront, srcX_Front, 0, imgFront, true);
    captureSection(fboBack,  srcX_Back,  0, imgBack,  true);
    captureSection(fboJar,   srcX_Jar, 688, imgJar, true);
    captureSection(fboCour,  srcX_Cour, 400, imgCour, true);

    // TOP JAR
    captureSection(fboSol, srcX_Front, 1472, imgSol, true);
    captureSection(fboTopJar, srcX_Jar, -912, imgTopJar, true);
    captureSection(fboTopCour, srcX_Cour, -608, imgTopCour, true);
}

//--------------------------------------------------------------
void Scene2D_SIDE::drawDynamicElements() {
    ofVec2f m = getTransformedMouse(); 

    layerManager.draw(m);

    // 6. BALLE (Toujours visible)
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofDrawCircle(ballPos.x, ballPos.y, 80); 
    ofPopStyle();

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
    ofPopMatrix();

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

    ofDrawBitmapStringHighlight(stats, 20, 30); 
}


//--------------------------------------------------------------
void Scene2D_SIDE::captureSection(ofFbo& targetFbo, float worldX, float worldTopY, ofImage& img, bool bDrawDynamics) {
    targetFbo.begin();
        ofClear(0, 0, 0, 0);
        
        if (bShowTextures && img.isAllocated()) {
            ofSetColor(255, 255, 255, 180);
            img.draw(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
        } else {
            ofNoFill(); ofSetColor(100);
            ofDrawRectangle(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
            ofFill();
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
    if (key == 'g' || key == 'G') bShowTextures = !bShowTextures;
    
    if (key == 'r' || key == 'R') {
        viewZoom = (float)ofGetWidth() / totalSceneWidth * 0.5f;
        viewPan.x = (ofGetWidth() - totalSceneWidth * viewZoom) / 2.0f;
        viewPan.y = (ofGetHeight() - hMax * viewZoom) / 2.0f;
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