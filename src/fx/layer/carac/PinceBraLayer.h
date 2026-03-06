#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

struct PinceNode {
    ofVec2f pos;
    float rot = 0.0f;
};

class PinceBra {
public:
    void setup(float x, float y, int numNodes);
    void update(float targetX, float targetY);
    void draw();
    
    vector<PinceNode> nodes;
    ofVec2f basePos;
    float segmentLength = 30.0f;
};

class PinceBraLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
    // Interaction pour ajouter des bras
    void mousePressed(float x, float y, int button) override;

    vector<PinceBra> bras;
    float simWidth, simHeight;
    
    // Limite le nombre de bras comme dans la ref
    const int maxBras = 6;
};