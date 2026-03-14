#pragma once
#include "ofMain.h"
#include "CousinCreature.h" // Includes CousinHair definition

class CousinSauteur {
public:
    CousinSauteur(float startX, float startY);
    ~CousinSauteur();

    void update(float mx, float my, float time);
    void draw();

    bool isInside(float mx, float my);

private:
    void updateState(float time);
    void updatePhysics();
    void updateBodyAnimation(); // Procedural animation logic

    // Physique
    ofVec2f pos;     // Position du pied (Base)
    ofVec2f vel;
    float groundY;   // Niveau du sol (fixé à la création)

    // Structure du corps (Monopode)
    int numSegments;
    vector<float> lengths;
    vector<float> thickness; // Pour le dessin du corps
    vector<ofVec2f> leftPoints, rightPoints; // Pour le contour
    vector<ofVec2f> joints; // Positions calculées

    // Etats
    enum State { IDLE, PREPARE, JUMPING, LANDING, RECOVER };
    State state;
    float stateTimer;
    float jumpDirection;
    
    // Animation lissée (Interpolation)
    float smoothHeightRatio;
    float smoothBend;
    float smoothArticulation;
    float smoothAirBend;

    // Visuel
    ofColor color;
    float uniqueOffset;
    
    // Poils
    vector<CousinHair> hairs;
    
    // Facteurs aléatoires pour la torsion
    vector<float> randFoldFactors;
    
    // Style de saut (Alternance)
    bool bFoldJump;
};
