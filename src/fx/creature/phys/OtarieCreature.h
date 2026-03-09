#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

class OtarieCreature {
public:
    struct Segment {
        ofVec2f pos;
        float angle; // Radians
        float width;
        float height;
        
        ofVec2f getPin() {
            return pos + ofVec2f(cos(angle) * width, sin(angle) * width);
        }
    };

    struct Ball {
        ofVec2f pos;
        ofVec2f vel;
        float radius;
    };

    OtarieCreature(float x, float y);
    
    void update(float mx, float my);
    void draw();
    void setCollider(shared_ptr<ColliderLayer> c);
    
    // Logic
    ofVec2f reach(Segment& seg, float x, float y);
    void position(Segment& segA, Segment& segB);
    void moveBall();
    void checkHit();
    
    vector<Segment> segments;
    Ball ball;
    
    ofVec2f basePos;
    int numSegments;
    float gravity;
    float bounce;
    shared_ptr<ColliderLayer> collider;
};