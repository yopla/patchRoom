#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

class IKSegment {
public:
    float x, y;
    float angle; // radians
    float len;
    float width;
    
    IKSegment(float l, float w) : x(0), y(0), angle(0), len(l), width(w) {}
    
    ofVec2f getPin() {
        return ofVec2f(x + cos(angle) * len, y + sin(angle) * len);
    }
};

class IKChain {
public:
    vector<shared_ptr<IKSegment>> segments;
    
    void setup(float startX, float startY, int numSegs, float len, float w);
    void update(float targetX, float targetY);
    void draw();
    
private:
    ofVec2f reach(shared_ptr<IKSegment> seg, float x, float y);
    void position(shared_ptr<IKSegment> segA, shared_ptr<IKSegment> segB);
};

class ChainCrea : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
private:
    vector<IKChain> chains;
    float simWidth, simHeight;
};