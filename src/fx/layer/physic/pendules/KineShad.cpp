#include "KineShad.h"

void KineShad::setup() {
    // Point fixe dans la scene 2D, au centre du mur "Front"
    posA.set(3824, 736);
    time = 0;
}

void KineShad::update(const ofVec2f& mouse, float localTime) {
    time = localTime;
    mousePos = mouse;

    float ta = time * 0.1f;
    
    // Le point B orbite autour de la souris
    posB.set(mouse.x + cos(ta) * 32, mouse.y + sin(ta) * 32);
    
    ofVec2f a = posA;
    ofVec2f b = posB;
    
    ofVec2f n = b - a;
    float mag = n.length();
    if (mag > 0.001f) {
        n.normalize();
    } else {
        n.set(1, 0); // Evite la division par zero
    }
    
    // Calcul du point intermediaire K, inspiré de la cinematique inverse de la ref AS3
    // kx = (bx+ax)*0.5 +ny*-16;
    // ky = (by+ay)*0.5 +nx* 16;
    // C'est le point milieu + un vecteur perpendiculaire
    posK = (a + b) * 0.5f + n.getPerpendicular() * 16.0f;
    
    float r = 64;
    
    // Iterations pour contraindre la position de K
    for(int i=0; i<8; i++) {
        ofVec2f d;
        if (i % 2 == 0) {
            d = posK - b;
        } else {
            d = posK - a;
        }
        
        float d_mag = d.length();
        if (d_mag < 0.001f) d_mag = 0.1f;
        d.normalize();
        
        float mag_adjust = r - d_mag;
        mag_adjust *= 0.5f;
        
        posK += d * mag_adjust;
    }
}

void KineShad::draw() {
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(255);
    
    float ta = time * 0.1f;
    
    // Dessin de l'indicateur sur la souris
    ofDrawCircle(mousePos.x, mousePos.y, 32);
    ofDrawLine(mousePos.x + cos(ta)*32, mousePos.y + sin(ta)*32,
               mousePos.x - cos(ta)*32, mousePos.y - sin(ta)*32);
    
    // Dessin de la chaine cinematique
    ofDrawCircle(posA.x, posA.y, 8);
    ofDrawCircle(posB.x, posB.y, 8);
    ofDrawCircle(posK.x, posK.y, 8);
    
    ofDrawLine(posA.x, posA.y, posK.x, posK.y);
    ofDrawLine(posK.x, posK.y, posB.x, posB.y);
    
    ofPopStyle();
}