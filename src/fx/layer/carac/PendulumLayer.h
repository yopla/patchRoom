#pragma once
#include "ofMain.h"

struct PendulumNode {
    ofVec2f pos;
    ofVec2f prevPos;
};

class PendulumLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY);
    void draw();
    
    void mousePressed(float x, float y);
    void mouseReleased(float x, float y);
    
private:
    float simWidth, simHeight;
    
    vector<PendulumNode> nodes;
    PendulumNode* draggedNode = nullptr;
    
    ofVec2f anchorPos;
    float gravity = 0.8f;
    float segmentLength = 30.0f;
    int constraintIterations = 10; // "Hardness" dans la ref
    
    void solveConstraints(float mouseX, float mouseY);
};