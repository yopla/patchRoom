#include "KrakenSystem.h"

//--------------------------------------------------------------
void KrakenTentacle::setup(float angle, float len, int n) {
    angleOffset = angle;
    length = len;
    numNodes = n;
    nodes.resize(numNodes);
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

//--------------------------------------------------------------
void KrakenTentacle::update(float time, ofVec3f headPos) {
    // Animation des noeuds du tentacule
    for(int i=0; i<numNodes; i++) {
        float pct = (float)i / (float)(numNodes-1);
        
        // Mouvement ondulatoire (Spirale + Vague)
        float wave = sin(time * 2.0f + i * 0.3f + angleOffset) * 0.5f;
        float waveVertical = cos(time * 1.5f + i * 0.2f) * 0.5f;
        
        // Forme "Coupe" pour entourer la room
        // Partie 1 (0.0 -> 0.3) : On s'éloigne du centre (Head) vers l'extérieur
        // Partie 2 (0.3 -> 1.0) : On monte autour de la room
        
        float r;
        float yOffset;
        
        if (pct < 0.3f) {
            // Expansion horizontale sous le sol
            float p = pct / 0.3f;
            r = p * 1800.0f; // Rayon cible > 1200 (demi-largeur room)
            yOffset = sin(p * PI) * 100.0f; 
        } else {
            // Montée verticale spiralaire autour de la room
            float p = (pct - 0.3f) / 0.7f;
            r = 1800.0f + sin(p * PI * 2.0f) * 100.0f; // Ondulation du rayon
            yOffset = p * 2500.0f; // Monte haut
        }

        float theta = angleOffset + wave * 0.3f * pct + (pct > 0.3f ? (pct-0.3f) * 0.5f : 0.0f);
        
        float x = headPos.x + cos(theta) * r;
        float z = headPos.z + sin(theta) * r;
        float y = headPos.y + yOffset + (waveVertical * 100.0f * pct);
        
        nodes[i].set(x, y, z);
    }
    
    // Génération du Mesh (Tube) autour de la colonne vertébrale (nodes)
    mesh.clear();
    int sides = 6; // Hexagone pour le tube (Low poly style)
    float baseRadius = 35.0f;
    
    for(int i=0; i<numNodes; i++) {
        float pct = (float)i / (float)(numNodes-1);
        float radius = baseRadius * (1.0f - pct); // S'affine vers le bout
        
        // Calcul du repère local (Frenet frame approximatif)
        ofVec3f forward;
        if(i < numNodes - 1) forward = (nodes[i+1] - nodes[i]).getNormalized();
        else forward = (nodes[i] - nodes[i-1]).getNormalized();
        
        ofVec3f right = forward.getCrossed(ofVec3f(0,1,0)).getNormalized();
        ofVec3f up = right.getCrossed(forward).getNormalized();
        
        for(int j=0; j<=sides; j++) {
            float ang = (float)j / (float)sides * TWO_PI;
            ofVec3f p = nodes[i] + (right * cos(ang) + up * sin(ang)) * radius;
            
            mesh.addVertex(p);
            // Couleur dégradée : Sombre vers Rouge/Violet
            mesh.addColor(ofColor(40 + pct*100, 0, 40 + (1-pct)*60)); 
        }
    }
    
    // Indices pour former les triangles
    for(int i=0; i<numNodes-1; i++) {
        for(int j=0; j<sides; j++) {
            int curr = i * (sides+1) + j;
            int next = curr + (sides+1);
            
            mesh.addIndex(curr); mesh.addIndex(next); mesh.addIndex(curr+1);
            mesh.addIndex(curr+1); mesh.addIndex(next); mesh.addIndex(next+1);
        }
    }
}

void KrakenTentacle::draw() {
    mesh.draw();
}

//--------------------------------------------------------------
void KrakenSystem::setup() {
    head.setRadius(300); // Noyau plus gros
    head.setResolution(24);
    
    int numTentacles = 8;
    for(int i=0; i<numTentacles; i++) {
        KrakenTentacle t;
        float angle = (TWO_PI / numTentacles) * i;
        t.setup(angle, 3000.0f, 60); // Plus long et plus de nodes pour la courbe
        tentacles.push_back(t);
    }
}

void KrakenSystem::start(float time) {
    startTime = time;
}

void KrakenSystem::update(float time) {
    // Animation d'émergence
    float animTime = time - startTime;
    float duration = 4.0f;
    float pct = ofClamp(animTime / duration, 0.0f, 1.0f);
    
    // Ease Out Cubic pour un surgissement dynamique
    pct = 1.0f - pow(1.0f - pct, 3.0f);
    
    // Monte de -2500 (sous l'eau) à -200 (juste sous la surface visible)
    float headY = ofMap(pct, 0.0f, 1.0f, -2500.0f, -200.0f) + sin(time * 0.5f) * 50.0f;
    
    head.setPosition(0, headY, 0);
    
    for(auto& t : tentacles) {
        t.update(time, head.getPosition());
    }
}

void KrakenSystem::draw() {
    ofPushStyle();
    ofSetColor(80, 20, 30); // Noyau rougeâtre
    head.draw();
    
    for(auto& t : tentacles) {
        t.draw();
    }
    ofPopStyle();
}