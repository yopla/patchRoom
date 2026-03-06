#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"
#include "LayerSystem.h"

struct Swinger {
    ofVec2f pos;
    ofVec2f oldPos; // Pour l'intégration Verlet
    
    bool isSwinging = false;
    ofVec2f ropeAnchor;
    float ropeLength;
    
    float size = 10.0f;
    ofColor color;
    
    float shootCooldown = 0.0f;
    float swingTimer = 0.0f;
};

class SwingLayer : public BaseLayer {
public:
    void setup(float w, float h, float scale, shared_ptr<ColliderLayer> collider);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;

private:
    void updateSwinger(Swinger& s, float dt);
    void shootRope(Swinger& s);

    vector<Swinger> swingers;
    shared_ptr<ColliderLayer> collider;
    
    float simWidth, simHeight;
    float scale = 1.0f;
    
    const float gravity = 0.4f;
    const float friction = 0.99f;
    const int constraintIterations = 5;
};