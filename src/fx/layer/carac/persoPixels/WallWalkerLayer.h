#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"
#include "LayerSystem.h"

struct WallWalker {
    ofVec2f pos;
    ofVec2f vel; 
    int dir;     // 0: Right, 1: Down, 2: Left, 3: Up
    bool isJumping;
    float size = 6.0f; // Plus petits (était 12.0f)
    ofColor color;
    float speed = 3.0f;
    float jumpTimer = 0.0f;
    int turnCount = 0; // Pour détecter les blocages
    int stuckTimer = 0; // Nouveau : Compteur de surplace global
    ofVec2f lastPos;    // Nouveau : Position précédente
};

class WallWalkerLayer : public BaseLayer {
public:
    void setup(float w, float h, float scale, shared_ptr<ColliderLayer> collider);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
private:
    vector<WallWalker> walkers;
    shared_ptr<ColliderLayer> collider;
    float simWidth, simHeight;
    float scale = 1.0f;
    
    void updateWalker(WallWalker& w);
    void jump(WallWalker& w);
    bool isWall(float x, float y);
};
