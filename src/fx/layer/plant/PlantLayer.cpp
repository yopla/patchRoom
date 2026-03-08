#include "PlantLayer.h"

void PlantStem::setup(float x, float y, float h) {
    height = h;
    noiseOffset = ofRandom(1000);
    // Variations de vert organique
    color.set(20 + ofRandom(40), 100 + ofRandom(100), 40 + ofRandom(40), 220);
    thickness = ofRandom(2, 5);
    currentPush = 0.0f;

    int numPoints = 15;
    float segLen = height / numPoints;
    for(int i=0; i<numPoints; i++) {
        nodes.push_back(ofVec2f(x, y - i * segLen));
    }
}

void PlantStem::update(float time, float mx, float my) {
    // Vent ondulant (Perlin Noise)
    float wind = ofSignedNoise(time * 0.8 + noiseOffset) * 0.25;
    
    // Interaction Souris (Repoussoir)
    float dist = ofDist(nodes[0].x, nodes[0].y, mx, my);
    float targetPush = 0;
    if(dist < 250) {
        // Plus on est proche, plus ça pousse
        float force = ofMap(dist, 0, 250, 0.8, 0);
        // Direction de la poussée (gauche ou droite par rapport à la tige)
        float dir = (mx > nodes[0].x) ? -1.0 : 1.0;
        targetPush = dir * force;
    }
    
    currentPush = ofLerp(currentPush, targetPush, 0.1f);

    // Cinématique Inverse simplifiée (Propagation de l'angle)
    for(int i=1; i<nodes.size(); i++) {
        float t = i / (float)nodes.size(); // 0 à la base, 1 au sommet
        
        // L'angle dévie de la verticale (-PI/2)
        // Le vent et la poussée affectent plus le sommet (t*t)
        float angleOffset = (wind + currentPush) * (t * t);
        float angle = -HALF_PI + angleOffset;
        
        float segLen = height / nodes.size();
        
        // Calcul de la nouvelle position relative au noeud précédent
        nodes[i].x = nodes[i-1].x + cos(angle) * segLen;
        nodes[i].y = nodes[i-1].y + sin(angle) * segLen;
    }
}

void PlantStem::draw() {
    ofSetColor(color);
    ofSetLineWidth(thickness);
    ofNoFill();
    
    ofBeginShape();
    for(auto& n : nodes) ofVertex(n);
    ofEndShape();
    
    // Petits cercles pour donner du corps aux jointures
    ofFill();
    for(int i=0; i<nodes.size(); i+=2) {
        float r = ofMap(i, 0, nodes.size(), thickness * 1.5, 1.0);
        ofDrawCircle(nodes[i], r);
    }
}

void PlantLayer::setup(float w, float h) {
    stems.clear();
    // Densité des plantes (une tous les 25 pixels environ)
    int numPlants = (int)(w / 25.0); 
    
    for(int i=0; i<numPlants; i++) {
        PlantStem s;
        float x = ofRandom(w);
        // Légère variation de Y pour la profondeur
        float y = h + ofRandom(-10, 10); 
        float hPlant = ofRandom(150, 450); // Hauteur variable
        
        s.setup(x, y, hPlant);
        stems.push_back(s);
    }
}

void PlantLayer::update(float mx, float my, float time) {
    for(auto& s : stems) {
        s.update(time, mx, my);
    }
}

void PlantLayer::draw() {
    for(auto& s : stems) {
        s.draw();
    }
}