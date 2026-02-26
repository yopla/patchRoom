#pragma once
#include "ofMain.h"

class DoublePendulumCreature {
public:
    DoublePendulumCreature(float x, float y);
    
    void update(float mx, float my);
    void draw();
    
    // Interactions
    bool isInside(float mx, float my);
    void onPress(float mx, float my);
    void onRelease(float mx, float my);

    // Position du point d'ancrage
    float cx, cy; 

private:
    // Paramètres physiques
    float r1, r2;      // Longueur des bras
    float m1, m2;      // Masses
    float a1, a2;      // Angles
    float a1_v, a2_v;  // Vélocité angulaire
    float g;           // Gravité locale
    
    // Positions calculées des masses
    float x1, y1;
    float x2, y2;

    // Traînée visuelle
    ofPolyline trail;
    
    // Interaction
    bool isDragging;
    ofVec2f dragOffset;
};