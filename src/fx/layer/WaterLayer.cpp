#include "WaterLayer.h"

//--------------------------------------------------------------
void WaterLayer::setup(float w, float h) {
    width = w;
    height = h;
    // Valeur AS3 : public static const GRAVITY:Number = 0.05;
    gravityVal = 0.05f; 
}

//--------------------------------------------------------------
void WaterLayer::update() {
    // On parcourt à l'envers pour pouvoir supprimer proprement
    for (int i = particles.size() - 1; i >= 0; i--) {
        WaterParticle& p = particles[i];

        // 1. Physique (AS3: move())
        p.vel.y += gravityVal; // Gravité
        p.pos += p.vel;
        
        // 2. Gestion des murs (Rebonds simples basés sur l'AS3)
        // AS3: if(x < 5) ...
        if (p.pos.x < 0) {
            p.pos.x = -p.pos.x;
            p.vel.x *= -0.5; // Perte d'énergie
        }
        if (p.pos.x > width) {
            p.pos.x = width - (p.pos.x - width);
            p.vel.x *= -0.5;
        }
        if (p.pos.y > height) {
            p.pos.y = height - (p.pos.y - height);
            p.vel.y *= -0.5; // Rebond au sol
            // Friction au sol (simulée)
            p.vel.x *= 0.9;
        }
        
        // 3. Vieillissement
        p.age++;
        
        // Suppression si trop vieux ou trop bas (AS3: if (pi.y>1000) || ((pi.age>pi.life)))
        if (p.age > p.life || p.pos.y > height + 100) {
            particles.erase(particles.begin() + i);
        }
    }
}

//--------------------------------------------------------------
void WaterLayer::draw() {
    ofPushStyle();
    // Le mode ADD donne un effet "Glow" similaire aux filtres AS3
    ofEnableBlendMode(OF_BLENDMODE_ADD); 
    
    for (const auto& p : particles) {
        ofSetColor(p.color);
        // AS3: img.fillRect(..., p.tail, p.tail) -> Dessin carré ou rond
        ofDrawRectangle(p.pos.x - p.size/2, p.pos.y - p.size/2, p.size, p.size);
    }
    
    ofDisableBlendMode();
    ofPopStyle();
}

//--------------------------------------------------------------
void WaterLayer::pour(float x, float y) {
    // Traduction de la fonction pour() de l'AS3
    // AS3: for(var j:int = 0; j < 10; j++) ...
    
    int amount = 10; // Débit
    for (int j = 0; j < amount; j++) {
        WaterParticle p;
        
        // Position avec léger aléatoire (AS3: mouseX + Math.random()*i * 8)
        float spread = 8.0f;
        p.pos.set(x + ofRandom(-spread, spread), y + ofRandom(-spread, spread));
        
        // Vélocité (AS3: vy = Math.random()*2-1.5; vx = Math.random()*2-1.0;)
        p.vel.x = ofRandom(-1.0, 1.0);
        p.vel.y = ofRandom(-1.5, 0.5);
        
        // "Splitters" logic (AS3: vx*=Math.random()*3)
        // Certaines particules partent plus vite
        if (ofRandom(100) < 10) {
            p.vel *= ofRandom(1.0, 3.0);
        }
        
        // Couleur (AS3: 18+Math.random()*0xffffffff)
        // On génère des teintes d'eau (Bleu/Blanc/Alpha)
        p.color = ofColor(100 + ofRandom(155), 200 + ofRandom(55), 255, 150);
        
        // Taille (AS3: tail=(Math.random()*7))
        p.size = ofRandom(1, 7);
        
        // Vie (AS3: life = Math.random()*105+5)
        p.life = ofRandom(50, 150); // Ajusté pour 60fps
        p.age = 0;
        
        particles.push_back(p);
    }
}

//--------------------------------------------------------------
void WaterLayer::setGravity(float g) {
    gravityVal = g;
}