#include "PancarteLayer.h"

//--------------------------------------------------------------
void PancarteLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // Configuration inspirée de la ref AS3 mais adaptée à l'échelle de la scène
    float scale = 2.5f; 
    float boardW = 160 * scale;
    float boardH = 80 * scale;
    float chainLen = 8 * scale;
    int numChain = 16; // Nombre de maillons
    
    float centerX = w * 0.5f;
    float startY = h * 0.2f; // Position de départ en hauteur
    
    anchorPos.set(centerX, startY);
    
    vertices.clear();
    springs.clear();
    
    // --- Construction ---
    // On utilise des indices pour relier les ressorts
    
    // Points d'ancrage (indices 0 et numChain)
    // Dans la ref, il y a 2 chaines parallèles
    
    // Chaine Gauche
    float x1 = centerX - boardW * 0.5f;
    vertices.push_back(make_shared<PVtx>(x1, startY, 0)); // Fixe
    for(int i=1; i<numChain; i++) {
        vertices.push_back(make_shared<PVtx>(x1, startY + i * chainLen, 1.0f));
        springs.push_back(PSpr(i, i-1, chainLen, false, true));
    }
    
    // Chaine Droite
    float x2 = centerX + boardW * 0.5f;
    int offsetRight = vertices.size();
    vertices.push_back(make_shared<PVtx>(x2, startY, 0)); // Fixe
    for(int i=1; i<numChain; i++) {
        vertices.push_back(make_shared<PVtx>(x2, startY + i * chainLen, 1.0f));
        springs.push_back(PSpr(offsetRight + i, offsetRight + i - 1, chainLen, false, true));
    }
    
    // La Boite (Pancarte)
    // 4 coins reliés aux bas des chaines
    int lastLeft = numChain - 1;
    int lastRight = offsetRight + numChain - 1;
    
    float boxY = startY + numChain * chainLen;
    
    // Coins supérieurs de la boite
    int idxBoxTL = vertices.size(); vertices.push_back(make_shared<PVtx>(x1, boxY, 0.2f));
    int idxBoxTR = vertices.size(); vertices.push_back(make_shared<PVtx>(x2, boxY, 0.2f));
    
    // Coins inférieurs de la boite
    int idxBoxBL = vertices.size(); vertices.push_back(make_shared<PVtx>(x1, boxY + boardH, 0.2f));
    int idxBoxBR = vertices.size(); vertices.push_back(make_shared<PVtx>(x2, boxY + boardH, 0.2f));
    
    // Connexions Chaines -> Boite
    springs.push_back(PSpr(lastLeft, idxBoxTL, chainLen, false, true));
    springs.push_back(PSpr(lastRight, idxBoxTR, chainLen, false, true));
    
    // Structure de la Boite (Rigide)
    springs.push_back(PSpr(idxBoxTL, idxBoxTR, boardW, true, true)); // Haut
    springs.push_back(PSpr(idxBoxBL, idxBoxBR, boardW, true, true)); // Bas
    springs.push_back(PSpr(idxBoxTL, idxBoxBL, boardH, true, true)); // Gauche
    springs.push_back(PSpr(idxBoxTR, idxBoxBR, boardH, true, true)); // Droite
    
    // Diagonales pour rigidifier (invisibles)
    float diag = sqrt(boardW*boardW + boardH*boardH);
    springs.push_back(PSpr(idxBoxTL, idxBoxBR, diag, true, false));
    springs.push_back(PSpr(idxBoxTR, idxBoxBL, diag, true, false));
}

//--------------------------------------------------------------
void PancarteLayer::update(float mouseX, float mouseY, float time) {
    // Interaction Souris (Déplacement de l'ancrage)
    if(isDragging) {
        anchorPos.x += (mouseX - anchorPos.x) * 0.2f;
        anchorPos.y += (mouseY - anchorPos.y) * 0.2f;
    }
    
    // Mise à jour des points fixes (le haut des chaines)
    // On suppose que vertices[0] est TopLeftChain et vertices[numChain] est TopRightChain
    // Il faut retrouver l'index du début de la chaine droite.
    // Dans setup: Chaine gauche 0..numChain-1. Chaine droite commence juste après.
    // Mais attention, j'ai utilisé vertices.size() pour offsetRight.
    // Recalcul simple basé sur la structure connue:
    // Vtx 0 = Ancre Gauche
    // Vtx 16 = Ancre Droite (si numChain=16)
    
    float halfW = (160 * 2.5f) * 0.5f;
    if(vertices.size() > 0) {
        vertices[0]->pos.set(anchorPos.x - halfW, anchorPos.y);
        vertices[0]->vel.set(0,0); // Force velocity zero
    }
    // L'index de l'ancre droite dépend de numChain. 
    // Pour être sûr, on peut le stocker, mais ici on sait que c'est le 16ème (si numChain=16)
    int idxRightAnchor = 16; 
    if(vertices.size() > idxRightAnchor) {
        vertices[idxRightAnchor]->pos.set(anchorPos.x + halfW, anchorPos.y);
        vertices[idxRightAnchor]->vel.set(0,0);
    }

    // Physique
    for(auto& v : vertices) v->update();
    
    // Résolution des contraintes (plusieurs itérations pour la stabilité)
    for(int i=0; i<10; i++) {
        solveSprings();
    }
}

//--------------------------------------------------------------
void PancarteLayer::solveSprings() {
    for(auto& s : springs) {
        auto& v1 = vertices[s.i1];
        auto& v2 = vertices[s.i2];
        
        ofVec2f delta = v2->pos - v1->pos;
        float dist = delta.length();
        if(dist == 0) continue;
        
        // Calcul de la force élastique amortie (inspiré ref AS3)
        // m = 1 / (m1 + m2)
        float invMassSum = 1.0f / (v1->mass + v2->mass);
        
        // Amortissement basé sur la vitesse relative
        ofVec2f relVel = v2->vel - v1->vel;
        ofVec2f normal = delta / dist;
        float rvn = relVel.dot(normal);
        
        float force = invMassSum * (dist - s.restLength + rvn * 0.8f);
        
        if (!s.rigid && force < 0) force = 0; // Les chaines ne poussent pas, elles tirent seulement
        
        ofVec2f fVec = normal * force;
        
        if(v1->mass > 0) { v1->vel += fVec * v1->mass; v1->pos += fVec * v1->mass; } // Correction pos + vel pour stabilité
        if(v2->mass > 0) { v2->vel -= fVec * v2->mass; v2->pos -= fVec * v2->mass; }
    }
}

//--------------------------------------------------------------
void PancarteLayer::draw() {
    ofPushStyle();
    ofSetColor(255);
    ofSetLineWidth(2);
    
    for(auto& s : springs) {
        if(s.visible) {
            ofDrawLine(vertices[s.i1]->pos, vertices[s.i2]->pos);
        }
    }
    ofPopStyle();
}

void PancarteLayer::mousePressed(float x, float y, int button) { isDragging = true; }
void PancarteLayer::mouseReleased(float x, float y, int button) { isDragging = false; }