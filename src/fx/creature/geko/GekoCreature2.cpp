#include "GekoCreature2.h"

//--------------------------------------------------------------
GekoCreature2::GekoCreature2(float startX, float startY, int nLegs) {
    bodyPos = glm::vec2(startX, startY);
    bodyVel = glm::vec2(0, 0);
    
    // Paramètres physiques (Style première version)
    friction = 0.85f;
    springStiffness = 0.06f; 
    
    bodySize = 20.0f;
    legReach = 60.0f;
    
    wanderAngle = 0.0f;
    movingLegIndex = -1;

    // Initialisation dynamique des pattes
    setLegCount(nLegs);
}

//--------------------------------------------------------------
void GekoCreature2::setLegCount(int n) {
    if (n < 2) n = 2; // Minimum de sécurité
    numLegs = n;
    legs.clear();
    
    for(int i=0; i<numLegs; i++) {
        GekoLeg leg;
        // Répartition équitable des angles (ex: 120° pour 3, 90° pour 4...)
        leg.angleOffset = (TWO_PI / (float)numLegs) * i;
        
        leg.position = bodyPos + glm::vec2(cos(leg.angleOffset), sin(leg.angleOffset)) * legReach;
        leg.target = leg.position;
        leg.isMoving = false;
        leg.moveProgress = 0.0f;
        legs.push_back(leg);
    }
}
void GekoCreature2::update(float mx, float my) {
    
    // 1. DÉFINITION DE LA CIBLE (Directement vers la souris)
    currentTarget = glm::vec2(mx, my);

    // Calcul de la direction vers la souris
    glm::vec2 toTarget = currentTarget - bodyPos;
    float distToTarget = glm::length(toTarget);
    
    // Orientation du corps : on regarde toujours la souris
    float bodyAngle = atan2(toTarget.y, toTarget.x);

    // Petit boost de force vers la cible si on est loin
    if(distToTarget > 10.0f) {
        bodyVel += glm::normalize(toTarget) * 0.5f; 
    }

    // 2. LOGIQUE DE MARCHE (GAIT) PLUS RÉACTIVE
    
    if (movingLegIndex == -1) {
        float maxDist = 0;
        int candidateIndex = -1;

        for(int i=0; i<numLegs; i++) {
            // Position idéale actuelle (relative au corps et à sa rotation)
            float globalLegAngle = bodyAngle + legs[i].angleOffset;
            glm::vec2 idealPos = bodyPos + glm::vec2(cos(globalLegAngle), sin(globalLegAngle)) * legReach;
            
            float d = glm::distance(legs[i].position, idealPos);
            
            // SEUIL RÉDUIT : On bouge la patte dès qu'elle s'éloigne un peu (0.6x au lieu de 1.5x)
            if (d > (legReach * 0.6f) && d > maxDist) {
                maxDist = d;
                candidateIndex = i;
            }
        }

        if (candidateIndex != -1) {
            movingLegIndex = candidateIndex;
            legs[movingLegIndex].isMoving = true;
            legs[movingLegIndex].moveProgress = 0.0f;
            
            // CIBLE DE LA PATTE : On anticipe le mouvement du corps
            float globalLegAngle = bodyAngle + legs[movingLegIndex].angleOffset;
            
            // On projette la patte vers l'avant (direction cible) pour "tirer" le corps
            glm::vec2 lead = (distToTarget > 1.0f) ? glm::normalize(toTarget) * 30.0f : glm::vec2(0);
            legs[movingLegIndex].target = bodyPos + lead + glm::vec2(cos(globalLegAngle), sin(globalLegAngle)) * legReach;
        }
    } 
    else {
        // ANIMATION PLUS RAPIDE (0.25f au lieu de 0.15f)
        GekoLeg &leg = legs[movingLegIndex];
        leg.moveProgress += 0.25f; 
        
        if (leg.moveProgress >= 1.0f) {
            leg.position = leg.target;
            leg.isMoving = false;
            movingLegIndex = -1;
        } else {
            // Interpolation plus directe pour un effet "snap"
            leg.position = glm::mix(leg.position, leg.target, leg.moveProgress);
        }
    }

    // 3. PHYSIQUE
    for(auto& leg : legs) {
        if(leg.isMoving) continue; 
        
        // Les pattes au sol tirent le corps vers elles (ressort)
        glm::vec2 pull = leg.position - bodyPos;
        bodyVel += pull * springStiffness;
    }

    bodyVel *= friction;
    bodyPos += bodyVel;
}

//--------------------------------------------------------------
void GekoCreature2::draw() {
    ofEnableAlphaBlending();
    
    // Dessin des pattes (Boucle dynamique)
    ofSetLineWidth(3);
    for(int i=0; i<numLegs; i++) {
        if(legs[i].isMoving) ofSetColor(255, 100, 100, 200);
        else ofSetColor(255, 255, 255, 150);
        
        ofDrawLine(bodyPos.x, bodyPos.y, legs[i].position.x, legs[i].position.y);
        ofDrawCircle(legs[i].position.x, legs[i].position.y, 8);
    }

    // Dessin du corps
    ofSetColor(0, 255, 150);
    ofDrawCircle(bodyPos.x, bodyPos.y, bodySize);
    
    // Yeux (Direction)
    if(glm::length(bodyVel) > 0.1) {
        glm::vec2 dir = glm::normalize(bodyVel);
        glm::vec2 side = glm::vec2(-dir.y, dir.x);
        
        ofSetColor(0);
        ofDrawCircle(bodyPos + dir*10 + side*8, 4);
        ofDrawCircle(bodyPos + dir*10 - side*8, 4);
    }
}