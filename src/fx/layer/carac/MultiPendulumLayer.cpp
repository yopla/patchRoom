#include "MultiPendulumLayer.h"

void PendulumUnit::setup(float amp, float len, ofColor col) {
    amplitude = amp;
    length = len;
    color = col;
    radian = 0;
    
    // AS3 Ref: _omega = Math.sqrt(DEF_LENGTH/_length)*DEF_OMEGA;
    // DEF_LENGTH = 10, DEF_OMEGA = 0.4
    float defLength = 10.0f;
    float defOmega = 0.4f;
    
    // Adaptation de la physique pour l'échelle de l'écran
    // On réduit la longueur "physique" perçue pour garder une vitesse d'oscillation intéressante
    float physicsLength = length * 0.15f; 
    omega = sqrt(defLength / physicsLength) * defOmega;
}

void PendulumUnit::update() {
    radian += omega;
    float newX = sin(radian) * amplitude;
    
    // y = sqrt(l^2 - x^2)
    // Protection contre NaN si l'amplitude dépasse la longueur (ne devrait pas arriver avec setup)
    float val = length * length - newX * newX;
    float newY = (val > 0) ? sqrt(val) : 0;
    
    pos.set(newX, newY);
}

void PendulumUnit::draw(float originX, float originY) {
    ofPushStyle();
    ofSetColor(color);
    ofFill();
    float r = 20.0f; // Rayon adapté
    ofDrawCircle(originX + pos.x, originY + pos.y, r);
    
    ofSetLineWidth(3);
    ofDrawLine(originX, originY, originX + pos.x, originY + pos.y);
    ofPopStyle();
}

void MultiPendulumLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    // Origine au centre horizontal, un peu en haut
    origin.set(w * 0.5f, 200.0f); 
    
    pendulums.clear();
    
    int num = 5;
    float scale = 3.5f; // Facteur d'échelle pour la scène HD
    float baseAmp = 40.0f * scale;
    float minLen = 100.0f * scale;
    float interval = 20.0f * scale;
    
    for(int i=0; i<num; i++) {
        PendulumUnit p;
        ofColor col = ofColor::fromHsb(ofRandom(255), 200, 255);
        p.setup(baseAmp, minLen + interval * i, col);
        pendulums.push_back(p);
    }
}

void MultiPendulumLayer::update(float mouseX, float mouseY, float time) {
    for(auto& p : pendulums) {
        p.update();
    }
}

void MultiPendulumLayer::draw() {
    for(auto& p : pendulums) {
        p.draw(origin.x, origin.y);
    }
}