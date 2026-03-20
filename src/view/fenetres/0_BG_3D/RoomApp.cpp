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

    // Initialisation du CloudRing (Maintenant une SPHERE complète)
    // Centrée sur le Rig (0, 600, 0), avec un grand rayon (3000) pour englober la pièce
    cloudRing.setup(3000.0f, ofVec3f(0, 600, 0));

    // Initialisation du LiquidSphereRing
    liquidSphereRing.setup(3000.0f, ofVec3f(0, 600, 0));

    // Initialisation du JellySphereRing
    jellySphereRing.setup(3000.0f, ofVec3f(0, 600, 0));

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

    // Initialisation du sol ondulant (Largeur pièce, Z début, Z fin)
    undulatingFloor.setup(60000.0f, -30000.0f, 30000.0f);
    
    // Initialisation du Kraken
    kraken.setup(); // <--- AJOUT
    externalKraken.setup(); // <--- AJOUT

    // Initialisation du lecteur vidéo 360
    scene360VideoPlayer.setup(&atmosphere, "_scene");
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
    inputHandler.mouseDragged(x, y, button);
}

void RoomApp::mousePressed(int x, int y, int button) {
    inputHandler.mousePressed(x, y, button);
}

void RoomApp::mouseReleased(int x, int y, int button) {
    inputHandler.mouseReleased(x, y, button);
}

void RoomApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void RoomApp::update(){
    if(!bEnabled) return; // <-- Coupe les calculs si désactivé
    if(bPaused) return;   // <-- Pause générale

    // --- FORCER LA ROTATION X/Y (ARCBALL) SUR TOUT L'ÉCRAN ---
    // On agrandit la "zone de contrôle" de ofEasyCam bien au-delà de l'écran 
    // pour que son cercle virtuel de détection englobe les coins de ta fenêtre.
    int w = ofGetWidth();
    int h = ofGetHeight();
    if (w > 0 && h > 0) {
        float maxDim = std::max(w, h);
        float giantSize = maxDim * 3.0f; // 3x la taille de l'écran
        
        // On centre cette zone géante
        camGlobal.setControlArea(ofRectangle(w/2.0f - giantSize/2.0f, h/2.0f - giantSize/2.0f, giantSize, giantSize));
        
        // On compense mathématiquement pour que la vitesse de Pan/Zoom/Orbite reste identique
        // Ajout d'un signe moins (-) sur le premier paramètre pour inverser l'axe X
        camGlobal.setRotationSensitivity((giantSize / std::min(w, h)), giantSize / std::min(w, h), 0.0f);
        camGlobal.setTranslationSensitivity(giantSize / (float)w, giantSize / (float)h, giantSize / (float)h);
    }

    // Animation légère de la position du rig (caméras Off-Axis)
    if (respire) rigPosition.y = 600 + sin(localTime*0.5)*100;
    
    // Calcul de l'oscillation de la pièce (Effet Bateau sur l'eau)
    if (bOscillateRoom) {
        float t = localTime * 0.5f; 
        roomPosOffset.set(0, sin(t * 0.7f) * 100.0f, 0); // Heave (Montée/Descente)
        roomRotOffset.set(sin(t * 0.4f) * 2.0f, sin(t * 0.25f) * 1.0f, sin(t * 0.3f) * 1.5f); // Pitch, Yaw, Roll
    } else {
        roomPosOffset.set(0);
        roomRotOffset.set(0);
    }

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

    fluidRing.update();

    if(bLightFlyRingEnabled) {
        lightFlyRing.update(localTime); // <--- Pass localTime
    }

    if(bDrawUndulatingFloor) {
        undulatingFloor.update(localTime);
    }
    
    if(bDrawKraken) { // <--- AJOUT
        kraken.update(localTime);
    }
    
    if(bDrawExternalKraken) { // <--- AJOUT
        externalKraken.update(localTime);
    }

    // --- MISE A JOUR DU CLOUD RING ---
    if (bDrawCloudRing) {
        cloudRing.update(ofGetLastFrameTime());
    }

    // --- MISE A JOUR DU JELLY SPHERE ---
    if (bDrawJellySphere) {
        jellySphereRing.update(inputHandler.jellyLocalX, inputHandler.jellyLocalY);
    }

    // --- MISE A JOUR DU LECTEUR VIDEO 360 ---
    if (bDrawScene360Video) {
        scene360VideoPlayer.update();
    }
}

//--------------------------------------------------------------
void RoomApp::applyRoomTransform() {
    ofTranslate(roomPosOffset);
    ofRotateXDeg(roomRotOffset.x);
    ofRotateYDeg(roomRotOffset.y);
    ofRotateZDeg(roomRotOffset.z);
}

void RoomApp::applyInverseRoomTransform() {
    ofRotateZDeg(-roomRotOffset.z);
    ofRotateYDeg(-roomRotOffset.y);
    ofRotateXDeg(-roomRotOffset.x);
    ofTranslate(-roomPosOffset);
}

//--------------------------------------------------------------
void RoomApp::drawSceneContent(bool showAtmosphere, bool isGlobalView) {
    ofEnableDepthTest();

    // --- 0. DESSIN DE L'ATMOSPHERE (NON ATTACHÉ) ---
    // On le dessine en premier (Fond) et détaché de l'oscillation de la room
    if (showAtmosphere) {
        ofPushMatrix();
        if (!isGlobalView) {
            applyInverseRoomTransform();
        }
        atmosphere.draw(bUseTexture);
        ofPopMatrix();
    }

    // --- 1. TRANSFORMATION DE LA ROOM ---
    // Si on est en vue globale (Preview), la Room bouge et le sol est fixe.
    if (isGlobalView) {
        ofPushMatrix();
        applyRoomTransform();
    }
    // Si on est en vue locale (Projecteurs), la Room est fixe (attachée à la caméra) 
    // et le sol bougera inversement plus bas.

    // --- 2. DESSIN DU CONTENU ATTACHÉ À LA ROOM ---
    
    if (bFluidRingEnabled || fluidRing.globalAlpha > 0.0f) {
        fluidRing.draw();
    }

    if (bLightFlyRingEnabled) {
        lightFlyRing.draw();
    }

    projection.drawPlanColle();

    if (bDrawWingedWorms) {
        wingedWormSystem.draw();
    }

    // --- DESSIN DU CLOUD RING ---
    if (bDrawCloudRing) {
        cloudRing.draw();
    }

    // --- DESSIN DU LIQUID SPHERE ---
    if (bDrawLiquidSphere) {
        liquidSphereRing.draw();
    }

    // --- DESSIN DU JELLY SPHERE ---
    if (bDrawJellySphere) {
        jellySphereRing.draw();
    }

    if(bDrawGab) {
        walls.draw(bShowRoof, wallAlpha);
    }

    if (bDrawWorms) {
        wormSystem.draw(walls);
    }

    if(bDrawRipples) {
        ripples.draw(walls);
    }
    
    if(bDrawKraken) { // <--- AJOUT
        kraken.draw();
    }
    
    if(bDrawExternalKraken) { // <--- AJOUT
        externalKraken.draw();
    }

    bool posterOk = false;
    if (posterOk) poster.draw(roomWidth, roomDepth); 
    // ------------------

    cursorSquare.drawProjected(walls);

    if(bDrawBeam) {
        projection.drawBeamProjection(walls, bShowRoof);
    }

    // --- 3. FIN TRANSFORMATION ROOM ---
    if (isGlobalView) {
        ofPopMatrix();
    }

    // --- 4. DESSIN DU SOL ONDULANT (NON ATTACHÉ) ---
    // Le sol est dessiné à la fin (transparence)
    if(bDrawUndulatingFloor) {
        ofPushMatrix();
        if (!isGlobalView) {
            // En vue locale (depuis la room), le sol doit bouger à l'inverse de la room
            // pour donner l'illusion que c'est la room qui flotte.
            applyInverseRoomTransform();
        }
        undulatingFloor.draw();
        ofPopMatrix();
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
        drawSceneContent(bDrawAtmosphere, true); // TRUE = Vue Globale (Room bouge)
        
        // Debug : Visualisation du projecteur
       if(bDrawBeam) { 
        bool debugBeam = false;
           if (debugBeam) projection.drawProjectorDebug(walls);
        }
        // Point cyan pour visualiser le Rig (masqué pendant la touche L)
        if (!bLockCameraCenter) {
            ofSetColor(0, 255, 255); ofDrawSphere(rigPosition, 10);
        }
    camGlobal.end();
    
    // UI de Debug
    ofSetColor(255);
    ofDrawBitmapString("GAB [G] Alpha: " + ofToString(wallAlpha, 1), 20, 20);
    ofDrawBitmapString("BEAM [F]: " + ofToString(bDrawBeam), 20, 35);
    ofDrawBitmapString("WORMS [Y]: " + ofToString(bDrawWorms), 20, 50);
    ofDrawBitmapString("WINGED [W]: " + ofToString(bDrawWingedWorms), 20, 50);
    ofDrawBitmapString("ATMO [B]: " + ofToString(bDrawAtmosphere), 20, 65); 
    ofDrawBitmapString("LIGHT FLY [H]: " + ofToString(bLightFlyRingEnabled), 20, 80);
    ofDrawBitmapString("KRAKEN [3]: " + ofToString(bDrawKraken), 20, 95); // <--- AJOUT
    ofDrawBitmapString("EXT KRAKEN [4]: " + ofToString(bDrawExternalKraken), 20, 110); // <--- AJOUT
    ofDrawBitmapString("CLOUD RING [6]: " + ofToString(bDrawCloudRing), 20, 125); // <--- AJOUT
    ofDrawBitmapString("LIQUID SPHERE [7]: " + ofToString(bDrawLiquidSphere), 20, 140);
    ofDrawBitmapString("JELLY SPHERE [0]: " + ofToString(bDrawJellySphere), 20, 155); // <--- AJOUT
    ofDrawBitmapString("SCENE 360 [8]: " + ofToString(bDrawScene360Video), 20, 170); // <--- AJOUT
    
    if(cursorSquare.isVisible) {
        ofDrawBitmapString("CURSOR 3D: " + ofToString(cursorSquare.getCurrentPos()), 20, 200);
    }
    
    if(bLightFlyRingEnabled) {
        bool bDrawCoordDebug = true;// false;
        if (bDrawCoordDebug) {
            ofDrawBitmapString("HALO CURSOR 3D: " + ofToString(inputHandler.cursor3DPos), 20, 200);
            ofDrawBitmapString("LAST HALO POS: " + ofToString(inputHandler.lastCreatedHalo3DPos), 20, 215);
        }
    }

    // Petit feedback visuel pour savoir quel mode est actif
    bool isCommandPressed = ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_SUPER);
    bool isAngleBracketPressed = ofGetKeyPressed('<') || ofGetKeyPressed('>');
    bool isSpacePressed = ofGetKeyPressed(' ');
    bool isTabPressed = ofGetKeyPressed(OF_KEY_TAB);
    bool isShiftPressed = ofGetKeyPressed(OF_KEY_LEFT_SHIFT) || ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);
    bool isAltPressed = ofGetKeyPressed(OF_KEY_ALT);

    if(isCommandPressed) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 1 SEUL (Camera Lock)", 20, 85, ofColor::red, ofColor::white);
    if(isTabPressed) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 2 SEUL (Camera Lock)", 20, 105, ofColor::blue, ofColor::white);
    if(isAngleBracketPressed) ofDrawBitmapStringHighlight("MODE: PROJECTEUR 3 SEUL (< ou >)", 20, 125, ofColor::magenta, ofColor::white);
    
    if(isSpacePressed) ofDrawBitmapStringHighlight("MODE: CAMERA SEULE (Espace)", 20, 145, ofColor::orange, ofColor::black);
    else if(isShiftPressed) ofDrawBitmapStringHighlight("MODE: INTERACT SEUL (Shift)", 20, 145, ofColor::green, ofColor::black);
    else if(isAltPressed) ofDrawBitmapStringHighlight("MODE: EASYCAM NATIVE (Option)", 20, 145, ofColor::cyan, ofColor::black);
}

//--------------------------------------------------------------
void RoomApp::keyPressed(int key){
    inputHandler.keyPressed(key);
}

//--------------------------------------------------------------
void RoomApp::generateEquirectangularImage() {
    int w = 3840;
    int h = 2160;
    
    ofLogNotice("RoomApp") << "Debut generation image 360 (" << w << "x" << h << ")... Cela peut prendre quelques secondes.";
    
    ofPixels pixels;
    pixels.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
    
    // Centre de projection (Centre de la pièce / Rig)
    // On utilise 600 en Y car c'est la hauteur moyenne des yeux/caméras dans le setup
    ofVec3f center(0, 600, 0); 

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // 1. Coordonnées UV normalisées [0, 1]
            float u = (float)x / (float)w;
            float v = (float)y / (float)h;
            
            // 2. Conversion Equirectangulaire -> Sphérique
            // Longitude (theta) : -PI à PI (Horizontal)
            // Latitude (phi) : -PI/2 à PI/2 (Vertical)
            float theta = (u - 0.5f) * TWO_PI; 
            float phi = (0.5f - v) * PI;
            
            // 3. Vecteur Direction 3D
            // Note: Dans OF/OpenGL, Y est souvent Up ou Down selon la matrice.
            // Ici on assume un repère standard Y-Up pour la géométrie 3D.
            // On inverse X pour correspondre au mapping "interne" de la sphère (scale -1,1,1)
            float dx = cos(phi) * cos(theta - HALF_PI); // -HALF_PI pour aligner le centre de l'image avec le mur du fond (Z)
            float dy = sin(phi);
            float dz = cos(phi) * sin(theta - HALF_PI);
            
            ofVec3f dir(dx, dy, dz);
            dir.normalize();
            
            // 4. Raycast
            ofColor col = walls.getPixelFromRay(center, dir);
            pixels.setColor(x, y, col);
        }
    }
    
    ofSaveImage(pixels, "export_360_room.png");
    ofLogNotice("RoomApp") << "Image 360 sauvegardee : bin/data/export_360_room.png";
}