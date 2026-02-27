#include "CanvasManager.h"

void CanvasManager::setup(int w, int h) {
    width = w;
    height = h;
    canvas.allocate(width, height, GL_RGBA, 4);
    imgFullGab.load("GAB/_fuyllgab.jpg");
    bIsVideo = false;
    canvas.begin(); ofClear(0); canvas.end();
}


void CanvasManager::update() {
  if (bIsVideo && videoPlayer.isLoaded()) {
        videoPlayer.update();
    }
}

void CanvasManager::loadFile(string path) {
    ofFile file(path);
    string ext = ofToUpper(file.getExtension());

    // 1. On arrête systématiquement la vidéo précédente au cas où
    if (videoPlayer.isLoaded()) {
        videoPlayer.stop();
        videoPlayer.close();
    }

    if (ext == "MOV" || ext == "MP4") {
        videoPlayer.setPixelFormat(OF_PIXELS_NATIVE); 
        videoPlayer.load(path);
        videoPlayer.play();
        videoPlayer.setLoopState(OF_LOOP_NORMAL);
        bIsVideo = true;
    } 
    // 2. Gestion explicite des images
    else if (ext == "JPG" || ext == "JPEG" || ext == "PNG") {
        imgFullGab.load(path);
        bIsVideo = false; // Très important : on repasse en mode image
    }
}



void CanvasManager::drawBackground(shared_ptr<RoomApp> room, 
                                   shared_ptr<Scene2D_SIDE> scene2D, 
                                   shared_ptr<Scene2DZenit> sceneZenit, 
                                   bool showGab,
                                   bool bDrawRoom, 
                                   bool bDrawZenit, 
                                   bool bDrawScene2D) {    
    
    ofClear(0, 0, 0, 0);
    ofBackground(0);

    ofEnableAlphaBlending();

    // ------------------------------------------------
    // LAYER 0 (BAS) : ROOM APP (Touche W)
    // ------------------------------------------------
    if (room && bDrawRoom) {
        ofSetColor(255);
        room->fboFront.draw(0, 0);
        room->fboSol.draw(0, 2944);
        room->fboBack.draw(0, 1472);
        room->fboJar.draw(2400, 3680);
        room->fboCour.draw(2400, 1008);
        if (room->fboTopJar.isAllocated()) room->fboTopJar.draw(2400, 2080);
        if (room->fboTopCour.isAllocated()) room->fboTopCour.draw(2400, 0);
    }
  
   if (showGab) {
        ofSetColor(255);
        
        if (bIsVideo && videoPlayer.isLoaded()) {
            // Dessine la vidéo étirée à la taille du canvas
            videoPlayer.draw(0, 0, width, height); 
        }
        else if (!bIsVideo && imgFullGab.isAllocated()) {
            // Dessine l'image
            imgFullGab.draw(0, 0, width, height);
        }
    }
    // ------------------------------------------------
    // LAYER 1 (MILIEU) : SCENE ZENIT (Touche X)
    // ------------------------------------------------
    if (sceneZenit && bDrawZenit) {
        ofSetColor(255); 
        // Note: l'alpha blending est activé, donc si tes FBOs ont de la transparence, 
        // on verra la Room en dessous.
        sceneZenit->fboFront.draw(0, 0);
        sceneZenit->fboBack.draw(0, 1472);
        sceneZenit->fboJar.draw(2400, 3680);
        sceneZenit->fboCour.draw(2400, 1008);
        
        sceneZenit->fboSol.draw(0, 2944);
        sceneZenit->fboTopJar.draw(2400, 2080);
        sceneZenit->fboTopCour.draw(2400, 0);
    }
  

    // ------------------------------------------------
    // LAYER 2 (HAUT) : SCENE 2D CLASSIQUE (Touche C)
    // ------------------------------------------------
    if (scene2D && bDrawScene2D) {
        ofSetColor(255);
        scene2D->fboFront.draw(0, 0);
        scene2D->fboBack.draw(0, 1472);
        scene2D->fboJar.draw(2400, 3680);
        scene2D->fboCour.draw(2400, 1008);
        scene2D->fboSol.draw(0, 2944);
        scene2D->fboTopJar.draw(2400, 2080);
        scene2D->fboTopCour.draw(2400, 0);
    }

  
    // ------------------------------------------------
    // LAYER 3 (OVERLAY) : GABARIT OU VIDEO (Touche G)
    // ------------------------------------------------
 

    ofDisableAlphaBlending();
}