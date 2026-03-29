#include "GearsSam.h"

void GearsSam::setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) {
    PhysicSamBase::setup(contour, bbox, sourceImg, scale, offsetY);
    anchorPos = pos;
    invMass = 0.0f; // Rend l'objet fixe (masse infinie) pour le solveur
}

void GearsSam::updateInteraction() {
    if (bIsDragged) {
        ofVec2f dragWorld = pos + dragPointLocal.getRotated(ofRadToDeg(angle));
        ofVec2f diff = dragTarget - dragWorld;
        
        ofVec2f targetVel = diff * 0.4f;
        ofVec2f r = dragWorld - pos;
        ofVec2f rPerp(-r.y, r.x);
        ofVec2f pointVel = rPerp * angularVel; // vel linéaire est 0
        
        ofVec2f velDiff = targetVel - pointVel;
        
        // On génère une force proportionnelle à l'inertie de l'engrenage
        ofVec2f force = velDiff * (inertia * 0.01f);
        applyForce(force, dragWorld);
        
        angularVel *= 0.8f; // Amortissement pendant la manipulation
    }
    
    angularVel += torqueAccum * invInertia;
    
    if (abs(angularVel) > 1.5f) {
        angularVel = (angularVel > 0 ? 1.5f : -1.5f);
    }
    
    angle += angularVel;
    angularVel *= 0.98f;
    
    vel.set(0,0); // Force la vélocité linéaire à zéro
    pos = anchorPos; // Force le maintien sur l'axe
    
    forceAccum.set(0,0);
    torqueAccum = 0;
}

void GearsSam::wrap(float shiftX) {
    PhysicSamBase::wrap(shiftX);
    anchorPos.x += shiftX;
}