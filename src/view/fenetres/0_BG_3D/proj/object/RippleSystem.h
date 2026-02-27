#pragma once
#include "ofMain.h"
#include "RoomWalls.h"

// Une structure simple pour représenter un projecteur individuel
struct RippleAgent {
    ofCamera projector;
    ofVec3f hitPoint;      // Point d'impact sur le mur
    ofColor color;         // Couleur du cercle
    
    float age;             // Temps écoulé
    float lifeSpan;        // Durée de vie (1.0 seconde)
    float maxRadius;       // Taille finale
    
    bool isDead() const { return age >= lifeSpan; }
};

class RippleSystem {
public:
    void setup(int count);
    void update(RoomWalls& walls); // On a besoin des murs pour recalculer les positions aléatoires
    void draw(RoomWalls& walls);   // On a besoin des murs pour dessiner la projection

private:
    vector<RippleAgent> agents;
    ofImage circleTexture; // La "diapositive" projetée (un cercle blanc)
    
    // Fonction utilitaire pour trouver un point aléatoire sur les murs
    void spawnAgent(RippleAgent& agent, RoomWalls& walls);

    
    // Génère la texture du cercle dynamiquement au démarrage
    void generateCircleTexture();
};