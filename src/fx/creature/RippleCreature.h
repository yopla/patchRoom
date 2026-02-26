#pragma once
#include "ofMain.h"

class RippleCreature {
public:
    // Propriétés spatiales
    float x, y, w, h;
    float rotation = 0.0f;
    
    // Images locales
    ofImage sourceImage;
    ofImage outputImage;
    
    // Buffers de simulation
    std::vector<int> buffer1;
    std::vector<int> buffer2;
    
    int cols, rows;
    
    // Paramètres
    float bufferScale;
    int rippleSize;

    
    // Constructeur / Destructeur
    RippleCreature(float _x, float _y, float _w, float _h, ofImage& _sharedSource);
    ~RippleCreature();
    
    // Méthodes
    void update();
    void draw();
    
    // Interaction
    bool isInside(float mx, float my);
    void checkInput(float mx, float my); // Vérifie si on doit créer une onde
    
private:
    void createRipple(int localX, int localY);
    void processRipples();
    void renderWater();
    
    // Pour détecter le mouvement de souris local
    float prevLocalX, prevLocalY;
};