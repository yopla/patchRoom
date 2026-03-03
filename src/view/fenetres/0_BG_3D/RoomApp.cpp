#include "RoomApp.h"

//--------------------------------------------------------------
void RoomApp::setup(){
    inputHandler.setup(this);
    ripples.setup(10); // <--- AJOUT : On crée 10 projecteurs simultanés

    ofSetVerticalSync(true);
    ofEnableDepthTest();
    ofDisableArbTex();
    cursorSquare.setup(); // <--- AJOUT
    // 1. Initialisation des modules
    walls.setup();
    projection.setup();
    atmosphere.setup();
    wormSystem.setup(50); // On crée 50 worms
    wingedWormSystem.setup(20); // On crée 20 vers volants

    // 2. Initialisation du nouveau module Poster
    // On passe les dimensions de la pièce pour le calcul du périmètre
    poster.setup(roomWidth, roomDepth, heightFrontBack);

    // Initialisation de l'anneau fluide
    // Rayon agrandi pour être plus grand que la salle (2500 > 1200 demi-largeur)
    fluidRing.setup(2500.0f, heightFrontBack+1000, 1200.0f);

    // Initialisation du LightFlyRing (Un peu plus grand que le FluidRing)
    // Rayon 2600
    lightFlyRing.setup(1600.0f, heightFrontBack+500, 800.0f);

    // 3. Allocation des FBOs (Sorties visuelles)
    fboFront.allocate(roomWidth, heightFrontBack, GL_RGB);
    fboBack.allocate(roomWidth, heightFrontBack, GL_RGB);
    fboCour.allocate(roomDepth, heightCour, GL_RGB);
    fboJar.allocate(roomDepth, heightJar, GL_RGB);
    fboSol.allocate(roomWidth, roomSolDepth, GL_RGB);
    fboTopCour.allocate(roomDepth, heightTopCour, GL_RGB);
    fboTopJar.allocate(roomDepth, heightTopJar, GL_RGB);
    
    // 4. Configuration de la caméra globale (Preview)
    camGlobal.setDistance(4000);
    camGlobal.setPosition(2000, 2500, 3000);
    camGlobal.lookAt(ofVec3f(0, 600, 0));
    
    rigPosition.set(0, 600, 0);
}

void RoomApp::dragEvent(ofDragInfo dragInfo){
    inputHandler.dragEvent(dragInfo);
}

void RoomApp::keyReleased(int key) {
    inputHandler.keyReleased(key);
}

void RoomApp::mouseMoved(int x, int y) {
}

void RoomApp::mouseDragged(int x, int y, int button) {
}

void RoomApp::mousePressed(int x, int y, int button) {
}

void RoomApp::mouseReleased(int x, int y, int button) {
}

void RoomApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void RoomApp::update(){
    if(!bEnabled) return; // <-- Coupe les calculs si désactivé
    if(bPaused) return;   // <-- Pause générale

    // Animation légère de la position du rig (caméras Off-Axis)
    if (respire) rigPosition.y = 600 + sin(localTime*0.5)*100;
    
    // Mise à jour des caméras Off-Axis (en utilisant les points géométriques de "walls")
    camFront.setPosition(rigPosition); camFront.setupOffAxisViewPortal(walls.pFront[0], walls.pFront[1], walls.pFront[2]);
    camBack.setPosition(rigPosition);  camBack.setupOffAxisViewPortal(walls.pBack[0], walls.pBack[1], walls.pBack[2]);
    camJar.setPosition(rigPosition);   camJar.setupOffAxisViewPortal(walls.pJar[0], walls.pJar[1], walls.pJar[2]);
    camCour.setPosition(rigPosition);  camCour.setupOffAxisViewPortal(walls.pCour[0], walls.pCour[1], walls.pCour[2]);
    camSol.setPosition(rigPosition);   camSol.setupOffAxisViewPortal(walls.pSolTL, walls.pSolBL, walls.pSolBR);
    
    if(bShowRoof) {
        camTopJar.setPosition(rigPosition);  camTopJar.setupOffAxisViewPortal(walls.pTopJarTL, walls.pTopJarBL, walls.pTopJarBR);
        camTopCour.setPosition(rigPosition); camTopCour.setupOffAxisViewPortal(walls.pTopCourTL, walls.pTopCourBL, walls.pTopCourBR);
    }

    // Mise à jour des interactions basées sur la caméra (avant le reste)
    // pour que l'état de survol soit à jour pour la logique OSC
    inputHandler.update();

    // Mise à jour de la logique des modules
    poster.update();
    projection.update();
    atmosphere.update(localTime); // <--- Pass localTime
    if (bDrawWorms) {
        wormSystem.update(walls);
    }

    if (bDrawWingedWorms) {
        wingedWormSystem.update(localTime); // <--- Pass localTime
    }

    ripples.update(walls); // <--- AJOUT : Update des ripples avec référence aux murs

    if(bLightFlyRingEnabled) {
        lightFlyRing.update(localTime); // <--- Pass localTime
    }
}

//--------------------------------------------------------------
void RoomApp::drawSceneContent(bool showAtmosphere) {
    ofEnableDepthTest();

    // 1. Dessiner l'Ambiance (Disco Ball ou Sphère Damier)
    if (showAtmosphere) {
        atmosphere.draw(bUseTexture);
    }
    
    // 2. Dessin de l'anneau fluide (Devant le LightFlyRing)
    if (bFluidRingEnabled) {
        fluidRing.draw();
    }

    // 1. Dessin du LightFlyRing (Le plus grand rayon = Arrière plan)
    // On le dessine en premier. Grâce au glDepthMask(GL_FALSE) dans sa méthode draw,
    // il ne bloquera pas le rendu des objets devant lui.
    if (bLightFlyRingEnabled) {
        lightFlyRing.draw();
    }

    // 2. Dessiner le Plan Collé (Rush A)
    projection.drawPlanColle();

    if (bDrawWingedWorms) {
        wingedWormSystem.draw();
    }

    // 3. Dessiner la Géométrie des Murs
    if(bDrawGab) {
        walls.draw(bShowRoof, wallAlpha);
    }

    if (bDrawWorms) {
        wormSystem.draw(walls);
    }

    if(bDrawRipples) {
        ripples.draw(walls);
    }

    bool posterOk = false;
    if (posterOk) poster.draw(roomWidth, roomDepth); 
    // ------------------

    cursorSquare.drawProjected(walls);

    // 4. Dessiner la Projection (Beam) sur la géométrie
    if(bDrawBeam) {
        projection.drawBeamProjection(walls, bShowRoof);
    }

    ofDisableDepthTest();
}
void RoomApp::draw(){
    if(!bEnabled) {
        ofBackground(0); // Affiche du noir dans la fenêtre dédiée
        return;          // Coupe le rendu
    }

    ofBackground(0);
    
    // --- RENDU DANS LES FBOS ---
    fboFront.begin();   ofClear(0, 255); camFront.begin();   drawSceneContent(); camFront.end();   fboFront.end();
    fboBack.begin();    ofClear(0, 255); camBack.begin();    drawSceneContent(); camBack.end();    fboBack.end();
    fboJar.begin();     ofClear(0, 255); camJar.begin();     drawSceneContent(); camJar.end();     fboJar.end();
    fboCour.begin();    ofClear(0, 255); camCour.begin();    drawSceneContent(); camCour.end();    fboCour.end();
    fboSol.begin();     ofClear(0, 255); camSol.begin();     drawSceneContent(); camSol.end();     fboSol.end();
    
    if(bShowRoof){
        fboTopJar.begin();  ofClear(0, 255); camTopJar.begin();  drawSceneContent(); camTopJar.end();  fboTopJar.end();
        fboTopCour.begin(); ofClear(0, 255); camTopCour.begin(); drawSceneContent(); camTopCour.end(); fboTopCour.end();
    }

    // --- PREVIEW GLOBALE ---
    ofViewport(0, 0, ofGetWidth(), ofGetHeight());

    camGlobal.begin(); 
        drawSceneContent(bDrawAtmosphere); 
        
        // Debug : Visualisation du projecteur
       if(bDrawBeam) { 
        bool debugBeam = false;
           if (debugBeam) projection.drawProjectorDebug(walls);
        }
        // Point cyan pour visualiser le Rig
        ofSetColor(0, 255, 255); ofDrawSphere(rigPosition, 30);
    camGlobal.end();
    
    // UI de Debug
    ofSetColor(255);
    ofDrawBitmapString("GAB [G] Alpha: " + ofToString(wallAlpha, 1), 20, 20);
    ofDrawBitmapString("BEAM [F]: " + ofToString(bDrawBeam), 20, 35);
    ofDrawBitmapString("WORMS [Y]: " + ofToString(bDrawWorms), 20, 50);
    ofDrawBitmapString("WINGED [W]: " + ofToString(bDrawWingedWorms), 20, 50);
    ofDrawBitmapString("ATMO [B]: " + ofToString(bDrawAtmosphere), 20, 65); 
    ofDrawBitmapString("LIGHT FLY [H]: " + ofToString(bLightFlyRingEnabled), 20, 80);
    
    if(cursorSquare.isVisible) {
        ofDrawBitmapString("CURSOR 3D: " + ofToString(cursorSquare.getCurrentPos()), 20, 145);
    }
    
    if(bLightFlyRingEnabled) {
        bool bDrawCoordDebug = false;
        if (bDrawCoordDebug) {
            ofDrawBitmapString("HALO CURSOR 3D: " + ofToString(inputHandler.cursor3DPos), 20, 160);
            ofDrawBitmapString("LAST HALO POS: " + ofToString(inputHandler.lastCreatedHalo3DPos), 20, 175);
        }
    }

    // Petit feedback visuel pour savoir quel mode est actif
    bool isLeftShift  = ofGetKeyPressed(OF_KEY_LEFT_SHIFT);
    bool isRightShift = ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);
    bool isSpacePressed = ofGetKeyPressed(' ');
    bool isTabPressed = ofGetKeyPressed(OF_KEY_TAB);

    if(isLeftShift) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 1 SEUL (Camera Lock)", 20, 85, ofColor::red, ofColor::white);
    else if(isRightShift) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 1 + CAMERA", 20, 85, ofColor::green, ofColor::black);

    if(isSpacePressed) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 2 SEUL (Camera Lock)", 20, 105, ofColor::blue, ofColor::white);
    if(isTabPressed) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 3 SEUL (Camera Lock)", 20, 125, ofColor::magenta, ofColor::white);
}

//--------------------------------------------------------------
void RoomApp::keyPressed(int key){
    inputHandler.keyPressed(key);
}