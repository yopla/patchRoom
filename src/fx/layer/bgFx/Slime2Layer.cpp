#include "Slime2Layer.h"

//--------------------------------------------------------------
void Slime2Layer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    scale = s;
    collider = col;
}

//--------------------------------------------------------------
void Slime2Layer::explode(float x, float y) {
    int numParticles = 80; 
    for(int i=0; i<numParticles; i++) {
        Slime2Particle p;
        p.pos.set(x, y);
        
        // Explosion dans toutes les directions
        float angle = ofRandom(TWO_PI);
        float speed = ofRandom(5.0, 18.0);
        p.vel.set(cos(angle) * speed, sin(angle) * speed);
        
        // Taille variable
        p.radius = ofRandom(5, 15);
        
        // Couleurs : Vert, Jaune, Violet (Style "Trempette radioactive")
        float r = ofRandom(1.0);
        if(r < 0.33) p.color = ofColor(100, 255, 50);   // Vert acide
        else if(r < 0.66) p.color = ofColor(255, 255, 0); // Jaune
        else p.color = ofColor(180, 0, 255);           // Violet
        
        p.life = 1.0f;
        p.isStuck = false;
        
        particles.push_back(p);
    }
}

//--------------------------------------------------------------
void Slime2Layer::update(float time) {
    for(auto& p : particles) {
        if(p.life <= 0) continue;

        if(!p.isStuck) {
            // Physique en l'air
            p.vel.y += 0.6f; // Gravité
            p.vel *= 0.96f;  // Friction de l'air
            p.pos += p.vel;
            
            // Collision avec le Collider
            if(collider) {
                // Conversion World -> Sim pour le collider
                float simX = p.pos.x / scale;
                float simY = p.pos.y / scale;

                // Vérification si on touche un mur
                if(simX >= 0 && simX < collider->simWidth && simY >= 0 && simY < collider->simHeight) {
                    if(collider->isWall(simX, simY)) {
                        p.isStuck = true;
                        p.vel.set(0,0); // Stop net (effet collant)
                    }
                }
            }
            
            // Sol basique (si pas de collider ou hors limites)
            if(p.pos.y > simHeight) {
                p.pos.y = simHeight;
                p.isStuck = true;
                p.vel.set(0,0);
            }
            
        } else {
            // Comportement gluant (Drip)
            // Ça coule doucement vers le bas
            if(ofRandom(1.0) < 0.15) { 
                float dripSpeed = ofRandom(0.5, 2.0);
                float nextY = p.pos.y + dripSpeed;
                
                // Conversion pour le check futur
                float simX = p.pos.x / scale;
                float simNextY = nextY / scale;

                // On continue de couler si on est toujours sur un mur
                if(collider && collider->isWall(simX, simNextY)) {
                    p.pos.y = nextY;
                } else {
                    // Si on arrive au bout du mur (vide en dessous)
                    // Soit on tombe, soit on reste accroché au bord (goutte)
                    if(ofRandom(1.0) < 0.1) {
                        p.isStuck = false; // Se détache et tombe
                    }
                }
            }
        }
        
        // Vie
        p.life -= 0.004f;
    }
    
    // Nettoyage
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Slime2Particle& p){ return p.life <= 0; }), particles.end());
}

//--------------------------------------------------------------
void Slime2Layer::draw() {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD); // Glow
    ofFill(); // Force le remplissage pour éviter l'effet filaire hérité
    
    // 1. Halo coloré (Dessiné en premier pour le mélange)
    for(auto& p : particles) {
        ofColor c = p.color;
        c.a = p.life * 100; // Transparence douce
        ofSetColor(c);
        ofDrawCircle(p.pos, p.radius * 2.5f * p.life);
    }

    // 2. Coeur blanc (Dessiné par dessus pour la netteté)
    for(auto& p : particles) {
        ofColor c(255, 255, 255);
        c.a = p.life * 200; // Blanc intense
        ofSetColor(c);
        ofDrawCircle(p.pos, p.radius * p.life);
    }
    
    ofDisableBlendMode();
    ofPopStyle();
}