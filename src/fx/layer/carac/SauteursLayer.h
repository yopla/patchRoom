#pragma once
#include "ofMain.h"
#include "LayerSystem.h" // <--- IMPORTANT : Inclusion du parent BaseLayer
#include "ColliderLayer.h"

struct Sauteur {
    ofVec2f pos; 
    ofVec2f vel; 
    float xrad = 4;
    float yrad = 4;
    int w = 0;   
    bool ready = false;
};

// On hérite de BaseLayer pour être compatible avec le LayerManager
class SauteursLayer : public BaseLayer {
public:
    // Le setup reste identique (spécifique à ce layer)
    void setup(float realSceneWidth, float targetRealHeight, shared_ptr<ColliderLayer> colliders); 
    
    // IMPÉRATIF : La signature doit correspondre exactement à celle de BaseLayer
    void update(float mouseX, float mouseY, float time) override;
    
    // IMPÉRATIF : Override du draw
    void draw() override; 

    float simWidth = 2048.0f;
    float simHeight; 
    float scale; 

private:
    vector<Sauteur> sauteurs;
    shared_ptr<ColliderLayer> colliderLayer;
    
    int numSauteurs = 50;
};