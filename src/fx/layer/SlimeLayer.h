#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"
#include "LayerSystem.h" // <--- 1. Inclusion du système de Layer

struct SlimeParticle {
    ofVec2f pos;
    ofVec2f vel;
    ofColor color;
    float radius;
    float age;
    float life;
    bool isSplitter; 
};

// <--- 2. Héritage de BaseLayer
class SlimeLayer : public BaseLayer {
public:
    void setup(float w, float h);

    // <--- 3. Override des méthodes virtuelles
    void update(float mouseX, float mouseY) override;
    void draw() override;
    
    // Méthode spécifique conservée
    void pour(float x, float y, float vx_start = 0, float vy_start = 0);
    
    void setCollider(shared_ptr<ColliderLayer> c) { collider = c; }
    void setScale(float s) { scale = s; }

private:
    vector<SlimeParticle> particles;
    float width, height;
    float gravity;
    float friction;
    
    float scale = 1.0f; 

    shared_ptr<ColliderLayer> collider;
};