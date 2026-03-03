#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

struct BubbleNode {
    ofVec2f pos;
    ofVec2f oldPos;
};

struct BubbleLink {
    int n1, n2;
    float length;
    float stiffness;
};

class Bubble {
public:
    void setup(float x, float y, float r);
    void update(float w, float h, shared_ptr<ColliderLayer> collider, float yOffset_sim, float time);
    void draw();
    
    void solveConstraints();
    
    vector<BubbleNode> nodes;
    vector<BubbleLink> links;
    ofVec2f center;
    float radius;
    ofColor color;
    float noiseOffset;
};

class BubbleLayer {
public:
    void setup(float w, float h, float s, shared_ptr<ColliderLayer> col, float yOff = 0);
    void update(float mx, float my, float time);
    void draw();
    void addBubble(float x, float y);
    
    vector<shared_ptr<Bubble>> bubbles;
    float simWidth, simHeight;
    float scale;
    shared_ptr<ColliderLayer> collider;
    float colliderYOffset_sim = 0;
};
