#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

class PinceFoireLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
    void mousePressed(float x, float y, int button) override;
    void mouseReleased(float x, float y, int button) override;

private:
    float simWidth;
    float simHeight;

    // Claw properties
    ofVec2f clawPos;
    ofVec2f clawVel;
    float clawJawAngle; // 0 = open, 45 = closed
    bool isMouseDown;
    
    // Physics for prizes
    struct Prize {
        ofVec2f pos;
        ofVec2f vel;
        float radius;
        ofColor color;
        bool isGrabbed;
        int id;
    };
    vector<Prize> prizes;
    
    void updatePrize(Prize& p);
    void checkGrab(Prize& p);
    void resolveCollisions();
};