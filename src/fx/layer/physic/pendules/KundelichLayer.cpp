#include "KundelichLayer.h"

// Comparateur pour le tri par profondeur (comme dans la ref AS3)
bool compKNode(shared_ptr<KNode> a, shared_ptr<KNode> b) {
    return a->dep < b->dep;
}

//--------------------------------------------------------------
void KundelichLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    vecNode.clear();
    
    // Root node (Le premier noeud)
    auto root = make_shared<KNode>(nullptr, 0, 0, 0);
    root->vang = 0.01;
    vecNode.push_back(root);
    
    // Génération des 64 enfants
    for (int i = 0; i < 64; i++) {
        // Choix d'un parent aléatoire parmi les noeuds existants
        int parentIdx = (int)(ofRandom(0, vecNode.size()));
        KNode* p = vecNode[parentIdx].get();
        
        // Position relative aléatoire
        float x_ = (ofRandom(1.0) - 0.5) * 150;
        float y_ = (ofRandom(1.0) - 0.5) * 150;
        float a_ = ofRandom(1.0) * 6.28;
        
        auto node = make_shared<KNode>(p, x_, y_, a_);
        node->vang = (ofRandom(1.0) - 0.5) * 0.2;
        vecNode.push_back(node);
    }
    
    // Tri par profondeur pour l'ordre d'affichage
    sort(vecNode.begin(), vecNode.end(), compKNode);
}

//--------------------------------------------------------------
void KundelichLayer::update(float mouseX, float mouseY, float time) {
    if(vecNode.empty()) return;

    // Le noeud racine suit la souris pour l'interaction
    vecNode[0]->cx = mouseX;
    vecNode[0]->cy = mouseY;
    
    // Mise à jour de la cinématique
    for (auto& a : vecNode) {
        a->ang += a->vang;
        
        // Calculs locaux
        a->tx = a->cx;
        a->ty = a->cy;
        
        // Logique de la ref AS3 : tang = cos * sin
        // Cela crée une oscillation de l'angle transmis aux enfants
        a->tang = cos(a->ang) * sin(a->ang);
        
        if (a->parent != nullptr) {
            float kx = a->cx;
            float ky = a->cy;
            float ka = a->parent->tang;
            
            // Rotation de la position relative par l'angle "tang" du parent
            a->tx = cos(ka)*kx - sin(ka)*ky;
            a->ty = sin(ka)*kx + cos(ka)*ky;
            
            // Translation par la position du parent
            a->tx += a->parent->tx;
            a->ty += a->parent->ty;
            
            // Accumulation de l'angle
            a->tang += a->parent->tang;
        }
    }
}

//--------------------------------------------------------------
void KundelichLayer::draw() {
    ofPushStyle();
    ofSetColor(255); // Noir comme dans la ref
    ofSetLineWidth(2);
    
    for (auto& a : vecNode) {
        if (a->parent != nullptr) {
            ofDrawLine(a->tx, a->ty, a->parent->tx, a->parent->ty);
        }
    }
    ofPopStyle();
}