#include "RigidbodySam.h"

void RigidbodySam::updateInteraction() {
    applyForce(ofVec2f(0, 0.4f * mass), pos); // Gravité

    if (bIsDragged) {
        ofVec2f dragWorld = pos + dragPointLocal.getRotated(ofRadToDeg(angle));
        ofVec2f diff = dragTarget - dragWorld;
        
        // Vitesse désirée proportionnelle à la distance
        ofVec2f targetVel = diff * 0.4f;
        
        ofVec2f r = dragWorld - pos;
        ofVec2f rPerp(-r.y, r.x);
        ofVec2f pointVel = vel + rPerp * angularVel;
        
        ofVec2f velDiff = targetVel - pointVel;
        
        // Force d'impulsion solide (Ressort + Damping pur)
        applyForce(velDiff * (mass * 1.0f), dragWorld);
        
        vel *= 0.8f;
        angularVel *= 0.8f;
    }
    
    vel += forceAccum * invMass;
    angularVel += torqueAccum * invInertia;
    
    if (vel.lengthSquared() > 900.0f) {
        vel.normalize();
        vel *= 30.0f;
    }
    if (abs(angularVel) > 1.5f) {
        angularVel = (angularVel > 0 ? 1.5f : -1.5f);
    }

    pos += vel;
    angle += angularVel;

    vel *= 0.99f;
    angularVel *= 0.98f;

    forceAccum.set(0,0);
    torqueAccum = 0;
}