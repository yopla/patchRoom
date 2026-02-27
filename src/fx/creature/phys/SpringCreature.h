#pragma once
#include "ofMain.h"

// Structure interne pour les noeuds du ressort
struct SpringNode {
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
};

class SpringCreature {
public:
    // Constructeur
    SpringCreature(float _x, float _y);
    ~SpringCreature();

    // Méthodes standard du système
    void update(float mx, float my);
    void draw();
    
    // Interactions
    bool isInside(float mx, float my);
    void onPress(float mx, float my);
    void onRelease(float mx, float my);

    // Propriétés spatiales
    float x, y;
    float w, h; // Zone de détection approximative

private:
    // Logique interne (Portage AS3)
    void doSpring(SpringNode& a, SpringNode& b, float rest, float dt);
    float getBez4(float x0, float x1, float x2, float x3, float x4, float x5, float t);

    vector<SpringNode> nodes;
    int numNodes;
    bool isDragging;
    
    // Pour l'interaction souris locale
    float localMX, localMY;
};