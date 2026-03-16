#include "RoomPreview.h"
#include "ofApp.h" // Nécessaire pour accéder à mainApp->canvas
#include "Scene2D_SIDE.h"
#include "ButtonApp.h"


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
    
    // (La configuration de la caméra se fait dans update() pour s'adapter à la taille de l'écran)
}

//--------------------------------------------------------------
void RoomPreview::update(){
    if (bPaused) return;

    if(bShowCursor) cursorSquare.updateRaycast(camGlobal, walls);

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

    // --- CHECK HOVER SUR LES BOUTONS EN 3D ---
    if (bDrawInteraction && mainApp && mainApp->buttonApp) {
        int mx = ofGetMouseX();
        int my = ofGetMouseY();
        if (mx >= 0 && mx < w && my >= 0 && my < h) {
            ofVec3f nearPoint = camGlobal.screenToWorld(ofVec3f(mx, my, 0.0f));
            ofVec3f farPoint = camGlobal.screenToWorld(ofVec3f(mx, my, 1.0f));
            ofVec3f mouseRay = farPoint - nearPoint;
            mouseRay.normalize();
            
            if (mouseRay.y != 0) {
                float t = (2.0f - nearPoint.y) / mouseRay.y; // 2.0f est la hauteur où les boutons sont dessinés
                if (t > 0) {
                    ofVec3f intersect = nearPoint + mouseRay * t;
                    
                    float d2 = roomDepth / 2.0f;
                    float pctX = (intersect.x + roomWidth / 2.0f) / roomWidth;
                    float pctY = (intersect.z - (-d2)) / roomSolDepth;
                    
                    if (pctX >= 0 && pctX <= 1 && pctY >= 0 && pctY <= 1) {
                        float winW = mainApp->buttonApp->buttonWindow.getWidth();
                        float winH = mainApp->buttonApp->buttonWindow.getHeight();
                        mainApp->buttonApp->buttonWindow.setExternalHover(pctX * winW, pctY * winH);
                    } else {
                        mainApp->buttonApp->buttonWindow.clearExternalHover();
                    }
                } else {
                    mainApp->buttonApp->buttonWindow.clearExternalHover();
                }
            } else {
                mainApp->buttonApp->buttonWindow.clearExternalHover();
            }
        } else {
            mainApp->buttonApp->buttonWindow.clearExternalHover();
        }
    } else if (mainApp && mainApp->buttonApp) {
        mainApp->buttonApp->buttonWindow.clearExternalHover();
    }

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

        if(bShowCursor) cursorSquare.drawProjected(walls);
        
        if(bDrawInteraction) {
            interactionVisualizer.draw(mainApp, sceneSide, roomWidth, roomDepth, roomSolDepth);
        }
        
        // Petit repère visuel
        ofSetColor(255, 0, 255); ofDrawSphere(0, 600, 0, 10);
    camGlobal.end();
    
    ofSetColor(255);
    ofDrawBitmapString("PREVIEW RECOMPOSEE", 20, 20);
    ofDrawBitmapString("Textures issues du Canvas", 20, 40);

    if(bShowCursor && cursorSquare.isVisible) {
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
    if(key == 'b' || key == 'B'){
        bDrawInteraction = !bDrawInteraction;
    }
    if(key == 's' || key == 'S'){
        bShowCursor = !bShowCursor;
    }
}

//--------------------------------------------------------------
void RoomPreview::mousePressed(int x, int y, int button){
    if (bDrawInteraction && mainApp && mainApp->buttonApp) {
        mainApp->buttonApp->buttonWindow.checkExternalClick();
    }
}