#include "RoomApp.h"

//--------------------------------------------------------------
void RoomApp::setup(){
    ripples.setup(10); // <--- AJOUT : On crée 10 projecteurs simultanés

    ofSetVerticalSync(true);
    ofEnableDepthTest();
    ofDisableArbTex();
    cursorSquare.setup(); // <--- AJOUT
    // 1. Initialisation des modules
    walls.setup();
    projection.setup();
    atmosphere.setup();

    // 2. Initialisation du nouveau module Poster
    // On passe les dimensions de la pièce pour le calcul du périmètre
    poster.setup(roomWidth, roomDepth, heightFrontBack);

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
    if(dragInfo.files.size() > 0){
        // On récupère le chemin du premier fichier déposé
        string file = dragInfo.files[0];
        
        // On l'envoie au système d'atmosphère
        atmosphere.loadTexture(file);
    }
}

//--------------------------------------------------------------
void RoomApp::update(){
    if(!bEnabled) return; // <-- Coupe les calculs si désactivé
    
    float fpsRec = 60.0f;
    float time = ofGetFrameNum() / fpsRec;

    // Animation légère de la position du rig (caméras Off-Axis)
    if (respire) rigPosition.y = 600 + sin(time*0.5)*100;
    
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

    // Mise à jour de la logique des modules
    poster.update();
    projection.update();
    ripples.update(walls); // <--- AJOUT : Update des ripples avec référence aux murs
}

//--------------------------------------------------------------
void RoomApp::drawSceneContent(bool showAtmosphere) {
    ofEnableDepthTest();

    // 1. Dessiner l'Ambiance (Disco Ball ou Sphère Damier)
    if (showAtmosphere) {
        atmosphere.draw(bUseTexture);
    }

    // 2. Dessiner le Plan Collé (Rush A)
    projection.drawPlanColle();

    // 3. Dessiner la Géométrie des Murs
    if(bDrawWalls) {
        walls.draw(bShowRoof, wallAlpha);
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

    // ---------------------------------------------------------
    // GESTION DES INPUTS (SHIFT GAUCHE vs DROIT)
    // ---------------------------------------------------------
    bool isLeftShift  = ofGetKeyPressed(OF_KEY_LEFT_SHIFT);
    bool isRightShift = ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);

    // 1. Gestion de la Caméra (EasyCam)
    // Si Shift Gauche est pressé -> On gèle la caméra
    // Sinon (Rien ou Shift Droit) -> La caméra bouge normalement
    if(isLeftShift) {
        camGlobal.disableMouseInput();
    } else {
        camGlobal.enableMouseInput();
    }

    camGlobal.begin(); 
        // Détection survol souris pour le plan collé
        cursorSquare.updateRaycast(camGlobal, walls);
        projection.checkMouseIntersection(camGlobal);

        // 2. Gestion du Projecteur
        // Le projecteur ne bouge que si l'un des Shift est pressé
        if(ofGetMousePressed(0)) { 
            if (isLeftShift || isRightShift) {
                projection.updateTarget(camGlobal, walls);
            }
        }

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
    ofDrawBitmapString("MURS [G]: " + ofToString(bDrawWalls), 20, 20);
    ofDrawBitmapString("ATMO [B]: " + ofToString(bDrawAtmosphere), 20, 40); 
    
    // Petit feedback visuel pour savoir quel mode est actif
    if(isLeftShift) ofDrawBitmapStringHighlight("MODE: PROJECTEUR SEUL (Camera Lock)", 20, 60, ofColor(255, 0, 0), ofColor(255));
    else if(isRightShift) ofDrawBitmapStringHighlight("MODE: PROJECTEUR + CAMERA", 20, 60, ofColor(0, 255, 0), ofColor(0));
}

//--------------------------------------------------------------
void RoomApp::keyPressed(int key){
    if(key == 'f' || key == 'F') bDrawWalls = !bDrawWalls;
    if(key == 'g' || key == 'G') {
        if(wallAlpha == 100.0f) {
            wallAlpha = 10.0f;
        } else {
            wallAlpha = 100.0f;
        }
    }
    
   if(key == 'b' || key == 'B') bDrawAtmosphere = !bDrawAtmosphere;
    if(key == 'l' || key == 'L') bUseTexture = !bUseTexture;
    
    if(key == 'r' || key == 'R') {
        camGlobal.setDistance(4000);
        camGlobal.setPosition(2000, 2500, 3000);
        camGlobal.lookAt(ofVec3f(0, 600, 0));
    }
    if(key == 'a' || key == 'A') bShowRoof = !bShowRoof;
    if(key == 'u' || key == 'U') respire = !respire;
if(key == 'k' || key == 'K') bDrawRipples = !bDrawRipples;

    // Délégation des touches aux modules
    projection.keyPressed(key);
    atmosphere.keyPressed(key);
}