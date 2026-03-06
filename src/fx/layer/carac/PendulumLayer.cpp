#include "PendulumLayer.h"

//--------------------------------------------------------------
void PendulumLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    anchorPos.set(w * 0.5f, 0);
    
    int numNodes = 20;
    nodes.clear();
    
    // Création de la chaîne
    for(int i=0; i<numNodes; i++) {
        PendulumNode n;
        n.pos.set(anchorPos.x, i * segmentLength);
        n.prevPos = n.pos;
        nodes.push_back(n);
    }
}

//--------------------------------------------------------------
void PendulumLayer::update(float mouseX, float mouseY) {
    // 1. Intégration de Verlet (Physique)
    for(auto& n : nodes) {
        // Gravité
        n.pos.y += gravity;
        
        // Inertie : pos += (pos - prevPos)
        ofVec2f vel = n.pos - n.prevPos;
        n.prevPos = n.pos; // Sauvegarde position actuelle avant modif
        
        // Friction de l'air (0.99)
        n.pos += vel * 0.99f;
    }
    
    // 2. Résolution des contraintes (Distance fixe)
    for(int k=0; k<constraintIterations; k++) {
        solveConstraints(mouseX, mouseY);
    }
}

//--------------------------------------------------------------
void PendulumLayer::solveConstraints(float mouseX, float mouseY) {
    // A. Contrainte de distance entre les noeuds
    for(size_t i=0; i<nodes.size()-1; i++) {
        PendulumNode& n1 = nodes[i];
        PendulumNode& n2 = nodes[i+1];
        
        ofVec2f delta = n2.pos - n1.pos;
        float dist = delta.length();
        if(dist == 0) continue;
        
        float diff = (segmentLength - dist) / dist;
        
        // Répartition du déplacement (0.5 chacun)
        ofVec2f offset = delta * 0.5f * diff;
        
        n1.pos -= offset;
        n2.pos += offset;
    }
    
    // B. Point d'ancrage fixe (Plafond)
    if(!nodes.empty()) {
        nodes[0].pos = anchorPos;
    }
    
    // C. Interaction Souris (Drag)
    if(draggedNode) {
        draggedNode->pos.set(mouseX, mouseY);
        // On reset la vélocité pour éviter l'explosion quand on relâche
        draggedNode->prevPos = draggedNode->pos; 
    }
}

//--------------------------------------------------------------
void PendulumLayer::draw() {
    if(nodes.empty()) return;

    // 1. Dessin de la corde
    ofPushStyle();
    ofSetColor(255, 150);
    ofSetLineWidth(2);
    ofNoFill();
    ofBeginShape();
    for(auto& n : nodes) {
        ofVertex(n.pos);
    }
    ofEndShape();
    
    // 2. Dessin de la lumière (Halo) au bout
    const auto& tail = nodes.back();
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    // Cœur brillant
    ofSetColor(255, 255, 200, 255);
    ofDrawCircle(tail.pos, 15);
    
    // Halo interne
    ofSetColor(255, 200, 100, 100);
    ofDrawCircle(tail.pos, 40);
    
    // Halo externe
    ofSetColor(255, 100, 50, 50);
    ofDrawCircle(tail.pos, 80);
    
    // Grand halo diffus
    ofSetColor(100, 50, 255, 20);
    ofDrawCircle(tail.pos, 150);
    
    ofDisableBlendMode();
    ofPopStyle();
}

//--------------------------------------------------------------
void PendulumLayer::mousePressed(float x, float y) {
    // Trouver le noeud le plus proche
    float minDist = 100.0f;
    for(auto& n : nodes) {
        float d = n.pos.distance(ofVec2f(x, y));
        if(d < minDist) {
            minDist = d;
            draggedNode = &n;
        }
    }
}

//--------------------------------------------------------------
void PendulumLayer::mouseReleased(float x, float y) {
    draggedNode = nullptr;
}