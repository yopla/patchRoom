#pragma once
#include "ofMain.h"

class CloudRippleRing {
public:
    struct Ripple {
        float u, v;
        float life;
        float maxLife;
    };
    
    void setup(float r, ofVec3f centerPos);
    void update(float dt);
    void draw();

    // Ajoute une onde de déformation à l'endroit cliqué
    void addRipple(float u, float v);

    float radius;
    ofVec3f center; // Position centrale de la sphère

private:
    ofMesh mesh;
    vector<Ripple> ripples;
    
    static ofImage cloudTexture; 
    static bool bTextureAllocated;
    void allocateTexture();

    int resX;
    int resY;
    vector<ofVec2f> baseUVs; // Stocke les UV originaux pour calculer la déformation pure
};