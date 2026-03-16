#include "TripodEyeLayer.h"

//--------------------------------------------------------------
// TRIPOD EYE (Créature)
//--------------------------------------------------------------
void TripodEye::setup(float x, float y) {
    bodyRadius = 35.0f;
    bodyPos.set(x, y - 100); // Le corps commence en hauteur
    bodyVel.set(0, 0);
    lookTarget.set(x, y);
    movingLegIndex = -1; // Aucune patte ne bouge au début

    // Configuration des 3 pattes (Arrière, Milieu, Avant)
    float offsetsX[3] = {-25, 0, 25};          // Attaches sous l'oeil
    float idealGndX[3] = {-60, 0, 60};         // Cibles idéales au sol
    float bendDirs[3] = {-1, -1, 1};           // Plis : Arrière, Arrière, Avant

    for (int i = 0; i < 3; i++) {
        TripodLeg leg;
        leg.rootOffset.set(offsetsX[i], bodyRadius * 0.8f);
        leg.footPos.set(x + idealGndX[i], y);
        leg.footTarget = leg.footPos;
        
        leg.L1 = 65.0f; // Longueur Cuisse
        leg.L2 = 75.0f; // Longueur Mollet
        
        leg.isMoving = false;
        leg.moveProgress = 0.0f;
        leg.stepSpeed = 0.12f; // Vitesse d'un pas
        leg.bendDir = bendDirs[i];
        leg.idealGroundOffsetX = idealGndX[i];
        
        legs.push_back(leg);
    }
}

void TripodEye::update(float mx, float my, float groundY) {
    lookTarget.set(mx, my);
    
    // 1. Mouvement du corps
    // Le corps essaie de suivre la cible X avec un easing
    float diffX = mx - bodyPos.x;
    bodyVel.x = diffX * 0.03f;
    
    // Vitesse max pour laisser le temps aux pattes de suivre
    bodyVel.x = ofClamp(bodyVel.x, -4.0f, 4.0f); 
    bodyPos.x += bodyVel.x;

    // Gestion de la hauteur du corps (amortisseur basé sur la position des pieds)
    float avgFootY = 0;
    for (auto& leg : legs) avgFootY += leg.footPos.y;
    avgFootY /= 3.0f;
    
    float targetBodyY = avgFootY - 100.0f;
    // Petit rebond du corps quand une patte bouge
    if (movingLegIndex != -1) {
        targetBodyY -= sin(legs[movingLegIndex].moveProgress * PI) * 12.0f;
    }
    bodyPos.y += (targetBodyY - bodyPos.y) * 0.15f;

    // 2. Scheduler de Marche (Garantit qu'une seule patte bouge à la fois)
    if (movingLegIndex == -1) {
        float maxError = 0;
        int candidate = -1;

        for (int i = 0; i < 3; i++) {
            float idealX = bodyPos.x + legs[i].idealGroundOffsetX;
            float error = abs(legs[i].footPos.x - idealX);
            
            // Seuil à partir duquel la patte est trop loin et doit faire un pas
            if (error > 40.0f && error > maxError) {
                maxError = error;
                candidate = i;
            }
        }

        if (candidate != -1) {
            movingLegIndex = candidate;
            auto& leg = legs[movingLegIndex];
            leg.isMoving = true;
            leg.moveProgress = 0.0f;
            leg.startPos = leg.footPos;
            
            // Cible projetée : on anticipe la position selon la vitesse actuelle
            float idealX = bodyPos.x + leg.idealGroundOffsetX;
            leg.footTarget.set(idealX + bodyVel.x * 15.0f, groundY); 
        }
    } else {
        // 3. Animation de la patte en mouvement
        auto& leg = legs[movingLegIndex];
        leg.moveProgress += leg.stepSpeed;
        
        if (leg.moveProgress >= 1.0f) {
            leg.moveProgress = 1.0f;
            leg.footPos = leg.footTarget;
            leg.isMoving = false;
            movingLegIndex = -1; // Libère le scheduler pour la patte suivante
        } else {
            // Interpolation position au sol + arc pour lever le pied
            leg.footPos.x = ofLerp(leg.startPos.x, leg.footTarget.x, leg.moveProgress);
            // Base linéaire Y (en cas de pente)
            float baseY = ofLerp(leg.startPos.y, leg.footTarget.y, leg.moveProgress);
            // Hauteur du pas (sinus)
            leg.footPos.y = baseY - sin(leg.moveProgress * PI) * 35.0f;
        }
    }

    // 4. Résolution de l'Inverse Kinematics (IK) pour placer le genou
    for (auto& leg : legs) {
        ofVec2f root = bodyPos + leg.rootOffset;
        float d = root.distance(leg.footPos);
        float maxLen = leg.L1 + leg.L2;
        
        if (d >= maxLen - 0.1f) {
            // Si trop loin, la jambe se tend complètement (Ligne droite)
            ofVec2f dir = (leg.footPos - root).getNormalized();
            leg.kneePos = root + dir * leg.L1;
            leg.footPos = root + dir * maxLen; // Empêche l'arrachement visuel
        } else {
            // Loi des Cosinus pour trouver l'angle de l'articulation
            float cosVal = (leg.L1*leg.L1 + d*d - leg.L2*leg.L2) / (2 * leg.L1 * d);
            cosVal = ofClamp(cosVal, -1.0f, 1.0f); // Sécurité mathématique
            
            float angle1 = acos(cosVal);
            float baseAngle = atan2(leg.footPos.y - root.y, leg.footPos.x - root.x);
            
            float kneeAngle = baseAngle + angle1 * leg.bendDir;
            leg.kneePos = root + ofVec2f(cos(kneeAngle), sin(kneeAngle)) * leg.L1;
        }
    }
}

void TripodEye::draw() {
    ofPushStyle();
    
    // Dessin des pattes
    ofSetLineWidth(6);
    for (int i = 0; i < 3; i++) {
        // Légère variation de couleur pour donner de la profondeur (la patte du milieu est "derrière")
        if (i == 1) ofSetColor(50, 50, 60); 
        else ofSetColor(120, 120, 130);
        
        auto& leg = legs[i];
        ofVec2f root = bodyPos + leg.rootOffset;
        
        // Segments (Cuisse et Mollet)
        ofDrawLine(root, leg.kneePos);
        ofDrawLine(leg.kneePos, leg.footPos);
        
        // Articulations (Genou et Pied)
        ofSetColor(200, 80, 80); // Rougearticulation
        ofDrawCircle(leg.kneePos, 5);
        ofDrawCircle(leg.footPos, 6);
    }

    // Dessin de l'œil (Corps)
    ofSetColor(255);
    ofFill();
    ofDrawCircle(bodyPos, bodyRadius);
    
    ofSetColor(0);
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawCircle(bodyPos, bodyRadius);
    
    // Dessin de la pupille orientée
    ofFill();
    ofSetColor(40, 180, 220); // Bleu vif
    ofVec2f lookDir = (lookTarget - bodyPos).getNormalized();
    float lookDist = std::min(15.0f, bodyPos.distance(lookTarget) * 0.15f);
    ofDrawCircle(bodyPos + lookDir * lookDist, 14);
    
    // Reflet dans l'oeil
    ofSetColor(255);
    ofDrawCircle(bodyPos + lookDir * lookDist + ofVec2f(-4, -4), 4);
    
    ofPopStyle();
}

//--------------------------------------------------------------
// TRIPOD EYE LAYER MANAGER
//--------------------------------------------------------------
void TripodEyeLayer::setup(float w, float h, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    collider = col;
    
    // On initialise au milieu de l'écran, près du sol
    creature.setup(w * 0.5f, h - 50);
}

void TripodEyeLayer::update(float mx, float my, float time) {
    // Par défaut, le sol est en bas de l'écran
    float groundY = simHeight - 20.0f;
    
    // Optionnel : Intégration du collider si tu l'utilises pour des plateformes
    /*
    if(collider) {
        // Logique de raycast pour trouver le sol exact sous le bodyPos.x
    }
    */
    
    creature.update(mx, my, groundY);
}

void TripodEyeLayer::draw() {
    creature.draw();
}