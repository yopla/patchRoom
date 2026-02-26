#include "DoublePendulumCreature.h"

//--------------------------------------------------------------
DoublePendulumCreature::DoublePendulumCreature(float x, float y) {
    cx = x;
    cy = y;
    
    // Configuration physique
    r1 = 120;
    r2 = 120;
    m1 = 10;
    m2 = 10;
    g = 1; // Gravité

    // Position de départ aléatoire (angles aléatoires)
    // Cela place les bras n'importe où "dans le rond" formé par leur longueur maximale
    a1 = ofRandom(0, TWO_PI);
    a2 = ofRandom(0, TWO_PI);
    
    a1_v = 0;
    a2_v = 0;
    
    isDragging = false;
}
//--------------------------------------------------------------
void DoublePendulumCreature::update(float mx, float my) {
    
    if (isDragging) {
        cx = mx - dragOffset.x;
        cy = my - dragOffset.y;
        // Quand on drag, on coupe la physique pour éviter que ça explose
        a1_v = 0;
        a2_v = 0;
    } else {
        // --- Calculs physiques (Lagrangien) ---
        float num1 = -g * (2 * m1 + m2) * sin(a1);
        float num2 = -m2 * g * sin(a1 - 2 * a2);
        float num3 = -2 * sin(a1 - a2) * m2;
        float num4 = a2_v * a2_v * r2 + a1_v * a1_v * r1 * cos(a1 - a2);
        float den = r1 * (2 * m1 + m2 - m2 * cos(2 * a1 - 2 * a2));
        
        float a1_a = (num1 + num2 + num3 * num4) / den;

        num1 = 2 * sin(a1 - a2);
        num2 = (a1_v * a1_v * r1 * (m1 + m2));
        num3 = g * (m1 + m2) * cos(a1);
        num4 = a2_v * a2_v * r2 * m2 * cos(a1 - a2);
        den = r2 * (2 * m1 + m2 - m2 * cos(2 * a1 - 2 * a2));
        
        float a2_a = (num1 * (num2 + num3 + num4)) / den;

        a1_v += a1_a;
        a2_v += a2_a;
        a1 += a1_v;
        a2 += a2_v;

        // Friction très légère pour éviter que ça devienne une hélice d'avion
        a1_v *= 0.999f;
        a2_v *= 0.999f;

        // --- AUTO-KICK (Mouvement Perpétuel) ---
        // Si la somme des vitesses est trop faible (il est presque à l'arrêt)
        // On lui donne une petite impulsion aléatoire.
        float totalEnergy = abs(a1_v) + abs(a2_v);
        
        if (totalEnergy < 0.05f) { // Seuil de "fatigue"
            // Petit coup de boost aléatoire
            a1_v += ofRandom(-0.08, 0.08); 
            a2_v += ofRandom(-0.08, 0.08);
        }
    }

    // --- Positions Cartésiennes ---
    x1 = cx + r1 * sin(a1);
    y1 = cy + r1 * cos(a1);

    x2 = x1 + r2 * sin(a2);
    y2 = y1 + r2 * cos(a2);

    // Traînée
    trail.addVertex(x2, y2);
    if (trail.size() > 100) { // Traînée plus longue pour voir le chaos
        trail.getVertices().erase(trail.getVertices().begin());
    }
}

//--------------------------------------------------------------
void DoublePendulumCreature::draw() {
    // Dessin de la traînée
    ofSetColor(100, 200, 255, 150);
    ofSetLineWidth(2);
    trail.draw();

    // Dessin des bras
    ofSetColor(255);
    ofSetLineWidth(3);
    ofDrawLine(cx, cy, x1, y1);
    ofDrawLine(x1, y1, x2, y2);

    // Dessin des masses (Noeuds)
    ofFill();
    ofSetColor(255, 100, 100); // Rouge pour l'ancrage
    ofDrawCircle(cx, cy, 8);

    ofSetColor(255);
    ofDrawCircle(x1, y1, m1); // Masse 1
    ofDrawCircle(x2, y2, m2); // Masse 2
}

//--------------------------------------------------------------
bool DoublePendulumCreature::isInside(float mx, float my) {
    // On considère qu'on clique sur le point d'ancrage pour le déplacer
    float dist = ofDist(mx, my, cx, cy);
    return (dist < 30);
}

void DoublePendulumCreature::onPress(float mx, float my) {
    if (isInside(mx, my)) {
        isDragging = true;
        dragOffset.set(mx - cx, my - cy);
        
        // Petit "coup" d'énergie quand on clique
        a1_v += ofRandom(-0.1, 0.1);
        a2_v += ofRandom(-0.1, 0.1);
    }
}

void DoublePendulumCreature::onRelease(float mx, float my) {
    isDragging = false;
}