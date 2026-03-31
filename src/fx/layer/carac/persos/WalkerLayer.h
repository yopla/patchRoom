#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

struct WalkerLeg {
    float rOffset;    // Décalage de phase pour l'animation
    float length;     // Longueur de la jambe
    float rMul;       // Facteur aléatoire de mouvement
    float currentH;   // Hauteur actuelle (pour calculer le rebond du corps)
};

class WalkerLayer : public BaseLayer {
public:
    void setup(float w, float h);
    
    // Implémentation de BaseLayer
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
    // Setters spécifiques
    void setScale(float s) { globalScale = s; }

private:
    void drawWalker(float offsetX, float offsetY);
    float getShortestDist(float current, float target, float w);

    float simWidth;
    float simHeight;
    float globalScale = 1.0f;

    // Physique Walker
    ofVec2f pos;        // Position (X = jambes, Y = calculé dynamiquement)
    // float bodyY;        // Hauteur du corps relative aux jambes
    float angleCycle;   // 'r' dans le code AS3 (cycle de marche)
    
    // Configuration
    vector<WalkerLeg> legs;
    // float walkerSpeed = 0.0f; // Vitesse calculée selon le déplacement
    
    // Visuals
    // float bodyW = 80;
    float bodyH = 60;
};