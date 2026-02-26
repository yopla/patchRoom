#include "DancingCreature.h"

//--------------------------------------------------------------
DancingCreature::DancingCreature(float x, float y) {
    anchorPos.set(x, y);
    width = 200; 
    height = 300;

    // Configuration Physique
    globalDamping = 0.95; // Un peu de friction pour pas que ça explose

    // --- 1. CREATION DES NOEUDS (Le Squelette) ---
    // On positionne le ragdoll en T-Pose approximative au départ
    
    // 0: Tête (Attachée au ciel comme dans le script AS3)
    nodes.push_back({ofVec2f(x, y), ofVec2f(x, y), 1.0f, true}); HEAD = 0;
    
    // 1: Torse
    nodes.push_back({ofVec2f(x, y + 50), ofVec2f(x, y + 50), 1.0f, false}); TORSO = 1;
    
    // 2: Bassin
    nodes.push_back({ofVec2f(x, y + 100), ofVec2f(x, y + 100), 1.2f, false}); PELVIS = 2;

    // Bras Gauche
    nodes.push_back({ofVec2f(x - 40, y + 50), ofVec2f(x - 40, y + 50), 0.5f, false}); L_ELBOW = 3;
    nodes.push_back({ofVec2f(x - 80, y + 70), ofVec2f(x - 80, y + 70), 0.5f, false}); L_HAND = 4;

    // Bras Droit
    nodes.push_back({ofVec2f(x + 40, y + 50), ofVec2f(x + 40, y + 50), 0.5f, false}); R_ELBOW = 5;
    nodes.push_back({ofVec2f(x + 80, y + 70), ofVec2f(x + 80, y + 70), 0.5f, false}); R_HAND = 6;

    // Jambe Gauche
    nodes.push_back({ofVec2f(x - 20, y + 150), ofVec2f(x - 20, y + 150), 0.8f, false}); L_KNEE = 7;
    nodes.push_back({ofVec2f(x - 25, y + 220), ofVec2f(x - 25, y + 220), 0.8f, false}); L_FOOT = 8;

    // Jambe Droite
    nodes.push_back({ofVec2f(x + 20, y + 150), ofVec2f(x + 20, y + 150), 0.8f, false}); R_KNEE = 9;
    nodes.push_back({ofVec2f(x + 25, y + 220), ofVec2f(x + 25, y + 220), 0.8f, false}); R_FOOT = 10;

    // --- 2. CREATION DES LIENS (Les Os) ---
    // Helper lambda pour ajouter un os
    auto addLimb = [&](int a, int b) {
        float dist = nodes[a].pos.distance(nodes[b].pos);
        limbs.push_back({a, b, dist});
    };

    addLimb(HEAD, TORSO);
    addLimb(TORSO, PELVIS);
    
    // Connexions Epaules (Torse -> Coudes)
    addLimb(TORSO, L_ELBOW);
    addLimb(L_ELBOW, L_HAND);
    addLimb(TORSO, R_ELBOW);
    addLimb(R_ELBOW, R_HAND);

    // Connexions Hanches (Bassin -> Genoux)
    addLimb(PELVIS, L_KNEE);
    addLimb(L_KNEE, L_FOOT);
    addLimb(PELVIS, R_KNEE);
    addLimb(R_KNEE, R_FOOT);
    
    // Connexion structurelle (pour éviter que le bassin ne tourne trop librement)
    // On ajoute un "ressort" invisible entre la tête et le bassin pour le maintien vertical (comme le anchor spring AS3)
    limbs.push_back({HEAD, PELVIS, 100.0f}); 
}

DancingCreature::~DancingCreature() {
    nodes.clear();
    limbs.clear();
}

//--------------------------------------------------------------
void DancingCreature::update(float mx, float my) {
    
    // 1. Detection Survol
    // On considère la zone autour de l'ancre
    isHovering = (mx > anchorPos.x - width/2 && mx < anchorPos.x + width/2 &&
                  my > anchorPos.y && my < anchorPos.y + height);

    // 2. Calcul de l'intensité du SPASME
    // Si survolé : grosse intensité (danse folle), sinon petit mouvement (respiration)
    float spasmForce = isHovering ? 8.5f : 0.8f;
    if(isHovering) globalDamping = 0.80f; // Plus d'énergie si survolé
    else globalDamping = 0.95f; // Plus calme sinon

    // 3. Physique (Verlet Integration)
    for (auto& n : nodes) {
        if (n.isLocked) continue; // La tête ne bouge pas (fixée au plafond)

        ofVec2f velocity = (n.pos - n.oldPos) * globalDamping;
        n.oldPos = n.pos;
        n.pos += velocity;

        // Gravité légère
        n.pos.y += 0.5f; 
    }

    // 4. Application du SPASME (Forces aléatoires type "noise")
    applySpasm(spasmForce);

    // 5. Résolution des contraintes (Garder les os solides)
    // On itère plusieurs fois pour rigidifier le corps
    for(int i=0; i<5; i++) {
        solveConstraints();
    }
}

//--------------------------------------------------------------
void DancingCreature::applySpasm(float intensity) {
    float fpsRec = 60.0f;
    float time = ofGetFrameNum() / fpsRec;
    
    for (int i = 0; i < nodes.size(); i++) {
        if(nodes[i].isLocked) continue;

        // On génère un bruit perlin rapide et différent pour chaque noeud
        // Le script AS3 avait une "restitution" élevée, ce qui crée du rebond.
        // Ici on simule ça par des impulsions chaotiques.
        
        float uniqueOffset = i * 13.5f; // Décale le bruit pour chaque membre
        float noiseX = ofSignedNoise(time * 10.0f + uniqueOffset); 
        float noiseY = ofSignedNoise(time * 12.0f + uniqueOffset + 100.0f);
        
        // On ajoute parfois un "kick" violent (le spasme)
        if(ofRandom(1.0) > 0.95) {
            noiseX *= 3.0;
            noiseY *= 3.0;
        }

        nodes[i].pos.x += noiseX * intensity;
        nodes[i].pos.y += noiseY * intensity;
    }
}

//--------------------------------------------------------------
void DancingCreature::solveConstraints() {
    for (auto& limb : limbs) {
        RagdollNode& n1 = nodes[limb.nodeA];
        RagdollNode& n2 = nodes[limb.nodeB];

        ofVec2f delta = n1.pos - n2.pos;
        float currentDist = delta.length();
        
        if (currentDist == 0) continue; 

        float difference = (currentDist - limb.length) / currentDist;
        
        // On repousse les noeuds pour respecter la longueur de l'os
        ofVec2f translate = delta * 0.5f * difference;
        
        if (!n1.isLocked) n1.pos -= translate;
        if (!n2.isLocked) n2.pos += translate;
        
        // Si un noeud est bloqué (la tête), l'autre prend tout le déplacement
        if (n1.isLocked) n2.pos += translate; 
        if (n2.isLocked) n1.pos -= translate;
    }
    
    // Contrainte Sol (optionnel, si tu veux qu'il touche le sol)
    // float floorY = anchorPos.y + height;
    // for(auto& n : nodes) {
    //    if(n.pos.y > floorY) { n.pos.y = floorY; n.oldPos.y = n.pos.y; }
    // }
}

//--------------------------------------------------------------
void DancingCreature::draw() {
    ofPushStyle();
    
    // Couleur changeante selon l'état (Visual feedback)
    if(isHovering) ofSetColor(255, 100, 100); // Rouge excitation
    else ofSetColor(220); // Blanc cassé calme

    ofSetLineWidth(3);

    // 1. Dessiner les membres
    for (auto& limb : limbs) {
        // Astuce : ne pas dessiner le ressort invisible de structure (le dernier)
        if(limb.length > 80 && limb.nodeB == PELVIS) continue; 
        
        ofDrawLine(nodes[limb.nodeA].pos, nodes[limb.nodeB].pos);
    }

    // 2. Dessiner les jointures
    ofFill();
    for (auto& n : nodes) {
        float r = (n.isLocked) ? 8 : 4; // Tête plus grosse
        ofDrawCircle(n.pos, r);
    }
    
    // Tête spécifique (Cercle creux autour du point d'ancrage)
    ofNoFill();
    ofDrawCircle(nodes[HEAD].pos, 20); // Grosse tête
    
    ofPopStyle();
}

//--------------------------------------------------------------
bool DancingCreature::isInside(float mx, float my) {
    return isHovering;
}