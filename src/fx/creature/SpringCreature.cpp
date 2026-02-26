#include "SpringCreature.h"

//--------------------------------------------------------------
SpringCreature::SpringCreature(float _x, float _y) {
    x = _x;
    y = _y;
    w = 400; // Zone d'interaction par défaut
    h = 400;
    
    isDragging = false;
    numNodes = 6; // Comme dans le code AS3 original

    // Initialisation des noeuds
    for(int i = 0; i < numNodes; i++) {
        SpringNode node;
        // Position initiale relative (centrée horizontalement)
        node.x = w / 2.0f + (i * 3); 
        node.y = 50 + (i * 8); 
        nodes.push_back(node);
    }
}

//--------------------------------------------------------------
SpringCreature::~SpringCreature() {
    nodes.clear();
}

//--------------------------------------------------------------
void SpringCreature::update(float mx, float my) {
    // Conversion en coordonnées locales pour la simulation
    localMX = mx - x;
    localMY = my - y;

    // 1. Gestion de la tête (Fixe ou animée)
    // Dans l'AS3 original: a.cx = 230; a.cy=100;
    // Ici on fixe le premier noeud en haut au centre de la créature
    nodes[0].x = w / 2.0f;
    nodes[0].y = 50; 
    nodes[0].vx = 0;
    nodes[0].vy = 0;

    // 2. Gestion de la queue (Interaction Souris)
    // Si on clique, le dernier noeud suit la souris
    if (isDragging) {
        SpringNode& tail = nodes[nodes.size() - 1];
        tail.x = localMX;
        tail.y = localMY;
        tail.vx = 0;
        tail.vy = 0;
    }

    // 3. Physique (Springs)
    // Le code AS3 fait une boucle k=0 à 10 pour stabiliser la simulation
    for (int k = 0; k < 10; k++) {
        for (int i = 1; i < numNodes; i++) {
            doSpring(nodes[i], nodes[i-1], 16, 10); // rest=16, dt=10 (valeurs AS3)
            
            // Damping et Gravité
            nodes[i].vx *= 0.999f;
            nodes[i].vy *= 0.999f;
            nodes[i].vy += 0.2f * 0.1f; // Gravité

            // Application de la vélocité
            nodes[i].x += nodes[i].vx;
            nodes[i].y += nodes[i].vy;
        }
    }
}

//--------------------------------------------------------------
void SpringCreature::draw() {
    ofPushMatrix();
    ofTranslate(x, y); // On dessine par rapport à la position de la créature

    // Dessin des noeuds (Debug visuel optionnel, comme les ronds rouges dans l'original)
    ofSetColor(255);
    /*
    for(auto& n : nodes){
        ofDrawCircle(n.x, n.y, 4);
    }
    */

    // Dessin de la courbe de Bézier (Algorithme getBez4)
    ofNoFill();
    ofSetLineWidth(2);
    ofBeginShape();

    // On s'assure d'avoir assez de noeuds pour l'algo (il en faut 6)
    if(nodes.size() >= 6) {
        float step = 0.05f;
        for (float t = 0; t <= 1.0f; t += step) {
            float ax = getBez4(nodes[0].x, nodes[1].x, nodes[2].x, nodes[3].x, nodes[4].x, nodes[5].x, t);
            float ay = getBez4(nodes[0].y, nodes[1].y, nodes[2].y, nodes[3].y, nodes[4].y, nodes[5].y, t);
            ofVertex(ax, ay);
        }
    }
    
    ofEndShape();
    ofPopMatrix();
}

//--------------------------------------------------------------
// Algorithme "DoSpring" traduit de l'AS3
void SpringCreature::doSpring(SpringNode& a, SpringNode& b, float rest, float dt) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float mag = sqrt(dx*dx + dy*dy);

    if (mag == 0) mag = 0.00000001f;

    float nx = dx / mag;
    float ny = dy / mag;
    
    float ax = a.vx - b.vx;
    float ay = a.vy - b.vy;

    // Formule d'impulsion du code original
    float imp = (((ax*nx + ay*ny) + ((mag - rest)/dt)) / 2.0f) * 0.5f;

    a.vx += imp * (nx * -0.5f);
    a.vy += imp * (ny * -0.5f);
    b.vx += imp * (nx * 0.5f);
    b.vy += imp * (ny * 0.5f);
}

//--------------------------------------------------------------
// Algorithme "GetBez4" traduit de l'AS3 (Interpolation manuelle)
float SpringCreature::getBez4(float x0, float x1, float x2, float x3, float x4, float x5, float t) {
    float k0, k1, k2, k3, k4;
    
    k0 = x0 + (x1 - x0) * t;
    k1 = x1 + (x2 - x1) * t;
    k2 = x2 + (x3 - x2) * t;
    k3 = x3 + (x4 - x3) * t;
    k4 = x4 + (x5 - x4) * t;
    
    k0 = k0 + (k1 - k0) * t;
    k1 = k1 + (k2 - k1) * t;
    k2 = k2 + (k3 - k2) * t;
    k3 = k3 + (k4 - k3) * t;
    
    k0 = k0 + (k1 - k0) * t;
    k1 = k1 + (k2 - k1) * t;
    k2 = k2 + (k3 - k2) * t;
    
    k0 = k0 + (k1 - k0) * t;
    k1 = k1 + (k2 - k1) * t;
    
    return k0 + (k1 - k0) * t;
}

//--------------------------------------------------------------
// Interactions
//--------------------------------------------------------------
bool SpringCreature::isInside(float mx, float my) {
    // Zone rectangulaire simple autour de la créature
    return (mx >= x && mx < x + w && my >= y && my < y + h);
}

void SpringCreature::onPress(float mx, float my) {
    if (isInside(mx, my)) {
        isDragging = true;
    }
}

void SpringCreature::onRelease(float mx, float my) {
    isDragging = false;
}