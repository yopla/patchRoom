#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

struct PuyoNode {
    ofVec2f pos;
    ofVec2f oldPos;
};

struct PuyoLink {
    int n1, n2;
    float length;
    float stiffness;
};

class Puyo {
public:
    void setup(float x, float y, float r, bool gravity);
    void update(float w, float h, shared_ptr<ColliderLayer> collider, float time);
    void draw(float scale);
    
    void solveConstraints();
    bool isFolded();
    
    vector<PuyoNode> nodes;
    vector<PuyoLink> links;
    
    ofVec2f center;
    float radius;
    bool bGravity;
    ofColor color;
    float noiseOffset;
    float foldedTimer;
};

class PuyoLayer {
public:
    void setup(float w, float h, float s, shared_ptr<ColliderLayer> collider);
    void update(float mx, float my, float time);
    void draw();
    
    void addPuyo(float x, float y);
    void getStats(int& folded, int& complete);
    
    vector<shared_ptr<Puyo>> puyos;
    shared_ptr<ColliderLayer> collider;
    
    float simWidth;
    float simHeight;
    float scale;
};
