#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

struct KNode {
    float cx = 0;
    float cy = 0;
    float ang = 0;
    float vang = 0;
    
    float tx = 0;
    float ty = 0;
    float tang = 0;
    
    int dep = 0;
    KNode* parent = nullptr;
    
    KNode(KNode* p, float x_, float y_, float a_) {
        cx = x_;
        cy = y_;
        ang = a_;
        parent = p;
        if (parent != nullptr) {
            dep = parent->dep + 1;
        }
    }
};

class KundelichLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
    vector<shared_ptr<KNode>> vecNode;
    float simWidth, simHeight;
};