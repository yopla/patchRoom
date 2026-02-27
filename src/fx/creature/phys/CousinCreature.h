#pragma once
#include "ofMain.h"

// Structure interne pour un segment de poil (IK)
struct HairSegment {
    float x, y;
    float angle;
    float length;
};

// Structure pour un poil entier (TailLine / IKline)
class CousinHair {
public:
    void setup(float len, int numSegs);
    void update(float targetX, float targetY, float time);
    void draw();

    vector<HairSegment> segments;
    float friction;
    float gravity;
    ofColor color;
    float offsetX, offsetY; // Décalage pour l'écartement
    float uniqueOffset;     // Pour le bruit individuel

    int attIndex = 0;
    float attPct = 0.0f;
    bool attSide = true;
};

class CousinCreature {
public:
    CousinCreature(float startX, float startY);
    ~CousinCreature();

    void update(float mx, float my);
    void draw();
    
    // Interactions basiques
    bool isInside(float mx, float my);
    void onPress(float mx, float my);
    void onRelease(float mx, float my);

private:
    // --- Paramètres du Corps (corp.as / glob.as) ---
    int numSegments;
    int numHairs; // Variable pour le nombre de poils
    
    // Tableaux de données pour la simulation du corps
    vector<float> posx, posy;
    vector<float> angles; // a
    
    // Paramètres de forme
    vector<float> thickness; // epais1
    vector<float> lengths;   // hauteur1
    vector<float> amplitudes; // qte1
    vector<float> freqs;      // frec1
    vector<float> torsions;   // tors
    vector<float> phases;     // depart
    
    // Points calculés pour le dessin (Left/Right)
    vector<float> lx, ly;
    vector<float> rx, ry;
    vector<float> lmx, lmy; // Midpoints Left
    vector<float> rmx, rmy; // Midpoints Right

    // --- Gestion des Poils (TailLine.as) ---
    vector<CousinHair> hairs;
    float hairTargetX, hairTargetY; // Point d'attache (origine_poils)

    // --- Variables globales simulation ---
    float time;
    float speedFactor;
    float headX, headY;
    
    // Interaction
    bool isDragging;
    ofVec2f dragOffset;
    
    // Helpers
    void updateBody();
    void updateHairs();
    
    // Helper pour dessiner une courbe quadratique via Bezier cubique (pour imiter curveTo de Flash)
    void drawQuadraticBezier(float x0, float y0, float cx, float cy, float x1, float y1);
    
    // Helper pour initialiser les vecteurs
    void initArrays();
};