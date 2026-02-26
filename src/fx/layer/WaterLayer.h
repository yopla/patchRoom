#pragma once
#include "ofMain.h"

struct WaterParticle {
    ofVec2f pos;
    ofVec2f vel;
    ofColor color;
    float size;
    float age;
    float life;
    float gravity;
};

class WaterLayer {
public:
    void setup(float w, float h);
    void update();
    void draw();
    
    // Fonction pour "verser" de l'eau (équivalent de pour() en AS3)
    void pour(float x, float y);
    
    // Interaction
    void setGravity(float g);

private:
    vector<WaterParticle> particles;
    float width, height;
    float gravityVal;
};