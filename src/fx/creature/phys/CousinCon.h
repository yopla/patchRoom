#pragma once
#include "ofMain.h"
#include "CousinCreature.h" // Nécessaire pour réutiliser la classe CousinHair

class CousinCon {
public:
    // Constructeur prenant un pointeur vers l'image (concombre.jpg)
    CousinCon(float startX, float startY, ofImage* img);
    ~CousinCon();

    void update(float mx, float my);
    void draw();
    
    // Interactions basiques
    bool isInside(float mx, float my);
    void onPress(float mx, float my);
    void onRelease(float mx, float my);

private:
    ofImage* texture; // Pointeur vers la texture partagée

    // --- Paramètres du Corps (Similaire à CousinCreature) ---
    int numSegments;
    int numHairs;
    
    // Tableaux de données pour la simulation du corps
    vector<float> posx, posy;
    vector<float> angles;
    
    // Paramètres de forme
    vector<float> thickness;
    vector<float> lengths;
    vector<float> amplitudes;
    vector<float> freqs;
    vector<float> torsions;
    vector<float> phases;
    
    // Points calculés pour le maillage (Left/Right)
    vector<float> lx, ly;
    vector<float> rx, ry;
    vector<float> lmx, lmy;
    vector<float> rmx, rmy;

    // --- Gestion des Poils ---
    vector<CousinHair> hairs;
    float hairTargetX, hairTargetY;

    // --- Variables globales simulation ---
    float time;
    float speedFactor;
    float headX, headY;
    
    // Interaction
    bool isDragging;
    ofVec2f dragOffset;
    
    void updateBody();
    void updateHairs();
    void initArrays();
};