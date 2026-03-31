#pragma once
#include "ofMain.h"

class DeuPatteuLayer {
public:
    void loadFg(const string& path);
    void loadBg(const string& path);
    void resetFg();
    void resetBg();
    void draw(float targetW, float targetH);
    
    // Fonction de déformation (estompe + révélation)
    void smudge(ofVec2f currentPos, ofVec2f lastPos, float targetW, float targetH);

    bool bActive = false;
    
    // Paramètres de la brosse
    float brushSize = 50.0f;
    float intensity = 0.5f;   // Slider (0.0 à 1.0)
    float hardness = 0.5f;    // Slider (0.0 à 1.0)
    float revealSpeed = 0.2f; // Slider (0.0 à 1.0) - Vitesse d'effacement du FG

private:
    ofImage originalFgImage;
    ofImage originalBgImage;
    ofPixels currentFgPixels;
    ofPixels currentBgPixels;
    ofTexture fgTex;
    ofTexture bgTex;
};