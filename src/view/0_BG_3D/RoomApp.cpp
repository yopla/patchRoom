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

    // Initialisation du ColorCopRing
    colorCopRing.setup(3000.0f, ofVec3f(0, 600, 0), 20);

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
    camGlobal.setFarClip(150000.0f); // Très lointain pour voir la totalité du tuyau géant
    
    camFront.setFarClip(150000.0f);
    camBack.setFarClip(150000.0f);
    camCour.setFarClip(150000.0f);
    camJar.setFarClip(150000.0f);
    camSol.setFarClip(150000.0f);
    camTopCour.setFarClip(150000.0f);
    camTopJar.setFarClip(150000.0f);
    
    rigPosition.set(0, 600, 0);

    // Initialisation du sol ondulant (Largeur pièce, Z début, Z fin)
    undulatingFloor.setup(60000.0f, -30000.0f, 30000.0f);
    
    // Initialisation du Kraken
    kraken.setup(); // <--- AJOUT
    externalKraken.setup(); // <--- AJOUT

    // Setup de la boite texturée (légèrement plus grande que la room)
    boxTexture.setup(roomWidth + 200, heightFrontBack + 200, roomDepth + 200);
    jupyterBox.setup(roomWidth + 200, heightFrontBack + 200, roomDepth + 200);
    golBox.setup(roomWidth + 200, heightFrontBack + 200, roomDepth + 200);
    golBoxMotion.setup(roomWidth + 200, heightFrontBack + 200, roomDepth + 200);
    autoSnakeBox.setup(roomWidth, heightFrontBack, roomDepth); // Dimensions exactes pour aligner les arêtes

    // Initialisation du lecteur vidéo 360
    scene360VideoPlayer.setup(&atmosphere, "_scene");
    
    // Initialisation du Tuyau 3D
    tuyau.setup();
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
    
    // --- MISE A JOUR DU COLOR COP RING ---
    if (bDrawColorCop) {
        colorCopRing.update(ofGetLastFrameTime());
    }

    // --- MISE A JOUR DU LECTEUR VIDEO 360 ---
    if (bDrawScene360Video) {
        scene360VideoPlayer.update();
    }
    
    if (bDrawTuyau) {
        tuyau.update(localTime);
    }

    if (bDrawJupyterBox) {
        jupyterBox.update();
    }

    if (bDrawGolBox) {
        golBox.update();
    }

    if (bDrawGolBoxMotion) {
        golBoxMotion.update();
    }

    if (bDrawAutoSnakeBox) {
        autoSnakeBox.update();
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
    
    if (bDrawBoxTexture) {
        boxTexture.draw();
    }
    
    if (bDrawJupyterBox) {
        jupyterBox.draw();
    }
    
    if (bDrawGolBox) {
        golBox.draw();
    }
    
    if (bDrawGolBoxMotion) {
        golBoxMotion.draw();
    }
    
    if (bDrawAutoSnakeBox) {
        autoSnakeBox.draw();
    }

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
    
    // --- DESSIN DU COLOR COP RING ---
    if (bDrawColorCop) {
        colorCopRing.draw();
    }
    
    if (bDrawTuyau) {
        tuyau.draw();
    }

   if(bDrawGab && bgMode != 1) {
        ofFbo* fFront = nullptr; ofFbo* fBack = nullptr; ofFbo* fCour = nullptr;
        ofFbo* fJar = nullptr; ofFbo* fSol = nullptr; ofFbo* fTopCour = nullptr; ofFbo* fTopJar = nullptr;
        if (bgMode == 2) {
            fFront = scene2DFboFront; fBack = scene2DFboBack; fCour = scene2DFboCour; fJar = scene2DFboJar; fSol = scene2DFboSol; fTopCour = scene2DFboTopCour; fTopJar = scene2DFboTopJar;
        } else if (bgMode == 3) {
            fFront = zenitFboFront; fBack = zenitFboBack; fCour = zenitFboCour; fJar = zenitFboJar; fSol = zenitFboSol; fTopCour = zenitFboTopCour; fTopJar = zenitFboTopJar;
        }
        
        walls.draw(bShowRoof, wallAlpha, bgMode,
                   fFront, fBack, fCour, fJar,
                   fSol, fTopCour, fTopJar);
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
    ofDrawBitmapString("COLOR COP: " + ofToString(bDrawColorCop), 20, 170); // <--- AJOUT NOUVEAU
    ofDrawBitmapString("SCENE 360 [8]: " + ofToString(bDrawScene360Video), 20, 185); // <--- AJOUT
    ofDrawBitmapString("TUYAU 3D [9]: " + ofToString(bDrawTuyau), 20, 200); // <--- AJOUT
    ofDrawBitmapString("TUYAU OBJ: " + ofToString(tuyau.bDrawTuyauObj), 20, 215); 
    ofDrawBitmapString("TUYAU ARCS: " + ofToString(tuyau.bDrawArcs), 20, 230); 
    ofDrawBitmapString("COUTURE ARCS: " + ofToString(tuyau.bDrawCouture), 20, 245); 
    
    if(cursorSquare.isVisible) {
        ofDrawBitmapString("CURSOR 3D: " + ofToString(cursorSquare.getCurrentPos()), 20, 260);
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

void RoomApp::generateFull360EquirectangularImage() {
    int w = 3840;
    int h = 2160;
    
    ofLogNotice("RoomApp") << "Debut generation image 360 Full (" << w << "x" << h << ")... Cela peut prendre quelques secondes.";
    
    // On lit les pixels des FBOs de la scene 2D s'ils sont disponibles
    ofPixels pFront, pBack, pCour, pJar, pSol, pTopCour, pTopJar;
    if(bgMode == 2) {
        if(scene2DFboFront && scene2DFboFront->isAllocated()) scene2DFboFront->readToPixels(pFront);
        if(scene2DFboBack && scene2DFboBack->isAllocated()) scene2DFboBack->readToPixels(pBack);
        if(scene2DFboCour && scene2DFboCour->isAllocated()) scene2DFboCour->readToPixels(pCour);
        if(scene2DFboJar && scene2DFboJar->isAllocated()) scene2DFboJar->readToPixels(pJar);
        if(scene2DFboSol && scene2DFboSol->isAllocated()) scene2DFboSol->readToPixels(pSol);
        if(scene2DFboTopCour && scene2DFboTopCour->isAllocated()) scene2DFboTopCour->readToPixels(pTopCour);
        if(scene2DFboTopJar && scene2DFboTopJar->isAllocated()) scene2DFboTopJar->readToPixels(pTopJar);
    } else if(bgMode == 3) {
        if(zenitFboFront && zenitFboFront->isAllocated()) zenitFboFront->readToPixels(pFront);
        if(zenitFboBack && zenitFboBack->isAllocated()) zenitFboBack->readToPixels(pBack);
        if(zenitFboCour && zenitFboCour->isAllocated()) zenitFboCour->readToPixels(pCour);
        if(zenitFboJar && zenitFboJar->isAllocated()) zenitFboJar->readToPixels(pJar);
        if(zenitFboSol && zenitFboSol->isAllocated()) zenitFboSol->readToPixels(pSol);
        if(zenitFboTopCour && zenitFboTopCour->isAllocated()) zenitFboTopCour->readToPixels(pTopCour);
        if(zenitFboTopJar && zenitFboTopJar->isAllocated()) zenitFboTopJar->readToPixels(pTopJar);
    }

    ofPixels pixels;
    pixels.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
    
    ofVec3f center(0, 600, 0); 

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float u = (float)x / (float)w;
            float v = (float)y / (float)h;
            
            float theta = (u - 0.5f) * TWO_PI; 
            float phi = (0.5f - v) * PI;
            
            float dx = cos(phi) * cos(theta - HALF_PI); 
            float dy = sin(phi);
            float dz = cos(phi) * sin(theta - HALF_PI);
            
            ofVec3f dir(dx, dy, dz);
            dir.normalize();
            
            ofColor col = walls.getPixelFromRayDynamic(center, dir, bgMode, &pFront, &pBack, &pCour, &pJar, &pSol, &pTopCour, &pTopJar);
            pixels.setColor(x, y, col);
        }
    }
    
    string filename = "360Full_" + ofGetTimestampString() + ".png";
    ofSaveImage(pixels, filename);
    ofLogNotice("RoomApp") << "Image 360 Full sauvegardee : bin/data/" << filename;
}

void RoomApp::generate360FullW() {
    int w = 3840;
    int h = 2160;
    int faceSize = 2048; // Resolution interne pour chaque face du cube (4K global)
    
    ofLogNotice("RoomApp") << "Debut generation image 360 Full W (" << w << "x" << h << ") par Cubemap... Cela va prendre quelques secondes.";
    
    // Configuration des 6 cameras formant le Cubemap
    ofCamera cams[6];
    ofVec3f center(0, 600, 0); // Centre (Rig)
    for(int i=0; i<6; i++) {
        cams[i].setPosition(center);
        cams[i].setFov(90.0f);
        cams[i].setAspectRatio(1.0f); // Obligatoire pour eviter la distorsion
        cams[i].setNearClip(10.0f);
        cams[i].setFarClip(30000.0f); // Assez loin pour le sol ondulant
    }
    
    cams[0].lookAt(center + ofVec3f(1, 0, 0), ofVec3f(0, 1, 0));   // +X (Droite)
    cams[1].lookAt(center + ofVec3f(-1, 0, 0), ofVec3f(0, 1, 0));  // -X (Gauche)
    cams[2].lookAt(center + ofVec3f(0, 1, 0), ofVec3f(0, 0, -1));  // +Y (Haut)
    cams[3].lookAt(center + ofVec3f(0, -1, 0), ofVec3f(0, 0, 1));  // -Y (Bas)
    cams[4].lookAt(center + ofVec3f(0, 0, 1), ofVec3f(0, 1, 0));   // +Z (Arriere)
    cams[5].lookAt(center + ofVec3f(0, 0, -1), ofVec3f(0, 1, 0));  // -Z (Avant)

    ofPixels faces[6];
    ofFbo fbo;
    fbo.allocate(faceSize, faceSize, GL_RGB);

    // Rendu successif de la vraie scene 3D pour chaque face
    for(int i=0; i<6; i++) {
        fbo.begin();
        ofClear(0, 255);
        cams[i].begin();
        drawSceneContent(bDrawAtmosphere, false); // On desactive la vue globale pour garder l'effet de tangage
        cams[i].end();
        fbo.end();
        fbo.readToPixels(faces[i]);
    }

    ofPixels eqPixels;
    eqPixels.allocate(w, h, OF_IMAGE_COLOR);

    // Reconstruction Equirectangulaire a partir du Cubemap
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float u = (float)x / (float)w;
            float v = (float)y / (float)h;

            float theta = (u - 0.5f) * TWO_PI;
            float phi = (0.5f - v) * PI;

            float dx = cos(phi) * cos(theta - HALF_PI);
            float dy = sin(phi);
            float dz = cos(phi) * sin(theta - HALF_PI);
            ofVec3f dir(dx, dy, dz); dir.normalize();

            float absX = fabs(dir.x), absY = fabs(dir.y), absZ = fabs(dir.z);
            int faceIndex = 0;
            float ma, faceU, faceV;

            if (absX >= absY && absX >= absZ) {
                ma = absX; if (dir.x > 0) { faceIndex = 0; faceU = dir.z; faceV = -dir.y; } else { faceIndex = 1; faceU = -dir.z; faceV = -dir.y; }
            } else if (absY >= absX && absY >= absZ) {
                ma = absY; if (dir.y > 0) { faceIndex = 2; faceU = -dir.x; faceV = dir.z; } else { faceIndex = 3; faceU = -dir.x; faceV = -dir.z; }
            } else {
                ma = absZ; if (dir.z > 0) { faceIndex = 4; faceU = -dir.x; faceV = -dir.y; } else { faceIndex = 5; faceU = dir.x; faceV = -dir.y; }
            }

            faceU = (faceU / ma + 1.0f) * 0.5f;
            faceV = (faceV / ma + 1.0f) * 0.5f; // L'inversion Y est deja resolue dans les signes ci-dessus

            eqPixels.setColor(x, y, faces[faceIndex].getColor(ofClamp(faceU * faceSize, 0, faceSize - 1), ofClamp(faceV * faceSize, 0, faceSize - 1)));
        }
    }
    string filename = "360FullW_" + ofGetTimestampString() + ".png";
    ofSaveImage(eqPixels, filename);
    ofLogNotice("RoomApp") << "Image 360 Full W sauvegardee par Cubemap : bin/data/" << filename;
}