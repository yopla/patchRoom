#include "SauteursLayer.h"

//--------------------------------------------------------------
void SauteursLayer::setup(float realSceneWidth, float targetRealHeight, shared_ptr<ColliderLayer> colliders) {
    this->colliderLayer = colliders;

    // Calcul de l'échelle comme avant
    // NOTE : Pour une synchro parfaite, il faudrait utiliser colliders->simWidth
    // mais ici on garde ta logique de calcul basée sur la largeur.
    scale = realSceneWidth / simWidth;
    simHeight = targetRealHeight / scale;

    // 5. Sauteurs
    sauteurs.resize(numSauteurs);
    for(auto& s : sauteurs) {
        s.pos.x = ofRandom(simWidth);
        s.pos.y = ofRandom(simHeight * 0.5); 
        s.xrad = .75; 
        s.yrad = .75;
        s.ready = true;
        s.vel.set(0,0);
    }
}

//--------------------------------------------------------------
void SauteursLayer::update(float mouseX, float mouseY) {
    if(!colliderLayer) return;

    for(auto& s : sauteurs) {
        
        // 1. Gravité
        s.vel.y += 0.2;

        // Limite Vitesse
        if(s.vel.y > 5.0) s.vel.y = 5.0; 

        // 2. Check Sol (via ColliderLayer)
        int bGround = 0;
        if (colliderLayer->isWall(s.pos.x, s.pos.y + s.yrad + 1)) {
            bGround = 1;
        }

        if (bGround) {
            s.w += 1;
            s.vel.x *= 0.8; // Frottement
        }

        // 3. Saut
        if (s.w >= 32) { 
            s.w = 0;
            s.vel.x = (ofRandom(1.0) - ofRandom(1.0)) * 3.0; 
            s.vel.y = -3.5 + (ofRandom(1.0) - ofRandom(1.0)) * -2.0; 

            // Rebond murs immédiat
            if (s.vel.x > 0 && colliderLayer->isWall(s.pos.x + s.xrad + 1, s.pos.y)) s.vel.x = -s.vel.x;
            else if (s.vel.x < 0 && colliderLayer->isWall(s.pos.x - s.xrad - 1, s.pos.y)) s.vel.x = -s.vel.x;
        }

        // 4. Collisions Vitesse (Predictive)
        if (s.vel.y > 0 && colliderLayer->isWall(s.pos.x, s.pos.y + s.yrad + s.vel.y)) s.vel.y = 0;
        if (s.vel.y < 0 && colliderLayer->isWall(s.pos.x, s.pos.y - s.yrad + s.vel.y)) s.vel.y = 0;
        
        if (s.vel.x > 0 && colliderLayer->isWall(s.pos.x + s.xrad + s.vel.x, s.pos.y)) s.vel.x = -s.vel.x * 0.5;
        if (s.vel.x < 0 && colliderLayer->isWall(s.pos.x - s.xrad + s.vel.x, s.pos.y)) s.vel.x = -s.vel.x * 0.5;

        // 5. Application Mouvement
        s.pos.x += s.vel.x;
        s.pos.y += s.vel.y;

        // 6. Anti-pénétration doux
        if (colliderLayer->isWall(s.pos.x, s.pos.y)) {
            s.pos.y -= 0.5; // On remonte doucement
        }

        // Wrapping X
        if (s.pos.x < 0) s.pos.x += simWidth;
        if (s.pos.x > simWidth) s.pos.x -= simWidth;
        
        // Sécurité
        if (s.pos.y > simHeight) {
            s.pos.y = 0;
            s.vel.set(0,0);
        }
    }
}

//--------------------------------------------------------------
void SauteursLayer::draw() {
    ofPushStyle();
    
    // NOTE : On ne dessine PLUS les murs ici. 
    // colliderLayer->draw() doit être appelé depuis Scene2D.

    // Sauteurs (Jaunes)
    for(auto& s : sauteurs) {
        float rx = s.pos.x * scale;
        float ry = s.pos.y * scale;
        float rw = s.xrad * scale * 2; 
        float rh = s.yrad * scale * 2;

        ofFill();
        ofSetColor(255, 255, 0); 
        ofDrawRectangle(rx - rw/2, ry - rh/2, rw, rh);
    }
    ofPopStyle();
}