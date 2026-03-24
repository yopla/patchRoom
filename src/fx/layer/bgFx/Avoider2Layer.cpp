#include "Avoider2Layer.h"

void Avoider2Layer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // Initialisation inspirée par le AS3 (spawn aléatoire)
    int numAvoiders = 15;
    for(int i = 0; i < numAvoiders; i++) {
        Avoider2Entity a;
        a.pos.set(ofRandom(w), ofRandom(h));
        a.vel.set(ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f));
        a.vel.normalize();
        a.vel *= ofRandom(1.0f, 3.0f);
        a.radius = 10.0f; // Comme R_BULLET dans l'AS3 original
        a.color = ofColor(238, 238, 238); // Couleur gris clair
        avoiders.push_back(a);
    }
}

void Avoider2Layer::update(float time) {
    for(auto& a : avoiders) {
        // Déplacement de base (l'évitement complet sera injecté ici plus tard)
        // Ajout d'un léger mouvement brownien fluide pour donner de la vie
        float noiseX = ofSignedNoise(a.pos.x * 0.005f, time * 0.5f);
        float noiseY = ofSignedNoise(time * 0.5f, a.pos.y * 0.005f);
        
        a.vel.x += noiseX * 0.2f;
        a.vel.y += noiseY * 0.2f;
        
        // Limite de vitesse (friction/amortissement global)
        if(a.vel.lengthSquared() > 16.0f) {
            a.vel.normalize();
            a.vel *= 4.0f;
        }
        
        // --- AJOUT : Évitement du collider ---
        if (collider) {
            // On anticipe la position future pour éviter de rentrer dans le mur
            ofVec2f nextPos = a.pos + a.vel * 2.0f;
            float simX = nextPos.x / scale;
            float simY = nextPos.y / scale;
            
            // On vérifie qu'on est dans les limites verticales du collider
            if (simY >= 0 && simY < collider->simHeight) {
                if (collider->isWall(simX, simY)) {
                    // On dévie la trajectoire (rebond sur l'axe concerné)
                    if (collider->isWall(a.pos.x / scale, simY)) a.vel.y *= -0.8f;
                    if (collider->isWall(simX, a.pos.y / scale)) a.vel.x *= -0.8f;
                    
                    // Légère poussée aléatoire pour éviter un blocage parfait
                    a.vel += ofVec2f(ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f)) * 0.5f;
                }
            }
        }
        
        a.pos += a.vel;
        
        // Wrapping pour cycler la scène de gauche à droite
        if(a.pos.x > simWidth) a.pos.x -= simWidth;
        else if(a.pos.x < 0) a.pos.x += simWidth;
        
        if(a.pos.y > simHeight) a.pos.y -= simHeight;
        else if(a.pos.y < 0) a.pos.y += simHeight;
    }
}

void Avoider2Layer::draw() {
    ofPushStyle();
    for(auto& a : avoiders) {
        ofSetColor(a.color, 200);
        ofFill();
        ofDrawCircle(a.pos, a.radius);
        
        // Petit reflet intérieur (imite le bevel d'origine)
        ofSetColor(255);
        ofDrawCircle(a.pos.x - 2, a.pos.y - 2, a.radius * 0.3f);
        
        // Dessin du fantôme pour un wrapping visuel fluide sur les bords horizontaux
        if(a.pos.x < a.radius) {
            ofSetColor(a.color, 200);
            ofDrawCircle(a.pos.x + simWidth, a.pos.y, a.radius);
        } else if(a.pos.x > simWidth - a.radius) {
            ofSetColor(a.color, 200);
            ofDrawCircle(a.pos.x - simWidth, a.pos.y, a.radius);
        }
    }
    ofPopStyle();
}