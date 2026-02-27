#pragma once
#include "ofMain.h"

class WancoCreature {
public:
    WancoCreature(float startX, float startY, ofImage& sharedImg);
    ~WancoCreature();

    void update(float mx, float my);
    void draw();

    // Propriétés spatiales du "Chien" (Wanco)
    glm::vec3 pos;      // x, y (hauteur saut), z (profondeur)
    float scale;        // Échelle calculée par la perspective
    float rotation;     // Rotation vers la cible

    // Propriétés de la Cible (Papillon/Hae)
    glm::vec3 targetPos;
    glm::vec3 targetBase; // Point de base de la cible
    float targetAngle;    // Pour le mouvement circulaire du papillon

    // Paramètres de la "Camera" 2.5D
    float focus;
    float centerX, centerY;

    // Animation du saut
    float jumpTime;
    
    // Référence image
    ofImage* texture;
};