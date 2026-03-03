#include "Scene2DZenit.h"

//--------------------------------------------------------------
void Scene2DZenit::setup() {
    // 1. Chargement Images & Allocations FBO (Inchangé)
    imgFront.load("GAB/FRONT.jpg"); imgBack.load("GAB/BACK.jpg");
    imgJar.load("GAB/JAR.jpg");     imgCour.load("GAB/COUR.jpg");
    imgSol.load("GAB/SOL.jpg");     imgTopJar.load("GAB/TOP_JAR.jpg");
    imgTopCour.load("GAB/TOP_COUR.jpg");

    // Optimisation : Redimensionner les images à la taille des FBO
    if(imgFront.isAllocated()) imgFront.resize(wFront, hFront);
    if(imgBack.isAllocated()) imgBack.resize(wBack, hBack);
    if(imgJar.isAllocated()) imgJar.resize(wJar, hJar);
    if(imgCour.isAllocated()) imgCour.resize(wCour, hCour);
    if(imgSol.isAllocated()) imgSol.resize(wSol, hSol);
    if(imgTopJar.isAllocated()) imgTopJar.resize(wTopJar, hTopJar);
    if(imgTopCour.isAllocated()) imgTopCour.resize(wTopCour, hTopCour);

    fboFront.allocate(wFront, hFront, GL_RGBA);
    fboBack.allocate(wBack, hBack, GL_RGBA);
    fboJar.allocate(wJar, hJar, GL_RGBA);
    fboCour.allocate(wCour, hCour, GL_RGBA);
    fboSol.allocate(wSol, hSol, GL_RGBA);
    fboTopJar.allocate(wTopJar, hTopJar, GL_RGBA);
    fboTopCour.allocate(wTopCour, hTopCour, GL_RGBA);

    viewZoom = 0.12f; 
    viewPan.set(ofGetWidth()/2, ofGetHeight()/2);

    // 2. Waypoints Balle (Inchangé)
    ofVec2f pSol(wSol/2, hSol/2);
    ofVec2f pFront(wFront/2, -hFront/2);
    ofVec2f pBack(wBack/2, hSol + gapBack + hBack/2); 
    ofVec2f pJar(-hJar/2, hSol - 400); 
    ofVec2f pTopJar(-hJar - hTopJar/2, hSol - 400);
    ofVec2f pCour(wSol + hCour/2, 400);
    ofVec2f pTopCour(wSol + hCour + hTopCour/2, 400);

    waypoints = { pSol, pJar, pTopJar, pJar, pSol, pFront, pSol, pCour, pTopCour, pCour, pSol, pBack, pSol };

    // --- 3. INIT FISH LAYER SOL ---
    // On crée un collider aux dimensions exactes du SOL
    colliderSol = make_shared<ColliderLayer>();
    colliderSol->setup(wSol, hSol, 1.0f); // Scale 1.0 car on est en taille réelle

    // On configure les poissons
    fishSol.setup(wSol, hSol, colliderSol);

    // On ajoute quelques sardines pour voir quelque chose tout de suite
    for(int i=0; i<50; i++) {
        fishSol.addSardine(ofRandom(wSol), ofRandom(hSol));
    }
    // Et un requin pour l'ambiance
    fishSol.addShark(wSol/2, hSol/2);
}

//--------------------------------------------------------------
void Scene2DZenit::update() {
    if(!bEnabled) return; // <-- Coupe les calculs
    if(bPaused) return;

    // 1. Animation Balle (Inchangé)
    if (waypoints.size() > 1) {
        float totalDuration = 20.0f; 
        float cycleTime = fmod(localTime, totalDuration);
        float progress = cycleTime / totalDuration;
        
        float scaledProgress = progress * (waypoints.size() - 1);
        int idx = (int)scaledProgress; 
        int nextIdx = (idx + 1) % waypoints.size();
        
        ballPos = waypoints[idx].getInterpolated(waypoints[nextIdx], scaledProgress - idx);
    }

    // --- 2. UPDATE POISSONS ---
    fishSol.update();

    // 3. Captures FBO (Inchangé - utilise drawDynamicElements)
    ofMatrix4x4 mSol; 
    captureView(fboSol, imgSol, mSol);

    ofMatrix4x4 mFront; mFront.makeTranslationMatrix(0, -hFront, 0);
    captureView(fboFront, imgFront, mFront);

    ofMatrix4x4 mBack;
    mBack.makeIdentityMatrix(); mBack.glTranslate(0, hSol + gapBack, 0); mBack.glTranslate(wBack, hBack, 0); mBack.glRotate(180, 0, 0, 1);
    captureView(fboBack, imgBack, mBack);

    ofMatrix4x4 mJar;
    mJar.makeIdentityMatrix(); mJar.glTranslate(0, hSol + gapBack, 0); mJar.glRotate(-90, 0, 0, 1); mJar.glTranslate(0, -hJar, 0);
    captureView(fboJar, imgJar, mJar);

    ofMatrix4x4 mTopJar;
    mTopJar.makeIdentityMatrix(); mTopJar.glTranslate(0, hSol + gapBack, 0); mTopJar.glRotate(-90, 0, 0, 1); mTopJar.glTranslate(0, -hJar - hTopJar, 0);
    captureView(fboTopJar, imgTopJar, mTopJar);

    ofMatrix4x4 mCour;
    mCour.makeIdentityMatrix(); mCour.glTranslate(wSol, 0, 0); mCour.glRotate(90, 0, 0, 1); mCour.glTranslate(0, -hCour, 0);
    captureView(fboCour, imgCour, mCour);

    ofMatrix4x4 mTopCour;
    mTopCour.makeIdentityMatrix(); mTopCour.glTranslate(wSol, 0, 0); mTopCour.glRotate(90, 0, 0, 1); mTopCour.glTranslate(0, -hCour - hTopCour, 0);
    captureView(fboTopCour, imgTopCour, mTopCour);
}

//--------------------------------------------------------------
void Scene2DZenit::drawDynamicElements() {
    // Dessin dans le repère GLOBAL (0,0 = Coin Haut-Gauche du SOL)
    
    // 1. Dessin de la Balle
     bool lastDebug = false;
    if (lastDebug) {
    ofSetColor(0, 100, 255);
    ofDrawCircle(ballPos.x, ballPos.y, 80);
    ofSetColor(255); // Reset couleur
    }
    fishSol.draw();
}

//--------------------------------------------------------------
void Scene2DZenit::captureView(ofFbo& fbo, ofImage& img, ofMatrix4x4 globalTransform) {
    fbo.begin();
    ofClear(0, 0, 0, 0);
    
    if(bShowImages && img.isAllocated()) {
          ofSetColor(255, 255, 255, 180); // 80 = Transparence
        img.draw(0, 0, fbo.getWidth(), fbo.getHeight());
    } else {
        bool lastDebug = false;
        if (lastDebug) {
            ofNoFill(); ofSetColor(50); ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight()); ofFill();
        }
    }

            ofSetColor(255);

    ofPushMatrix();
    ofMultMatrix(globalTransform.getInverse());
    drawDynamicElements(); 
    ofPopMatrix();
    
    fbo.end();
}

// ... Le reste (draw, inputs) reste identique au fichier précédent ...
void Scene2DZenit::draw() {
    if(!bEnabled) {
        ofBackground(0);
        return;
    }

    ofBackground(0);
    ofPushMatrix();
    ofTranslate(viewPan);
    ofScale(viewZoom);
    ofTranslate(-wSol/2, -hSol/2); 

    // Rendu global pour debug
    ofSetColor(255); fboSol.draw(0,0); 
    fboFront.draw(0, -hFront);
    
    ofPushMatrix(); ofTranslate(0, hSol + gapBack); ofRotateZDeg(-90);
    fboJar.draw(0, -hJar); fboTopJar.draw(0, -hJar - hTopJar); ofPopMatrix();
    
    ofPushMatrix(); ofTranslate(wSol, 0); ofRotateZDeg(90);
    fboCour.draw(0, -hCour); fboTopCour.draw(0, -hCour - hTopCour); ofPopMatrix();
    
    ofPushMatrix(); ofTranslate(0, hSol + gapBack); ofTranslate(wBack, hBack); ofRotateZDeg(180);
    fboBack.draw(0, 0); ofPopMatrix();

    drawDynamicElements(); // Balle + Poissons

    ofPopMatrix();
}
// ... Stubs inputs ...
void Scene2DZenit::drawLabel(string text, float x, float y) {
    if(!bShowImages) { 
        ofPushStyle(); ofSetColor(255, 255, 0); ofNoFill();
        ofDrawCircle(x, y, 20); ofDrawBitmapStringHighlight(text, x + 30, y); ofPopStyle();
    }
}
void Scene2DZenit::keyPressed(int key) { 
    if(key == 'g' || key == 'G') bShowImages = !bShowImages; 
    if(key == 'r' || key == 'R') {
        viewZoom = 0.12f; 
        viewPan.set(ofGetWidth()/2, ofGetHeight()/2);
    }
}


void Scene2DZenit::mouseScrolled(int x, int y, float sx, float sy) {
    float oldZoom = viewZoom; viewZoom = ofClamp(viewZoom + sy * 0.01, 0.01, 2.0);
    viewPan -= (ofVec2f(x, y) - viewPan) * (viewZoom / oldZoom - 1);
}
void Scene2DZenit::mousePressed(int x, int y, int button) { lastMouse.set(x, y); }
void Scene2DZenit::mouseDragged(int x, int y, int button) { viewPan += (ofVec2f(x, y) - lastMouse); lastMouse.set(x, y); }