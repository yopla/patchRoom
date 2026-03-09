#pragma once
#include "ofMain.h"

// Structure interne pour ne pas utiliser FireParticle.h
struct FireCParticle {
    float x, y;
    float sx, sy; // Scale
    float vx, vy;
    float life;
    float maxLife;
    float growth; // Vitesse de grossissement
    int delay;
};

class FireCLayer {
public:
    void setup(float w, float h);
    void update(float mx, float my);
    void draw();
    void mousePressed(float x, float y);

    // Méthode de tir inspirée de la ref AS3
    void fire(float x, float y, float tx, float ty, float life, float scale, int delay);

private:
    float width, height;
    
    // Particules
    vector<FireCParticle> particles;
    ofImage particleImg; // Sprite dégradé
    
    // Rendu et Displacement
    ofFbo bufferFbo;    // Buffer où on dessine les particules
    ofFbo noiseFbo;     // Texture de bruit pour le displacement
    ofShader displaceShader;
    
    float noiseScrollY;
    
    // État
    bool bPress;
    int frameCount;
    float bx, by; // Position émetteur
};