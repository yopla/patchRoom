#pragma once
#include "ofMain.h"

// Structure commune pour une patte
struct GekoLeg {
    glm::vec2 position;
    glm::vec2 target;
    bool isMoving = false;
    float moveProgress = 0.0f;
    float angleOffset = 0.0f;
};

// --- CLASSE DE BASE (Abstraite) ---
class GekoBase {
    friend class GekoManager;
public:
    GekoBase(float startX, float startY, int nLegs);
    virtual ~GekoBase() {}

    // Méthode virtuelle pure : chaque gecko doit définir son mouvement
    virtual void update(float mx, float my) = 0;
    
    void draw();
    void setLegCount(int n);

protected:
    // Outils pour les classes enfants
    void updateLegsInternal(float bodyAngleRad, float speedMult = 1.0f);
    void applyPhysics();

    // Propriétés
    glm::vec2 bodyPos;
    glm::vec2 bodyVel;
    float wanderAngle;
    
    // Paramètres
    float friction;
    float springStiffness;
    float bodySize;
    float legReach;
    float maxSpeed; // Pour la contrainte des 5 secondes

    // Pattes
    vector<GekoLeg> legs;
    int numLegs;
    int movingLegIndex;
};

// --- GEKO 1 : STANDARD (Mouvement fluide, patte par patte) ---
class GekoStandard : public GekoBase {
public:
    GekoStandard(float x, float y);
    void update(float mx, float my) override;
private:
    int nextLegToMove;
};

// --- GEKO 2 : AGILE (Mouvement réactif, saccadé) ---
class GekoAgile : public GekoBase {
public:
    GekoAgile(float x, float y);
    void update(float mx, float my) override;
};

// --- GEKO 3 : ORBITER (Tourne autour, mouvement perpétuel) ---
class GekoOrbiter : public GekoBase {
public:
    GekoOrbiter(float x, float y);
    void update(float mx, float my) override;
private:
    float orbitAngle;
    bool isOrbiting;
};