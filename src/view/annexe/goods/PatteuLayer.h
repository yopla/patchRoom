#pragma once
#include "ofMain.h"

class PatteuLayer {
public:
    void load(const string& path);
    void reset();
    void draw(float targetW, float targetH);
    
    // Fonction de déformation (estompe)
    void smudge(ofVec2f currentPos, ofVec2f lastPos, float targetW, float targetH);

    bool bActive = false;
    
    // Paramètres de la brosse
    float brushSize = 50.0f;
    float intensity = 0.5f; // Slider (0.0 à 1.0)
    float hardness = 0.5f;  // Slider (0.0 à 1.0)

private:
    ofImage originalImage;
    ofPixels currentPixels;
    ofTexture tex;
};