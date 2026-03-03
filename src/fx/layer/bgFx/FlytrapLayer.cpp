#include "FlytrapLayer.h"

void Flytrap::setup(float x, float y) {
    basePos.set(x, y);
    stemLength = ofRandom(200, 350);
    int numSegments = 10;
    segmentLength = stemLength / numSegments;
    noiseOffset = ofRandom(1000);
    
    for(int i=0; i<=numSegments; i++) {
        nodes.push_back(ofVec2f(x, y - i * segmentLength));
    }
    
    headPos = nodes.back();
    headAngle = -HALF_PI;
    mouthOpenness = 0.0f;
    isBiting = false;
    biteTimer = 0.0f;
    
    color.set(100 + ofRandom(50), 180 + ofRandom(50), 50);
}

void Flytrap::update(float mx, float my, float time) {
    
    // 1. Détermination de la cible (Souris ou Repos)
    ofVec2f target;
    float distToMouse = basePos.distance(ofVec2f(mx, my));
    bool mouseActive = (distToMouse < 400 && my < basePos.y); // Si souris proche et au-dessus du sol
    
    if (mouseActive) {
        target.set(mx, my);
    } else {
        // Mouvement de repos (ondulation)
        float sway = ofSignedNoise(time * 0.5 + noiseOffset) * 80;
        target.set(basePos.x + sway, basePos.y - stemLength);
    }
    
    // 2. Animation de la tige (Cinématique simplifiée)
    // On calcule l'angle global vers la cible
    ofVec2f dirToTarget = target - basePos;
    float targetAngle = atan2(dirToTarget.y, dirToTarget.x);
    
    // Le premier noeud est fixe
    nodes[0] = basePos;
    
    for (int i = 1; i < nodes.size(); i++) {
        // Plus on monte, plus c'est flexible
        float stiffness = 1.0 - (float)i / nodes.size(); 
        float reactivity = mouseActive ? 0.15 : 0.03; 
        
        // Angle actuel du segment
        ofVec2f currentDir = nodes[i] - nodes[i-1];
        float currentAngle = atan2(currentDir.y, currentDir.x);
        
        // Bruit de vent
        float wind = ofSignedNoise(time * 1.5 + i * 0.1 + noiseOffset) * 0.3 * stiffness;
        
        // Interpolation angulaire vers la cible
        float angleDiff = targetAngle - currentAngle;
        while (angleDiff < -PI) angleDiff += TWO_PI;
        while (angleDiff > PI) angleDiff -= TWO_PI;
        
        float newAngle = currentAngle + angleDiff * reactivity + wind;
        
        // Mise à jour position
        nodes[i].x = nodes[i-1].x + cos(newAngle) * segmentLength;
        nodes[i].y = nodes[i-1].y + sin(newAngle) * segmentLength;
    }
    
    // 3. Logique de la Tête (Morsure)
    headPos = nodes.back();
    // L'angle de la tête suit le dernier segment
    ofVec2f stemDir = nodes.back() - nodes[nodes.size()-2];
    headAngle = atan2(stemDir.y, stemDir.x);
    
    float distHeadToMouse = headPos.distance(ofVec2f(mx, my));
    
    if (isBiting) {
        // Fermeture rapide
        mouthOpenness = ofLerp(mouthOpenness, 0.0, 0.3);
        biteTimer -= 1.0/60.0;
        if (biteTimer <= 0) isBiting = false;
    } else {
        if (mouseActive) {
            // Orienter la tête précisément vers la souris
            float angleToMouse = atan2(my - headPos.y, mx - headPos.x);
            float angleDiff = angleToMouse - headAngle;
            while (angleDiff < -PI) angleDiff += TWO_PI;
            while (angleDiff > PI) angleDiff -= TWO_PI;
            headAngle += angleDiff * 0.5; // Rotation rapide de la tête
            
            // Ouvrir la bouche si proche
            if (distHeadToMouse < 150) {
                mouthOpenness = ofLerp(mouthOpenness, 1.0, 0.1);
            } else {
                mouthOpenness = ofLerp(mouthOpenness, 0.2, 0.05);
            }
            
            // DÉCLENCHEMENT MORSURE
            if (distHeadToMouse < 30 && mouthOpenness > 0.8) {
                isBiting = true;
                biteTimer = 2.0; // Reste fermé 2 secondes
            }
        } else {
            mouthOpenness = ofLerp(mouthOpenness, 0.1, 0.05);
        }
    }
}

void Flytrap::draw() {
    // Dessin Tige
    ofSetColor(color);
    ofSetLineWidth(4);
    ofNoFill();
    ofBeginShape();
    for(auto& n : nodes) ofVertex(n);
    ofEndShape();
    
    // Dessin Tête
    ofPushMatrix();
    ofTranslate(headPos.x, headPos.y);
    ofRotateRad(headAngle);
    
    float size = 25;
    float openAngle = mouthOpenness * 0.8; // Max ouverture ~45 degrés
    
    // Mâchoire supérieure
    ofPushMatrix();
    ofRotateRad(-openAngle);
    ofSetColor(200, 50, 50); // Intérieur rouge
    ofDrawCircle(size/2, -size/4, size/1.5); // Bulbe
    ofSetColor(color); // Extérieur vert
    ofSetLineWidth(2);
    ofNoFill();
    ofDrawCircle(size/2, -size/4, size/1.5); // Contour
    // Dents
    ofFill();
    ofSetColor(255, 255, 200);
    ofDrawTriangle(size, -size/2, size+10, -size/2 - 5, size+5, -size/2 + 5);
    ofPopMatrix();
    
    // Mâchoire inférieure
    ofPushMatrix();
    ofRotateRad(openAngle);
    ofSetColor(200, 50, 50);
    ofDrawCircle(size/2, size/4, size/1.5);
    ofSetColor(color);
    ofNoFill();
    ofDrawCircle(size/2, size/4, size/1.5);
    // Dents
    ofFill();
    ofSetColor(255, 255, 200);
    ofDrawTriangle(size, size/2, size+10, size/2 + 5, size+5, size/2 - 5);
    ofPopMatrix();
    
    ofPopMatrix();
}

void FlytrapLayer::setup(float w, float h) {
    traps.clear();
    int numTraps = 8; // Nombre de plantes
    for(int i=0; i<numTraps; i++) {
        Flytrap t;
        float x = ofRandom(100, w - 100);
        float y = h + ofRandom(0, 20);
        t.setup(x, y);
        traps.push_back(t);
    }
}

void FlytrapLayer::update(float mx, float my, float time) {
    for(auto& t : traps) {
        t.update(mx, my, time);
    }
}

void FlytrapLayer::draw() {
    for(auto& t : traps) {
        t.draw();
    }
}