#pragma once
#include "ofMain.h"
#include "LayerSystem.h"
#include "ColliderLayer.h"
#include "EatMapLayer.h"

struct ExplodingPixel {
    ofVec2f pos;
    ofVec2f vel;
    ofColor color;
    float life;
    float maxLife;
};

struct SurSauteur {
    ofVec2f pos; 
    ofVec2f vel; 
    float xrad = 4;
    float yrad = 4;
    int w = 0;   
    bool ready = false;
};

class SurSauteurLayer : public BaseLayer {
public:
    void setup(float realSceneWidth, float targetRealHeight, shared_ptr<ColliderLayer> colliders, shared_ptr<EatMapLayer> eatMap); 
    void update(float mouseX, float mouseY, float time) override;
    void draw() override; 
    
    void loadTexture(string path);
    void reset();

    float simWidth = 2048.0f;
    float simHeight; 
    float scale; 

private:
    vector<SurSauteur> sauteurs;
    vector<ExplodingPixel> particles;
    shared_ptr<ColliderLayer> colliderLayer;
    shared_ptr<EatMapLayer> eatMapLayer;
    
    int numSauteurs = 50;
    
    ofImage textSurSaut;
    ofPixels originalTextPixels;
    string currentTextPath = "";
    float textSimOffsetY = 0.0f;
    bool bHasText = false;
    
    void explode(float x, float y, float radius);
};