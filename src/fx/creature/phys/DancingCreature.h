#pragma once
#include "ofMain.h"

// Structure simple pour un point du corps
struct RagdollNode {
    ofVec2f pos;
    ofVec2f oldPos; // Pour la physique Verlet
    float mass = 1.0f;
    bool isLocked = false; // Pour la tête qui tient le tout
};

// Structure pour relier deux points (un os)
struct RagdollLimb {
    int nodeA;
    int nodeB;
    float length;
};

class DancingCreature {
public:
    DancingCreature(float x, float y);
    ~DancingCreature();

    void update(float mx, float my, float time);
    void draw();

    // Interaction
    bool isInside(float mx, float my);

private:
    void solveConstraints();
    void applySpasm(float intensity, float time);

    vector<RagdollNode> nodes;
    vector<RagdollLimb> limbs;

    // Position d'ancrage (comme le "Anchor" du script AS3)
    ofVec2f anchorPos;
    
    // Dimensions pour la hitbox
    float width, height;

    // Etats
    bool isHovering;
    float globalDamping;
    
    // Indices des noeuds pour dessiner facilement
    int HEAD, TORSO, PELVIS, L_ELBOW, L_HAND, R_ELBOW, R_HAND, L_KNEE, L_FOOT, R_KNEE, R_FOOT;
};