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

    // --- FLOU ---
    ofShader shaderBlurX;
    ofShader shaderBlurY;
    ofFbo fboPing, fboPong;
    bool bBlur = false;

    void setup() override {
        shaderBlurX.load("shaders/shaderBlurX");
        shaderBlurY.load("shaders/shaderBlurY");
    }

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

    void draw() override {
        if (bBlur) {
            // Allocation dynamique si nécessaire
            if (!fboPing.isAllocated() || fboPing.getWidth() != ofGetWidth() || fboPing.getHeight() != ofGetHeight()) {
                fboPing.allocate(ofGetWidth(), ofGetHeight());
                fboPong.allocate(ofGetWidth(), ofGetHeight());
            }

            // 1. Rendu de la scène dans FBO Ping
            fboPing.begin();
            ofClear(0);
            if (mainApp) {
                layerManager.draw(mainApp->canvas.getTexture());
            }
            fboPing.end();

            // 2. Blur Horizontal : Ping -> Pong
            fboPong.begin();
            ofClear(0);
            shaderBlurX.begin();
            shaderBlurX.setUniform1f("blurAmnt", 2.0);
            shaderBlurX.setUniform1f("texwidth", fboPing.getWidth());
            fboPing.draw(0, 0);
            shaderBlurX.end();
            fboPong.end();

            // 3. Blur Vertical : Pong -> Ecran
            shaderBlurY.begin();
            shaderBlurY.setUniform1f("blurAmnt", 2.0);
            shaderBlurY.setUniform1f("texheight", fboPong.getHeight());
            fboPong.draw(0, 0);
            shaderBlurY.end();
            
            ofDrawBitmapStringHighlight("BLUR ON (F)", 10, 60, ofColor::red, ofColor::white);
        } else {
            ofBackground(0);
            if (mainApp) {
                layerManager.draw(mainApp->canvas.getTexture());
            }
        }

        // Afficher la position actuelle pour debug
        if(myWindow){
            ofVec2f pos = myWindow->getWindowPosition();
            ofDrawBitmapStringHighlight("Win Pos: " + ofToString(pos), 10, 40);
        }
        ofDrawBitmapStringHighlight("View FPS: " + ofToString(ofGetFrameRate(), 0), 10, 20);
    }

    void keyPressed(int key) override {
        layerManager.keyPressed(key);
        if(key == 'f' || key == 'F') {
            bBlur = !bBlur;
        }
    }
};