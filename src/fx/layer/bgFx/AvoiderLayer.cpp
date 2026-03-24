#include "AvoiderLayer.h"

//--------------------------------------------------------------
void AvoiderLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // Génération du groupe d'Avoiders
    for(int i = 0; i < 45; i++) {
        Avoider a;
        a.pos.set(ofRandom(w), ofRandom(h));
        a.vel.set(ofRandom(-1, 1), ofRandom(-1, 1));
        a.vel.normalize();
        a.vel *= ofRandom(2, 4);
        a.maxSpeed = ofRandom(4.0f, 7.0f);
        a.maxForce = 0.15f;
        a.radius = ofRandom(8, 16);
        
        // Couleurs (Turquoise / Violet style cyber-biologique)
        a.color.setHsb(ofRandom(120, 190), 200, 255); 
        avoiders.push_back(a);
    }
}

//--------------------------------------------------------------
void AvoiderLayer::update(float time) {
    for(auto& a : avoiders) {
        // 1. Comportement Wander (Champ de vecteurs basé sur le Perlin noise)
        float noiseX = ofSignedNoise(a.pos.x * 0.003, a.pos.y * 0.003, time * 0.5) * a.maxSpeed;
        float noiseY = ofSignedNoise(a.pos.x * 0.003, a.pos.y * 0.003, time * 0.5 + 1000) * a.maxSpeed;
        
        ofVec2f desired(noiseX, noiseY);
        ofVec2f steer = desired - a.vel;
        steer.limit(a.maxForce);
        a.applyForce(steer);

        // 2. Application de la physique
        a.update();

        // 3. Bouclage infini sur les bords (Wrap gauche/droite)
        if(a.pos.x < -a.radius) a.pos.x = simWidth + a.radius;
        if(a.pos.x > simWidth + a.radius) a.pos.x = -a.radius;

        // 4. Rebond doux en haut et en bas
        if(a.pos.y < a.radius) { a.pos.y = a.radius; a.vel.y *= -1; }
        if(a.pos.y > simHeight - a.radius) { a.pos.y = simHeight - a.radius; a.vel.y *= -1; }
    }
}

//--------------------------------------------------------------
void AvoiderLayer::draw() {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD); // Effet luminescent
    
    // Préparation pour le dessin cyclique (Fantômes sur les bords)
    vector<ofVec2f> offsets = {ofVec2f(0, 0), ofVec2f(simWidth, 0), ofVec2f(-simWidth, 0)};
    float wrapThreshold = 200.0f; // Ne dessiner les fantômes que si l'avoider est proche des bords

    for(auto& a : avoiders) {
        ofSetColor(a.color);
        
        for(auto& offset : offsets) {
            // Optimisation : On ne dessine les offsets que si c'est utile
            float drawX = a.pos.x + offset.x;
            if (drawX < -wrapThreshold || drawX > simWidth + wrapThreshold) continue;

            ofPushMatrix();
            ofTranslate(drawX, a.pos.y);
            
            // Rotation dans le sens de la vélocité
            float angle = atan2(a.vel.y, a.vel.x);
            ofRotateRad(angle);

            // Dessin forme "Boid" (Triangle pointu)
            ofFill();
            ofDrawTriangle(a.radius, 0, -a.radius, -a.radius * 0.6, -a.radius, a.radius * 0.6);
            
            // Halo énergétique central
            ofSetColor(255, 255, 255, 150);
            ofDrawCircle(-a.radius * 0.2, 0, a.radius * 0.4);
            
            ofPopMatrix();
        }
    }
    
    ofDisableBlendMode();
    ofPopStyle();
}