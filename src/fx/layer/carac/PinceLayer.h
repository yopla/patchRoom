// /Users/ludo/Desktop/360MECA/_ofap/_of_v0.12.1_osx_release/_apps/myApps/patch-186/src/fx/layer/carac/PinceLayer.h
#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

class PinceLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;

private:
    float simWidth, simHeight;
    
    // Paramètres IK (Cinématique Inverse)
    static const int NARM = 4;
    float segLength = 60.0f;
    vector<float> angles;
    
    // Position de la base du bras
    ofVec2f basePos;
    
    // Animation de la pince (phase d'ouverture)
    float ph = 0.0f;
    
    // Solveur IK (Algorithme de poursuite récursif inspiré de la ref)
    ofVec2f pursuit(int bi, ofVec2f o, ofVec2f target);
};
