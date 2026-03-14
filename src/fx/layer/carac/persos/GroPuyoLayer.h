#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

struct GroPuyoNode {
    ofVec2f pos;
    ofVec2f oldPos;
};

class GroPuyo {
public:
    void setup(float x, float y);
    void update(float w, float h, shared_ptr<ColliderLayer> collider, float time);
    void draw();
    
    void solveConstraints();
    
    vector<GroPuyoNode> nodes;
    vector<float> restLengths;
    
    ofVec2f center;
    float currentRadius;
    float targetRadius;
    float phaseOffset;
    ofColor color;
};

class GroPuyoLayer {
public:
    void setup(float w, float h, float s, shared_ptr<ColliderLayer> col);
    void update(float mx, float my, float time);
    void draw();
    void addGroPuyo(float x, float y);
    
    vector<shared_ptr<GroPuyo>> puyos;
    shared_ptr<ColliderLayer> collider;
    float simWidth, simHeight, scale;
};