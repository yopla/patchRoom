#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "ViewLayerManager.h"

class ViewApp : public ofBaseApp {
public:
    shared_ptr<ofApp> mainApp;
    ViewLayerManager layerManager;
    
    // Gestion fenêtre
    shared_ptr<ofAppBaseWindow> myWindow;
    ofVec2f targetPos;
    ofVec2f targetSize;
    bool bMoved = false;

    void setupView(shared_ptr<ofApp> app) { mainApp = app; }
    
    void setWindowMovement(shared_ptr<ofAppBaseWindow> win, int x, int y, int w, int h) {
        myWindow = win;
        targetPos.set(x, y);
        targetSize.set(w, h);
    }

    // --- NOUVEAU : Méthode pour déplacer la fenêtre via OSC ---
    void moveWindow(int x, int y) {
        if (myWindow) {
            myWindow->setWindowPosition(x, y);
            // On met à jour targetPos pour ne pas que l'update() l'écrase
            targetPos.set(x, y); 
            bMoved = true; // On considère le mouvement fait
        }
    }

    void update() {
        // Ta logique existante de mouvement automatique au démarrage
        if (!bMoved && ofGetElapsedTimef() > 3.0 && myWindow) {
            myWindow->setWindowPosition(targetPos.x, targetPos.y);
            if(targetSize.x > 0 && targetSize.y > 0){
                myWindow->setWindowShape(targetSize.x, targetSize.y);
            }
            bMoved = true;
        }
    }

    void draw() {
        ofBackground(0);
        if(mainApp) {
            layerManager.draw(mainApp->canvas.getTexture());
        }
        // Afficher la position actuelle pour debug
        if(myWindow){
            ofVec2f pos = myWindow->getWindowPosition();
            ofDrawBitmapStringHighlight("Win Pos: " + ofToString(pos), 10, 40);
        }
        ofDrawBitmapStringHighlight("View FPS: " + ofToString(ofGetFrameRate(), 0), 10, 20);
    }

    void keyPressed(int key) {
        layerManager.keyPressed(key);
    }
};