#pragma once
#include "ofMain.h"
#include "LayerSystem.h" // Inclure le fichier créé au dessus

// Une classe interne pour chaque segment de tentacule
struct Limb {
    float x, y;      // Position actuelle
    float tx, ty;    // Position cible (Target)
    float size;      // Épaisseur
    
    // Pour le bruit
    float noiseOffset; 
    
    void setup(float startX, float startY, float s);
};

// Une classe interne pour un tentacule entier
class Tentacle {
public:
    void setup(float x, float y, float angleOffset, float lengthScale);
    void update(float headX, float headY, float simWidth, float simHeight);
void draw(float offsetX, float offsetY, ofColor col, float extraWidth, float simWidth);

    vector<Limb> segments;
    float angleOffset; // Angle de départ par rapport à la tête
    float stretchMult = 6.0f;
};

class PoulpeLayer : public BaseLayer {
public:
    void setup(float width, float height);
    void update(float mouseX, float mouseY) override {
        if (!bExternalControl) {
            setTarget(mouseX, mouseY); // On utilise les coords passées par le manager
        }
        update(); // Appel de ta logique interne
        bExternalControl = false;
    };
    
    void update(); // Ta méthode interne
    void draw() override;
    
    // Permet de définir une cible manuellement (ex: la souris transformée de Scene2D)
    void setTarget(float x, float y);

    ofVec2f getHeadPos() const { return headPos; }
    bool bExternalControl = false;

private:
    float simWidth;
    float simHeight;
    
    // Physique de la tête
    ofVec2f headPos;
    ofVec2f currentPos; // Position lissée
    float easing = 0.08f; // Vitesse de suivi

    // Cible
    float targetX, targetY;

    // Le poulpe a plusieurs tentacules
    vector<Tentacle> tentacles;
    
    // Méthode utilitaire pour le plus court chemin sur un tore
    float getShortestDist(float p1, float p2, float w);
};