#include "GekoUnits.h"

// =============================================================
// GEKO BASE (Pas de changement ici, sauf constructeur si besoin)
// =============================================================
GekoBase::GekoBase(float startX, float startY, int nLegs) {
    bodyPos = glm::vec2(startX, startY);
    bodyVel = glm::vec2(0, 0);
    friction = 0.90f;
    springStiffness = 0.05f;
    bodySize = 20.0f;
    legReach = 60.0f;
    movingLegIndex = -1;
    wanderAngle = 0.0f;
    maxSpeed = 18.0f; 
    setLegCount(nLegs);
}
// ... (reste des méthodes de base identiques : setLegCount, applyPhysics, draw) ...
void GekoBase::setLegCount(int n) {
    numLegs = n;
    legs.clear();
    for(int i=0; i<numLegs; i++) {
        GekoLeg leg;
        leg.angleOffset = (TWO_PI / (float)numLegs) * i;
        leg.position = bodyPos + glm::vec2(cos(leg.angleOffset), sin(leg.angleOffset)) * legReach;
        leg.target = leg.position;
        legs.push_back(leg);
    }
}

void GekoBase::applyPhysics() {
    for(auto& leg : legs) {
        if(!leg.isMoving) {
            glm::vec2 pull = leg.position - bodyPos;
            float d = glm::length(pull);
            float stretch = d - legReach;
            if(stretch > 0) bodyVel += glm::normalize(pull) * stretch * springStiffness;
        }
    }
    bodyVel *= friction;
    bodyPos += bodyVel;
}

void GekoBase::draw() {
    ofEnableAlphaBlending();
    ofSetLineWidth(3);
    for(auto& leg : legs) {
        if(leg.isMoving) ofSetColor(255, 100, 100, 200);
        else ofSetColor(255, 255, 255, 150);
        ofDrawLine(bodyPos.x, bodyPos.y, leg.position.x, leg.position.y);
        ofDrawCircle(leg.position.x, leg.position.y, 6);
    }
    ofPushMatrix();
    ofTranslate(bodyPos.x, bodyPos.y);
    ofRotateDeg(wanderAngle);
    ofSetColor(0, 255, 150); 
    ofDrawEllipse(0, 0, bodySize * 2.5, bodySize * 1.5);
    ofSetColor(0);
    ofDrawCircle(15, -8, 4); ofDrawCircle(15, 8, 4);
    ofPopMatrix();
}


// =============================================================
// GEKO STANDARD (Modifié : Cherche la patte la plus loin de l'idéal)
// =============================================================
GekoStandard::GekoStandard(float x, float y) : GekoBase(x, y, 4) {
    maxSpeed = 12.0f; 
    bodySize = 25.0f;
}

void GekoStandard::update(float mx, float my) {
    glm::vec2 target(mx, my);
    glm::vec2 dir = target - bodyPos;
    float dist = glm::length(dir);
    
    if(dist > 100.0f) {
        glm::vec2 force = glm::normalize(dir) * 1.5f;
        bodyVel += force;
        if(glm::length(bodyVel) > maxSpeed) bodyVel = glm::normalize(bodyVel) * maxSpeed;
    }

    float targetAngle = atan2(dir.y, dir.x);
    wanderAngle = ofLerpDegrees(wanderAngle, ofRadToDeg(targetAngle), 0.1f);

    // --- LOGIQUE INTELLIGENTE ---
    if (movingLegIndex == -1) {
        float maxDistScore = 0;
        int bestLeg = -1;

        // On cherche la patte qui est la plus "en retard"
        for(int i=0; i<numLegs; i++) {
            // Où la patte DEVRAIT être (Position idéale)
            float globalAngle = ofDegToRad(wanderAngle) + legs[i].angleOffset;
            glm::vec2 idealPos = bodyPos + glm::vec2(cos(globalAngle), sin(globalAngle)) * legReach;
            
            // On ajoute la vélocité future pour anticiper encore mieux
            idealPos += (bodyVel * 20.0f); 

            // Distance entre où elle est et où elle devrait être
            float d = glm::distance(legs[i].position, idealPos);

            // Si elle est trop loin et que c'est la pire, on la choisit
            if (d > legReach * 0.8f && d > maxDistScore) {
                maxDistScore = d;
                bestLeg = i;
            }
        }

        if (bestLeg != -1) {
            movingLegIndex = bestLeg;
            legs[bestLeg].isMoving = true;
            legs[bestLeg].moveProgress = 0.0f;
            
            // Calcul cible : Idéal + Anticipation forte vers la souris
            float globalAngle = ofDegToRad(wanderAngle) + legs[bestLeg].angleOffset;
            
            // Le "Lead" (anticipation) est crucial pour aller vers le curseur
            glm::vec2 lead = glm::normalize(dir) * 40.0f; 
            
            legs[bestLeg].target = bodyPos + lead + glm::vec2(cos(globalAngle), sin(globalAngle)) * legReach;
        }
    } else {
        GekoLeg &leg = legs[movingLegIndex];
        leg.moveProgress += 0.15f; 
        leg.position = glm::mix(leg.position, leg.target, leg.moveProgress);
        if(leg.moveProgress >= 1.0f) { leg.isMoving = false; movingLegIndex = -1; }
    }

    applyPhysics();
}

// =============================================================
// GEKO AGILE (Modifié : Très agressif sur la patte la plus loin)
// =============================================================
GekoAgile::GekoAgile(float x, float y) : GekoBase(x, y, 3) {
    maxSpeed = 22.0f; 
    friction = 0.80f; 
    bodySize = 15.0f; 
    springStiffness = 0.1f; 
}

void GekoAgile::update(float mx, float my) {
    glm::vec2 target(mx, my);
    glm::vec2 dir = target - bodyPos;
    float dist = glm::length(dir);

    if(dist > 105.0f) {
        bodyVel += glm::normalize(dir) * 2.5f;
        if(glm::length(bodyVel) > maxSpeed) bodyVel = glm::normalize(bodyVel) * maxSpeed;
    }

    float targetAngle = atan2(dir.y, dir.x);
    wanderAngle = ofLerpDegrees(wanderAngle, ofRadToDeg(targetAngle), 0.25f);

    if (movingLegIndex == -1) {
        float maxDistScore = 0;
        int bestLeg = -1;

        for(int i=0; i<numLegs; i++) {
            float a = ofDegToRad(wanderAngle) + legs[i].angleOffset;
            glm::vec2 ideal = bodyPos + glm::vec2(cos(a), sin(a)) * legReach;
            
            // Anticipation énorme pour le mode agile
            ideal += glm::normalize(dir) * 50.0f;

            float d = glm::distance(legs[i].position, ideal);
            
            // Seuil très bas (0.4) pour bouger frénétiquement la patte la plus en retard
            if(d > legReach * 0.4f && d > maxDistScore) { 
                maxDistScore = d; bestLeg = i;
            }
        }
        
        if(bestLeg != -1) {
            movingLegIndex = bestLeg;
            legs[bestLeg].isMoving = true;
            legs[bestLeg].moveProgress = 0.0f;
            float a = ofDegToRad(wanderAngle) + legs[bestLeg].angleOffset;
            
            // Cible projetée très loin devant
            legs[bestLeg].target = bodyPos + (glm::normalize(dir) * 60.0f) + glm::vec2(cos(a), sin(a)) * legReach;
        }
    } else {
        GekoLeg &leg = legs[movingLegIndex];
        leg.moveProgress += 0.3f; 
        leg.position = glm::mix(leg.position, leg.target, leg.moveProgress);
        if(leg.moveProgress >= 1.0f) { leg.isMoving = false; movingLegIndex = -1; }
    }
    applyPhysics();
}

// =============================================================
// GEKO ORBITER (Modifié : Logique fluide)
// =============================================================
GekoOrbiter::GekoOrbiter(float x, float y) : GekoBase(x, y, 5) {
    maxSpeed = 16.0f;
    friction = 0.92f;
    isOrbiting = false;
    orbitAngle = 0;
}

void GekoOrbiter::update(float mx, float my) {
    glm::vec2 mouse(mx, my);
    glm::vec2 toMouse = mouse - bodyPos;
    float dist = glm::length(toMouse);
    float orbitRadius = 150.0f;

    glm::vec2 dest;
    glm::vec2 moveDir; // Direction du mouvement souhaité

    if(dist > orbitRadius) {
        dest = mouse;
        isOrbiting = false;
        orbitAngle = atan2(-toMouse.y, -toMouse.x);
        moveDir = glm::normalize(toMouse);
    } else {
        isOrbiting = true;
        orbitAngle += 0.08f;
        dest = mouse + glm::vec2(cos(orbitAngle), sin(orbitAngle)) * orbitRadius;
        moveDir = glm::normalize(dest - bodyPos);
    }

    glm::vec2 dir = dest - bodyPos;
    if(glm::length(dir) > 1.0f) {
        bodyVel += glm::normalize(dir) * 1.2f;
        if(glm::length(bodyVel) > maxSpeed) bodyVel = glm::normalize(bodyVel) * maxSpeed;
    }

    float targetAngle = atan2(bodyVel.y, bodyVel.x);
    wanderAngle = ofLerpDegrees(wanderAngle, ofRadToDeg(targetAngle), 0.1f);

    if (movingLegIndex == -1) {
        float maxDistScore = 0;
        int bestLeg = -1;
        
        for(int i=0; i<numLegs; i++) {
            float a = ofDegToRad(wanderAngle) + legs[i].angleOffset;
            glm::vec2 ideal = bodyPos + glm::vec2(cos(a), sin(a)) * legReach;
            
            // On calcule l'écart par rapport à la direction du mouvement
            // Plus une patte est "derrière", plus elle doit bouger
            float d = glm::distance(legs[i].position, ideal);
            
            // Produit scalaire pour privilégier les pattes arrières par rapport au mouvement
            glm::vec2 legToBody = glm::normalize(bodyPos - legs[i].position);
            float alignment = glm::dot(legToBody, moveDir); // Si proche de 1, la patte est derrière
            
            // Score combiné : Distance + Alignement (si la patte est derrière, score augmente)
            float score = d + (alignment * 30.0f);

            if(d > legReach * 0.9f && score > maxDistScore) {
                maxDistScore = score; bestLeg = i;
            }
        }
        
        if(bestLeg != -1) {
            movingLegIndex = bestLeg;
            legs[bestLeg].isMoving = true;
            legs[bestLeg].moveProgress = 0.0f;
            float a = ofDegToRad(wanderAngle) + legs[bestLeg].angleOffset;
            
            // Cible fluide
            legs[bestLeg].target = bodyPos + (moveDir * 35.0f) + glm::vec2(cos(a), sin(a)) * legReach;
        }
    } else {
        GekoLeg &leg = legs[movingLegIndex];
        leg.moveProgress += 0.1f; 
        leg.position = glm::mix(leg.position, leg.target, leg.moveProgress);
        if(leg.moveProgress >= 1.0f) { leg.isMoving = false; movingLegIndex = -1; }
    }
    applyPhysics();
}