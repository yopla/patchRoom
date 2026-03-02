#include "RoomPreview.h"
#include "ofApp.h" // Nécessaire pour accéder à mainApp->canvas
#include "../ButtonApp.h" // Nécessaire pour accéder à ButtonApp et ButtonWindow
#include "../2_vueSIDE/Scene2D_SIDE.h"


//--------------------------------------------------------------
void RoomPreview::setup(){
    ofSetVerticalSync(true);
    ofEnableDepthTest();
    ofDisableArbTex(); 
    
    // 1. Définition des coordonnées de CROP
    cropX_Front = 0;    cropY_Front = 0;
    cropX_Sol   = 0;    cropY_Sol   = 2944;
    cropX_Back  = 0;    cropY_Back  = 1472;
    cropX_Jar   = 2400; cropY_Jar   = 3680;
    cropX_Cour  = 2400; cropY_Cour  = 1008;
    cropX_TopJar = 2400; cropY_TopJar = 2080;
    cropX_TopCour = 2400; cropY_TopCour = 0;

    // 2. Allocation des FBOs
    texFront.allocate(roomWidth, heightFrontBack, GL_RGB);
    texBack.allocate(roomWidth, heightFrontBack, GL_RGB);
    texCour.allocate(roomDepth, heightCour, GL_RGB);
    texJar.allocate(roomDepth, heightJar, GL_RGB);
    
    // --- CORRECTION ICI : Utiliser roomSolDepth au lieu de roomDepth ---
    // Cela assure que la texture a la même taille que dans RoomApp
    texSol.allocate(roomWidth, roomSolDepth, GL_RGB); 
    // -------------------------------------------------------------------

    texTopCour.allocate(roomDepth, heightTopCour, GL_RGB);
    texTopJar.allocate(roomDepth, heightTopJar, GL_RGB);

    walls.setup();
    cursorSquare.setup();

    rigPosition.set(0, 600, 0); 
    
    // 3. Setup Géométrie
    float w2 = roomWidth / 2.0f; 
    float d2 = roomDepth / 2.0f;
    
    peakX = 1452.0f - 1200.0f; 
    peakY = heightFrontBack - 20.0f; 
    
    // --- CORRECTION GEOMETRIE DU SOL (LE TROU) ---
    // Le sol commence au mur FRONT (-d2) et s'arrête à roomSolDepth
    float zStartSol = -d2;
    float zEndSol   = -d2 + roomSolDepth; // C'est ici que le sol s'arrête (création du trou)

    meshSol.clear();
    meshSol.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    // Coin Haut Gauche (Coté Front)
    meshSol.addVertex(ofVec3f(-w2, 0, zStartSol)); meshSol.addTexCoord(ofVec2f(0.0f, 0.0f));
    // Coin Haut Droite (Coté Front)
    meshSol.addVertex(ofVec3f(w2, 0, zStartSol));  meshSol.addTexCoord(ofVec2f(1.0f, 0.0f));
    // Coin Bas Droite (Coté Vide/Trou) - On utilise zEndSol ici !
    meshSol.addVertex(ofVec3f(w2, 0, zEndSol));    meshSol.addTexCoord(ofVec2f(1.0f, 1.0f));
    // Coin Bas Gauche (Coté Vide/Trou) - On utilise zEndSol ici !
    meshSol.addVertex(ofVec3f(-w2, 0, zEndSol));   meshSol.addTexCoord(ofVec2f(0.0f, 1.0f));
    // ---------------------------------------------

    // Points Toits (Reste inchangé)
    ofVec3f peakFront(peakX, peakY, -d2); ofVec3f peakBack(peakX, peakY, d2);
    ofVec3f wallCourFront(w2, heightCour, -d2); ofVec3f wallCourBack(w2, heightCour, d2);
    ofVec3f wallJarFront(-w2, heightJar, -d2); ofVec3f wallJarBack(-w2, heightJar, d2);
    
    // Mesh Top Cour (Reste inchangé)
    meshTopCour.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshTopCour.addVertex(peakFront);     meshTopCour.addTexCoord(ofVec2f(0.0f, 0.0f)); 
    meshTopCour.addVertex(peakBack);      meshTopCour.addTexCoord(ofVec2f(1.0f, 0.0f)); 
    meshTopCour.addVertex(wallCourBack);  meshTopCour.addTexCoord(ofVec2f(1.0f, 1.0f)); 
    meshTopCour.addVertex(wallCourFront); meshTopCour.addTexCoord(ofVec2f(0.0f, 1.0f)); 

    // Mesh Top Jar (Reste inchangé)
    meshTopJar.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshTopJar.addVertex(peakFront);      meshTopJar.addTexCoord(ofVec2f(1.0f, 0.0f)); 
    meshTopJar.addVertex(peakBack);       meshTopJar.addTexCoord(ofVec2f(0.0f, 0.0f)); 
    meshTopJar.addVertex(wallJarBack);    meshTopJar.addTexCoord(ofVec2f(0.0f, 1.0f)); 
    meshTopJar.addVertex(wallJarFront);   meshTopJar.addTexCoord(ofVec2f(1.0f, 1.0f)); 

    camGlobal.setDistance(4000); 
    camGlobal.setFarClip(50000);
    camGlobal.setPosition(2000, 2500, 3000); 
    camGlobal.lookAt(ofVec3f(0, 600, 0));
}

//--------------------------------------------------------------
void RoomPreview::update(){
    if (bPaused) return;

    cursorSquare.updateRaycast(camGlobal, walls);

    // --- C'EST ICI QU'ON RECUPERE LES TEXTURES DU CANVAS ---
    if(mainApp && mainApp->canvas.isAllocated()){
        
        ofTexture& giantTex = mainApp->canvas.getTexture();
        
        // Fonction lambda pour copier un morceau du canvas vers nos FBOs locaux
        auto updateTexture = [&](ofFbo& targetFbo, float srcX, float srcY) {
            targetFbo.begin();
           ofClear(0, 255); // Raccourci pour noir opaque
            // On dessine la sous-partie de la texture géante
            // drawSubsection(x_dest, y_dest, w_dest, h_dest, x_src, y_src, w_src, h_src)
            giantTex.drawSubsection(0, 0, targetFbo.getWidth(), targetFbo.getHeight(), 
                                    srcX, srcY, targetFbo.getWidth(), targetFbo.getHeight());
            targetFbo.end();
        };

        // Mise à jour de toutes les textures
        updateTexture(texFront, cropX_Front, cropY_Front);
        updateTexture(texBack,  cropX_Back,  cropY_Back);
        updateTexture(texSol,   cropX_Sol,   cropY_Sol);
        updateTexture(texJar,   cropX_Jar,   cropY_Jar);
        updateTexture(texCour,  cropX_Cour,  cropY_Cour);
        updateTexture(texTopJar, cropX_TopJar, cropY_TopJar);
        updateTexture(texTopCour, cropX_TopCour, cropY_TopCour);
    }
}

//--------------------------------------------------------------
void RoomPreview::drawRoomGeometry(){
    // On dessine la géométrie en utilisant nos FBOs locaux comme textures
    
    ofEnableAlphaBlending(); 
    ofSetColor(255); // Pas de transparence ici, on veut voir le résultat final (ou mettre 255,255,255,76 si voulu)

    float w2 = roomWidth / 2.0f; float d2 = roomDepth / 2.0f;
    
    texFront.getTexture().bind(); ofDrawPlane(0, heightFrontBack/2, -d2, roomWidth, heightFrontBack); texFront.getTexture().unbind();
    
    texBack.getTexture().bind(); ofPushMatrix(); ofTranslate(0, heightFrontBack/2, d2); ofRotateYDeg(180); ofDrawPlane(0, 0, 0, roomWidth, heightFrontBack); ofPopMatrix(); texBack.getTexture().unbind();
    
    texJar.getTexture().bind(); ofPushMatrix(); ofTranslate(-w2, heightJar/2, 0); ofRotateYDeg(90); ofDrawPlane(0, 0, 0, roomDepth, heightJar); ofPopMatrix(); texJar.getTexture().unbind();
    
    texCour.getTexture().bind(); ofPushMatrix(); ofTranslate(w2, heightCour/2, 0); ofRotateYDeg(-90); ofDrawPlane(0, 0, 0, roomDepth, heightCour); ofPopMatrix(); texCour.getTexture().unbind();
    
    texSol.getTexture().bind(); meshSol.draw(); texSol.getTexture().unbind();
    
  if(mainApp && mainApp->roomApp && mainApp->roomApp->bShowRoof) {
        texTopCour.getTexture().bind(); meshTopCour.draw(); texTopCour.getTexture().unbind(); 
        texTopJar.getTexture().bind(); meshTopJar.draw(); texTopJar.getTexture().unbind(); 
    }
    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void RoomPreview::draw(){
    if (bPaused) {
        ofBackground(0);
        return;
    }
    
    ofBackground(0);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    camGlobal.begin();
        ofDrawAxis(500);
        ofPushMatrix(); ofRotateXDeg(90); ofSetColor(50); ofDrawGridPlane(1000, 10, false); ofPopMatrix();
        
        ofEnableDepthTest();
        drawRoomGeometry();
        ofDisableDepthTest();

        cursorSquare.drawProjected(walls);
        
        // Dessin des boutons sur le sol (intégré à la scène 3D)
        if(mainApp && mainApp->buttonApp){
             float d2 = roomDepth / 2.0f;
             float zCenterSol = -d2 + roomSolDepth / 2.0f;
             // On dessine légèrement au dessus du sol (y=2) pour éviter le z-fighting
             mainApp->buttonApp->buttonWindow.drawPreview(0, 2, zCenterSol, roomWidth, roomSolDepth);
             
             // Ligne jaune entre le bouton survolé et les HaloCreatures
             vector<ofVec3f> btnPositions = mainApp->buttonApp->buttonWindow.get3DPosForActiveButtons(roomWidth, roomSolDepth, roomDepth);
             
             if(!btnPositions.empty()) {
                 if(sceneSide) {
                     for(auto& btnPos : btnPositions) {
                         for(auto& h : sceneSide->layerManager.halos) {
                             ofVec3f haloPos = sceneSide->get3DPos(h->pos.x, h->pos.y);
                             ofPushStyle();
                             ofSetColor(255, 255, 0);
                             ofSetLineWidth(4);
                             ofDrawLine(btnPos, haloPos);
                             ofPopStyle();
                         }
                     }
                 }

                 // Visualisation des liens vers les Halos du LightFlyRing
                 if(mainApp && mainApp->roomApp && mainApp->roomApp->bLightFlyRingEnabled) {
                     auto& ring = mainApp->roomApp->lightFlyRing;
                     float fboW = ring.fbo.getWidth();
                     float fboH = ring.fbo.getHeight();
                     float totalH = ring.height + ring.bottomExt;

                     for(auto& btnPos : btnPositions) {
                         for(auto& c : ring.creatures) {
                             // Conversion coordonnées FBO (2D) -> Monde (3D Cylindrique)
                             float u = c->pos.x / fboW;
                             float v = c->pos.y / fboH;
                             
                             float angle = u * TWO_PI;
                             float yWorld = ring.height - (v * totalH);
                             float xWorld = cos(angle) * ring.radius;
                             float zWorld = sin(angle) * ring.radius;
                             
                             ofVec3f haloPos(xWorld, yWorld, zWorld);
                             
                             ofPushStyle();
                             ofSetColor(255, 255, 0);
                             ofSetLineWidth(4);
                             ofDrawLine(btnPos, haloPos);
                             ofPopStyle();
                         }
                     }
                 }
             }
        }
        
        // Petit repère visuel
        ofSetColor(255, 0, 255); ofDrawSphere(0, 600, 0, 30);
    camGlobal.end();
    
    ofSetColor(255);
    ofDrawBitmapString("PREVIEW RECOMPOSEE", 20, 20);
    ofDrawBitmapString("Textures issues du Canvas", 20, 40);

    if(cursorSquare.isVisible) {
        ofDrawBitmapString("CURSOR PREVIEW: " + ofToString(cursorSquare.getCurrentPos()), 20, 60);
    }
}

//--------------------------------------------------------------
void RoomPreview::keyPressed(int key){
    // --- RESET CAMERA ---
    if(key == 'r' || key == 'R'){
        camGlobal.setDistance(4000);
        camGlobal.setPosition(2000, 2500, 3000);
        camGlobal.lookAt(ofVec3f(0, 600, 0));
    }
    if(key == 'a' || key == 'A') {
        if(mainApp && mainApp->roomApp) {
            mainApp->roomApp->bShowRoof = !mainApp->roomApp->bShowRoof;
        }
    }
}