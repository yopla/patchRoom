#pragma once
#include "ofMain.h"

struct Shard {
    ofMesh mesh;
    ofVec2f pos;
    ofVec2f vel;
    float angle;
    float angVel;
    
    void setup(ofVec2f _pos, ofVec2f _vel, ofMesh _mesh) {
        pos = _pos;
        vel = _vel;
        mesh = _mesh;
        angle = 0;
        angVel = ofRandom(-15, 15);
        
        // Recenter mesh vertices around (0,0) relative to the shard position
        auto centroid = mesh.getCentroid();
        auto& verts = mesh.getVertices();
        for(auto& v : verts) {
            v -= centroid;
        }
        // Move the shard position to the centroid
        pos.x += centroid.x;
        pos.y += centroid.y;
    }
    
    void update() {
        vel.y += 0.6; // Gravity
        pos += vel;
        angle += angVel;
    }
    
    void draw(ofImage* tex) {
        ofPushMatrix();
        ofTranslate(pos);
        ofRotateDeg(angle);
        if(tex) tex->bind();
        mesh.draw();
        if(tex) tex->unbind();
        ofPopMatrix();
    }
};

class BreakableCreature {
public:
    BreakableCreature(float x, float y, float w, float h, ofImage* img);
    
    void update(float mx, float my);
    void draw();
    void onPress(float mx, float my);
    void onRelease(float mx, float my);
    bool isInside(float mx, float my);
    
    ofVec2f pos;
    float width, height;
    ofImage* texture;
    
    bool bBroken;
    vector<Shard> shards;
    
    void breakApart(float impactX, float impactY);
    bool isFinished() { return bBroken && shards.empty(); }
};