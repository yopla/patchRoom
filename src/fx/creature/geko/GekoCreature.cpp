#include "GekoCreature.h"

//--------------------------------------------------------------
GekoCreature::GekoCreature(float startX, float startY, int nLegs) {
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
void GekoCreature::setLegCount(int n) {
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
//--------------------------------------------------------------
void GekoCreature::update(float mx, float my) {
    
    // 1. DIRECTION ET FORCE VERS LA SOURIS
    currentTarget = glm::vec2(mx, my);
    glm::vec2 toTarget = currentTarget - bodyPos;
    float distToTarget = glm::length(toTarget);

    // On applique une force vers la souris pour que le corps "tire" sur les pattes
    if (distToTarget > 5.0f) {
        glm::vec2 force = glm::normalize(toTarget) * 0.8f; 
        bodyVel += force;
    }

    // Orientation du corps vers la souris
    float targetAngle = atan2(toTarget.y, toTarget.x);
    // Lissage de l'angle pour éviter les rotations brusques
    wanderAngle = ofLerpDegrees(wanderAngle, ofRadToDeg(targetAngle), 0.1f);
    float bodyAngleRadians = ofDegToRad(wanderAngle);

    // 2. LOGIQUE DE MARCHE SÉQUENTIELLE
    // On ne cherche plus la "pire" patte, on les fait marcher une par une
    
    if (movingLegIndex == -1) {
        // On vérifie si la prochaine patte dans la liste a besoin de bouger
        // On utilise un compteur simple pour passer de 0 -> 1 -> 2 -> 0...
        static int nextLegToMove = 0;
        
        // Sécurité : Si le nombre de pattes a changé dynamiquement
        if (nextLegToMove >= legs.size()) nextLegToMove = 0;
        
        GekoLeg &leg = legs[nextLegToMove];
        
        // Calcul de la position idéale pour CETTE patte
        float globalLegAngle = bodyAngleRadians + leg.angleOffset;
        glm::vec2 idealPos = bodyPos + glm::vec2(cos(globalLegAngle), sin(globalLegAngle)) * legReach;
        
        // Si le corps s'est assez éloigné de la position actuelle de la patte
        if (glm::distance(leg.position, idealPos) > legReach * 0.8f) {
            movingLegIndex = nextLegToMove;
            leg.isMoving = true;
            leg.moveProgress = 0.0f;
            
            // On projette la cible de la patte un peu en avant pour anticiper le mouvement
            leg.target = idealPos + glm::normalize(toTarget) * 20.0f;
            
            // On prépare l'index de la prochaine patte pour le prochain cycle
            nextLegToMove = (nextLegToMove + 1) % numLegs;
        }
    } 
    else {
        // Animation du pas (interpolation)
        GekoLeg &leg = legs[movingLegIndex];
        leg.moveProgress += 0.1f; // Vitesse du pas
        
        // Arc de cercle pour donner l'impression que la patte se lève
        // On utilise mix pour la position et on pourrait ajouter un offset visuel ici
        leg.position = glm::mix(leg.position, leg.target, 0.2f);
        
        if (leg.moveProgress >= 1.0f || glm::distance(leg.position, leg.target) < 1.0f) {
            leg.position = leg.target;
            leg.isMoving = false;
            movingLegIndex = -1; // Libère le verrou pour la patte suivante
        }
    }

    // 3. PHYSIQUE DU CORPS
    for(auto& leg : legs) {
        // Les pattes au sol tirent/retiennent le corps
        if(!leg.isMoving) {
            glm::vec2 pull = leg.position - bodyPos;
            float d = glm::length(pull);
            
            // Force de ressort : le corps essaie de rester à 'legReach' de ses pattes
            float stretch = d - legReach;
            bodyVel += glm::normalize(pull) * stretch * springStiffness;
        }
    }

    // Friction et application du mouvement
    bodyVel *= friction;
    bodyPos += bodyVel;
}
//--------------------------------------------------------------
void GekoCreature::draw() {
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