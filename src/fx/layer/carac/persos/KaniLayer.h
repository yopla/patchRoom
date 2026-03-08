#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

struct KaniLeg {
    ofVec2f pos;        // Position actuelle du bout de la patte
    ofVec2f target;     // Cible du mouvement
    ofVec2f startPos;   // Position de départ du pas
    float offsetX;      // Décalage X par rapport au corps
    float dirY;         // 1.0 pour sol, -1.0 pour plafond
    
    bool isMoving;
    float moveProgress; // 0.0 à 1.0
    float moveSpeed;
    
    // Pour l'animation "mécanique" style Nemesis
    int gear; 
    bool grounded = false;
    float thresholdOffset = 0.0f; // Pour asymétriser le mouvement
};

class Kani {
public:
    void setup(float x, float y);
    void update(float targetX, float targetY, shared_ptr<ColliderLayer> collider, float time, float worldWidth);
    void draw(float offsetX, float offsetY, float worldWidth);

    ofVec2f pos;
    ofVec2f vel;
    
    vector<KaniLeg> legs;
    
    // Paramètres
    float bodySize = 30.0f;
    float legReachY = 800.0f; // Distance max de recherche de mur (augmentée)
    float stepThreshold = 60.0f; // Distance avant de faire un pas
    
    // Gestion des groupes de pattes
    int moveGroupSize = 1; // Nombre de pattes bougeant simultanément
    float moveDelay = 0.05f; // Délai entre les mouvements de groupes
    float lastMoveTime = 0.0f;
    
private:
    bool castRay(float x, float startY, float dir, shared_ptr<ColliderLayer> collider, float& outY);
    bool findBestSurface(float startX, float startY, float dir, shared_ptr<ColliderLayer> collider, float worldWidth, ofVec2f& outPoint);
};

class KaniLayer {
public:
    void setup(float w, float h, shared_ptr<ColliderLayer> collider);
    void update(float mx, float my, float time);
    void draw();

    shared_ptr<ColliderLayer> collider;
    Kani kani;
    
    float simWidth;
    float simHeight;
};