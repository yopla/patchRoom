#include "PinceBraLayer.h"

//--------------------------------------------------------------
// PINCE BRA (IK Chain)
//--------------------------------------------------------------
void PinceBra::setup(float x, float y, int numNodes) {
    basePos.set(x, y);
    nodes.resize(numNodes);
    
    // Initialisation verticale vers le bas
    for(int i=0; i<numNodes; i++) {
        nodes[i].pos.set(x, y + i * segmentLength);
        nodes[i].rot = 0;
    }
}

void PinceBra::update(float targetX, float targetY) {
    if(nodes.empty()) return;

    ofVec2f target(targetX, targetY);
    
    // Vitesse de rotation (plus élevé que 0.001 de la ref pour réactivité)
    float rotMax = 0.08f; 

    // --- INVERSE KINEMATICS (CCD) ---
    // On part de l'avant-dernier noeud et on remonte vers la base
    int endIdx = nodes.size() - 1;
    ofVec2f& endPos = nodes[endIdx].pos;
    
    for(int i = nodes.size() - 2; i >= 0; i--) {
        PinceNode& n = nodes[i];
        
        // Vecteur vers la cible
        ofVec2f tv = target - n.pos;
        // Vecteur vers l'effecteur (bout du bras)
        ofVec2f sv = endPos - n.pos;
        
        // Rotation de sv pour tester la direction (comme dans la ref AS3)
        float cosR = cos(rotMax);
        float sinR = sin(rotMax);
        
        // Rotation Droite (Clockwise dans repère écran Y-down)
        ofVec2f rv(sv.x * cosR - sv.y * sinR, sv.x * sinR + sv.y * cosR);
        // Rotation Gauche
        ofVec2f lv(sv.x * cosR + sv.y * sinR, -sv.x * sinR + sv.y * cosR);
        
        // Comparaison des produits scalaires pour savoir quel angle rapproche le plus
        float ds = tv.dot(sv);
        float dr = tv.dot(rv);
        float dl = tv.dot(lv);
        
        if (ds < dr && ds < dl) {
            // Déjà optimal ou bloqué
        } else if (dr > dl) {
            n.rot += rotMax;
        } else {
            n.rot -= rotMax;
        }
    }
    
    // --- FORWARD KINEMATICS ---
    // Application des angles pour positionner les segments
    nodes[0].pos = basePos;
    
    for(int i = 0; i < nodes.size() - 1; i++) {
        PinceNode& curr = nodes[i];
        PinceNode& next = nodes[i+1];
        
        // Dans la ref AS3: x = sin(rot), y = cos(rot) -> 0° est en bas (+Y)
        next.pos.x = curr.pos.x + segmentLength * sin(curr.rot);
        next.pos.y = curr.pos.y + segmentLength * cos(curr.rot);
    }
}

void PinceBra::draw() {
    ofSetLineWidth(3);
    ofSetColor(180, 160, 0); // Couleur Or/Bronze
    
    for(size_t i = 0; i < nodes.size() - 1; i++) {
        ofDrawLine(nodes[i].pos, nodes[i+1].pos);
        ofSetColor(220, 200, 0);
        ofDrawCircle(nodes[i].pos, 5);
        ofSetColor(180, 160, 0);
    }
    // Effecteur
    ofSetColor(255, 100, 0);
    ofDrawCircle(nodes.back().pos, 8);
}

//--------------------------------------------------------------
// LAYER IMPLEMENTATION
//--------------------------------------------------------------
void PinceBraLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // Ajout d'un bras par défaut
    PinceBra b;
    b.setup(w * 0.5f, h * 0.5f, 12);
    bras.push_back(b);
}

void PinceBraLayer::update(float mouseX, float mouseY, float time) {
    for(auto& b : bras) b.update(mouseX, mouseY);
}

void PinceBraLayer::draw() {
    for(auto& b : bras) b.draw();
}

void PinceBraLayer::mousePressed(float x, float y, int button) {
    PinceBra b;
    b.setup(x, y, (int)ofRandom(6, 15)); // Longueur aléatoire
    bras.push_back(b);
    
    if(bras.size() > maxBras) {
        bras.erase(bras.begin());
    }
}