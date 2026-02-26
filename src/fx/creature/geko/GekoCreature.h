#pragma once
#include "ofMain.h"

struct GekoLeg {
    glm::vec2 position;      // Position actuelle (au sol)
    glm::vec2 target;        // Où la patte veut aller
    bool isMoving;           // Est-ce qu'elle est en l'air ?
    float moveProgress;      // 0.0 à 1.0
    float angleOffset;       // Angle "natif" de la patte autour du corps
};

class GekoCreature {
public:
    // Constructeur avec nombre de pattes configurable (défaut = 3)
    GekoCreature(float startX, float startY, int nLegs = 3);
    
    void update(float mx, float my);
    void draw();
    
    // Changement dynamique
    void setLegCount(int n);

    // Physique du corps
    glm::vec2 bodyPos;
    glm::vec2 bodyVel;
    float friction;
    float springStiffness;

    // Gestion des pattes
    vector<GekoLeg> legs;
    int numLegs;            // Variable pour le nombre de pattes
    int movingLegIndex;     // Quelle patte bouge (-1 si aucune)
    
    // Logique de "Flânerie" (Première version)
    float wanderAngle;       // Angle pour tourner autour de la souris
    glm::vec2 currentTarget; // Le point virtuel que le corps suit

    // Paramètres visuels
    float bodySize;
    float legReach;
};