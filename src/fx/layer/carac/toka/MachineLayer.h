#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

// Particule physique simple (Verlet)
struct MParticle {
    ofVec2f pos, oldPos;
    ofVec2f force;
    float mass = 1.0f;
    bool bFixed = false; // Si true, la particule est contrôlée cinématiquement (moteur)
};

// Contrainte de distance (Ressort rigide)
struct MConstraint {
    int p1, p2;
    float length;
    float stiffness = 1.0f;
    bool bVisible = true;
};

class MachineLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    void mousePressed(float x, float y, int button) override;

    // Réinitialise la machine à une position donnée
    void createMachine(float x, float y);
    
    // Physique
    void solveVerlet();
    void solveConstraints();
    void checkFloor();

    vector<MParticle> particles;
    vector<MConstraint> constraints;
    
    float simWidth, simHeight;
    float floorY;
    
    // Moteur
    float motorAngle = 0;
    float motorSpeed = 0.2f;
    
    // Indices des parties clés pour l'animation
    int crankIdx;      // Le point qui tourne (Moteur)
    int bodyCenterIdx; // Le centre du corps
    
    // Helper pour ajouter des éléments
    int addParticle(float x, float y, bool fixed = false);
    void addConstraint(int i1, int i2, float stiff = 1.0f, bool visible = true);
};
