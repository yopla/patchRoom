#include "SlimeLayer.h"

//--------------------------------------------------------------
void SlimeLayer::setup(float w, float h) {
    width = w;
    height = h;
    gravity = 0.45f; 
    friction = 0.96f; 
    
    // Config pour le LayerManager
    name = "Slime";
    toggleKey = 'm'; // Touche par défaut
}

//--------------------------------------------------------------
// <--- 4. Adaptation de la signature update
void SlimeLayer::update(float mouseX, float mouseY, float time) {
    
    // GESTION INTERNE DE L'INTERACTION (Optionnel mais pratique)
    // Si on clique gauche, on verse du slime
    if (ofGetMousePressed(0)) {
        // Correction : Utilisation directe des coordonnées souris (déjà transformées)
        // On vérifie qu'on est bien dans la zone du slime avant de verser
        if (mouseY >= 0 && mouseY <= height) {
             pour(mouseX, mouseY, ofRandom(-2, 2), 5.0); 
        }
    }

    // --- PHYSIQUE EXISTANTE (INCHANGÉE) ---
    for (int i = particles.size() - 1; i >= 0; i--) {
        SlimeParticle& p = particles[i];

        // 1. Physique
        p.vel.y += gravity;
        p.vel *= friction;

        // Prédiction
        float nextX = p.pos.x + p.vel.x;
        float nextY = p.pos.y + p.vel.y;
        
        bool hit = false;

       float simX = nextX / scale;
       float simY = p.pos.y / scale; // Note: j'ai corrigé p.pos.y qui traînait dans ton code original si besoin

        // 1. GESTION DU BOUCLAGE (LOOP)
        if (nextX < 0) {
            p.pos.x = width + nextX; 
        } 
        else if (nextX > width) {
            p.pos.x = nextX - width;
        }
        // 2. OBSTACLES (Murs Violets)
        else if (collider && collider->isWall(simX, simY)) {
            p.vel.x *= -0.7; 
            p.pos.x += p.vel.x; 
            hit = true;
        } 
        else {
            p.pos.x = nextX;
        }

        // --- COLLISION Y ---
        simX = p.pos.x / scale; 
        simY = nextY / scale;

        if (collider && collider->isWall(simX, simY)) {
            p.vel.y *= -0.5; 
            p.vel.x *= 0.9;  
            hit = true;
        }
        else if (nextY > height) { 
            p.pos.y = height;
            p.vel.y *= -0.5;
            hit = true;
        }
        else {
            p.pos.y = nextY;
        }

        // --- SPLASH ---
        if (hit && p.isSplitter && p.vel.length() > 3.0 && p.radius > 3.0) {
            p.isSplitter = false;
            p.radius *= 0.7;
            
            int drops = (int)ofRandom(2, 5);
            for(int k=0; k<drops; k++){
                SlimeParticle sp;
                sp.pos = p.pos; 
                sp.vel = p.vel * -0.3 + ofVec2f(ofRandom(-5, 5), ofRandom(-5, -2));
                sp.radius = p.radius * 0.5;
                sp.life = p.life * 0.8;
                sp.age = 0;
                sp.color = p.color;
                sp.isSplitter = false;
                particles.push_back(sp);
            }
        }
        
        // Vieillissement
        p.age++;
        p.radius *= 0.995;
        if (p.age > p.life || p.radius < 0.5) {
            particles.erase(particles.begin() + i);
        }
    }
}

//--------------------------------------------------------------
void SlimeLayer::draw() {
    ofPushStyle();
        ofFill(); // Force le remplissage pour éviter l'effet filaire hérité

    // Le mode ADD permet aux couleurs de s'additionner -> effet lumineux/néon
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    for (const auto& p : particles) {
        ofSetColor(p.color.r, p.color.g, p.color.b, 100); 
        ofDrawCircle(p.pos, p.radius * 2.5); 
    }

    for (const auto& p : particles) {
        ofSetColor(255, 255, 255, 200); 
        ofDrawCircle(p.pos, p.radius);  
    }
    
    ofDisableBlendMode();
    ofPopStyle();
}

//--------------------------------------------------------------
void SlimeLayer::pour(float x, float y, float vx_start, float vy_start) {
    int amount = 5; 
    
    for (int j = 0; j < amount; j++) {
        SlimeParticle p;
        float spread = 10.0f;
        p.pos.set(x + ofRandom(-spread, spread), y + ofRandom(-spread, spread));
        p.vel.x = vx_start + ofRandom(-3, 3);
        p.vel.y = vy_start + ofRandom(-3, 3);
        
        if(ofRandom(1.0) > 0.5){
            p.color = ofColor(0, 255, 100); 
        } else {
            p.color = ofColor(0, 200, 255); 
        }
        
        p.radius = ofRandom(3, 12); 
        p.life = ofRandom(100, 200);
        p.age = 0;
        p.isSplitter = true; 
        
        particles.push_back(p);
    }
}