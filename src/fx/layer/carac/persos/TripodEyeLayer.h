#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

// Structure représentant une patte avec Inverse Kinematics (IK)
struct TripodLeg {
    ofVec2f rootOffset;         // Décalage de l'attache par rapport au centre du corps
    ofVec2f footPos;            // Position actuelle du pied au sol
    ofVec2f footTarget;         // Cible du pied pendant un pas
    ofVec2f startPos;           // Point de départ du pas
    
    float L1, L2;               // Longueurs des deux segments (Cuisse et Mollet)
    bool isMoving;              // État du mouvement
    float moveProgress;         // Progression de l'animation du pas (0.0 à 1.0)
    float stepSpeed;            // Vitesse du pas
    
    ofVec2f kneePos;            // Position calculée de l'articulation (Genou)
    float bendDir;              // Sens de la pliure du genou (1 ou -1)
    float idealGroundOffsetX;   // Position idéale au sol relative au corps
};

// La créature "Oeil"
class TripodEye {
public:
    void setup(float x, float y);
    void update(float mx, float my, float groundY);
    void draw();

    ofVec2f bodyPos;
    ofVec2f bodyVel;
    ofVec2f lookTarget; // Où l'oeil regarde
    
    vector<TripodLeg> legs;
    int movingLegIndex; // Permet de s'assurer qu'une seule patte bouge à la fois
    
    float bodyRadius;
};

// Le layer (Wrapper) de la créature pour la scène
class TripodEyeLayer {
public:
    TripodEyeLayer() {}
    ~TripodEyeLayer() {}

    void setup(float w, float h, shared_ptr<ColliderLayer> col = nullptr);
    void update(float mx, float my, float time);
    void draw();

    TripodEye creature;
    
    float simWidth;
    float simHeight;
    shared_ptr<ColliderLayer> collider;
};