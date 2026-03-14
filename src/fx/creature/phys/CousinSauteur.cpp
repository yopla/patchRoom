#include "CousinSauteur.h"

//--------------------------------------------------------------
CousinSauteur::CousinSauteur(float startX, float startY) {
    pos.set(startX, startY);
    groundY = startY; // On considère que le point de spawn est le sol
    vel.set(0, 0);
    
    // Initialisation du corps
    numSegments = 6;
    float totalHeight = 160.0f;
    lengths.resize(numSegments);
    joints.resize(numSegments + 1);
    thickness.resize(numSegments + 1);
    leftPoints.resize(numSegments + 1);
    rightPoints.resize(numSegments + 1);

    for(int i=0; i<numSegments; i++) {
        lengths[i] = totalHeight / numSegments;
    }
    for(int i=0; i<=numSegments; i++) {
        joints[i].set(startX, startY - i * (totalHeight / numSegments));
        // Epaisseur qui diminue vers le haut
        thickness[i] = ofMap(i, 0, numSegments, 10.0f, 5.0f);
    }

    state = IDLE;
    stateTimer = ofRandom(0.5f, 2.0f);
    
    color = ofColor(100, 255, 150); // Vert un peu radioactif
    uniqueOffset = ofRandom(1000);
    
    // Init animation vars
    smoothHeightRatio = 1.0f;
    smoothBend = 0.0f;
    smoothArticulation = 0.0f;
    smoothAirBend = 0.0f;
    bFoldJump = false;
    randFoldFactors.resize(numSegments, 0.0f);
    
    // Ajout des poils
    for(int i=0; i<12; i++) {
        CousinHair hair;
        hair.setup(ofRandom(140, 290), 26);
        hair.attIndex = numSegments - 1; // Attaché au dernier segment (près de la tête/oeil)
        hair.attPct = ofRandom(0.3f, 0.9f); // Sur la partie haute du segment
        hair.attSide = (ofRandom(1.0f) > 0.5f);
        hair.color = ofColor(255, 255, 200, 180);
        hairs.push_back(hair);
    }
}

CousinSauteur::~CousinSauteur() {
    hairs.clear();
}

//--------------------------------------------------------------
void CousinSauteur::update(float mx, float my, float time) {
    // 1. Logique d'état
    // Interaction souris : Saut au survol
    if (state == IDLE && isInside(mx, my)) {
        state = PREPARE;
        stateTimer = 0.3f; // Réaction rapide
        // Direction aléatoire gauche ou droite au survol
        jumpDirection = (ofRandom(1.0f) > 0.5f) ? 1.0f : -1.0f;
        
        // Génération d'une torsion aléatoire pour la préparation (Premier pli unique)
        for(int i=0; i<numSegments; i++) {
             // On alterne globalement pour plier, mais avec du chaos
             float sign = (i % 2 == 0) ? 1.0f : -1.0f;
             // Parfois on inverse le sens pour casser la régularité
             if(ofRandom(1.0f) < 0.3f) sign *= -1.0f;
             randFoldFactors[i] = sign * ofRandom(0.5f, 2.0f); 
        }
    }

    updateState(time);
    
    // 2. Physique (Saut)
    updatePhysics();
    
    // 3. Animation Procédurale (IK/FK simulé)
    updateBodyAnimation();
    
    // 4. Mise à jour des poils (Suivent le corps)
    for(auto& h : hairs) {
        int idx = h.attIndex;
        if(idx >= joints.size()-1) idx = joints.size()-2;
        
        ofVec2f p1 = joints[idx];
        ofVec2f p2 = joints[idx+1];
        ofVec2f att = p1.getInterpolated(p2, h.attPct);
        
        // Offset latéral pour l'épaisseur
        ofVec2f dir = (p2 - p1).getNormalized();
        ofVec2f perp(-dir.y, dir.x);
        if(!h.attSide) perp *= -1;
        att += perp * 8.0f;
        
        h.update(att.x, att.y, time);
    }
}

//--------------------------------------------------------------
void CousinSauteur::updateState(float time) {
    // On utilise un delta fixe ou basé sur le temps
    float dt = 1.0f / 60.0f;
    stateTimer -= dt;

    switch(state) {
        case IDLE:
            if(stateTimer <= 0) {
                state = PREPARE;
                stateTimer = 0.5f; // Temps de préparation (s'écrase)
                
                // Génération d'une torsion aléatoire pour la préparation (Premier pli unique)
                for(int i=0; i<numSegments; i++) {
                     float sign = (i % 2 == 0) ? 1.0f : -1.0f;
                     if(ofRandom(1.0f) < 0.3f) sign *= -1.0f;
                     // Intensité un peu moins forte que l'impact d'atterrissage
                     randFoldFactors[i] = sign * ofRandom(0.5f, 2.0f); 
                }
            }
            break;
            
        case PREPARE:
            if(stateTimer <= 0) {
                state = JUMPING;
                // Impulsion
                vel.y = -ofRandom(18.0f, 25.0f); // Saut haut
                jumpDirection = (ofRandom(1.0f) > 0.5f) ? 1.0f : -1.0f;
                vel.x = ofRandom(4.0f, 8.0f) * jumpDirection;
                
                // On alterne le style de saut une fois sur deux
                bFoldJump = !bFoldJump;
            }
            break;
            
        case JUMPING:
            // La transition se fait via la collision sol dans updatePhysics
            break;
            
        case LANDING:
            if(stateTimer <= 0) {
                state = RECOVER;
                stateTimer = 0.4f; // Temps pour se redresser
            }
            break;
            
        case RECOVER:
            if(stateTimer <= 0) {
                state = IDLE;
                stateTimer = ofRandom(1.0f, 2.5f); // Pause avant prochain saut
            }
            break;
    }
}

//--------------------------------------------------------------
void CousinSauteur::updatePhysics() {
    if(state == JUMPING) {
        vel.y += 0.7f; // Gravité
        pos += vel;
        
        // Contact Sol
        if(pos.y >= groundY) {
            pos.y = groundY;
            vel.set(0,0);
            
            // Impact !
            state = LANDING;
            stateTimer = 0.2f; // Temps très court : il touche et se relève aussitôt
            
            // Génération d'une nouvelle torsion aléatoire pour TOUS les segments
            for(int i=0; i<numSegments; i++) {
                 // On alterne globalement pour plier, mais avec du chaos
                 float sign = (i % 2 == 0) ? 1.0f : -1.0f;
                 // Parfois on inverse le sens pour casser la régularité
                 if(ofRandom(1.0f) < 0.3f) sign *= -1.0f;
                 randFoldFactors[i] = sign * ofRandom(0.8f, 2.5f); // Intensité variable
            }
        }
    }
}

//--------------------------------------------------------------
void CousinSauteur::updateBodyAnimation() {
    // Calcul de la forme du corps selon l'état (Compression / Torsion)
    
    // 1. Définition des CIBLES (Targets) selon l'état
    float targetHeightRatio = 1.0f;
    float targetBend = 0.0f;
    float targetArticulation = 0.0f;
    float targetAirBend = 0.0f;
    
    if(state == IDLE) {
        // Respiration légère et continue
        targetHeightRatio = 0.95f + sin(ofGetElapsedTimef() * 3.0f + uniqueOffset) * 0.03f;
        targetBend = sin(ofGetElapsedTimef() * 2.0f + uniqueOffset) * 0.08f;
    } 
    else if(state == PREPARE) {
        // Anticipation : S'écrase et se tord
        targetHeightRatio = 0.5f; 
        targetBend = 0.2f * jumpDirection; // Légère inclinaison directionnelle
        targetArticulation = 1.0f; // Active le pliage aléatoire généré dans update/updateState
    } 
    else if(state == JUMPING) {
        if (bFoldJump) {
            // SAUT RECROQUEVILLÉ (PLI)
            targetHeightRatio = 0.5f; // Très compact
            targetArticulation = 1.8f; // Pliage extrême en Z
            targetAirBend = -vel.x * 0.01f; // Légère courbure seulement
        } else {
            // SAUT EN EXTENSION (NORMAL)
            targetHeightRatio = 1.35f; 
            targetArticulation = 0.0f;
            // Courbure dynamique selon la vitesse horizontale pour faire un bel arc
            targetAirBend = -vel.x * 0.04f;
        }
    } 
    else if(state == LANDING) {
        // Atterrissage : Compression douce et Pliage en Z (Articulation)
        targetHeightRatio = 0.6f; // Ne s'écrase pas totalement (0.6 au lieu de 0.4)
        targetArticulation = 1.0f; // Facteur multiplicateur pour randFoldFactors
    } 
    else if(state == RECOVER) {
        // Retour à la normale
        targetHeightRatio = 1.0f;
        targetArticulation = 0.0f;
        targetBend = 0.0f;
    }

    // 2. INTERPOLATION (Lissage)
    // Cela garantit qu'il n'y a jamais de "pop" ou de saute de frame, quelle que soit la transition d'état.
    float lerpSpeed = 0.15f; 
    if (state == JUMPING) lerpSpeed = 0.25f; // Extension rapide
    if (state == LANDING) lerpSpeed = 0.3f;  // Impact rapide (compression vive)
        
    smoothHeightRatio = ofLerp(smoothHeightRatio, targetHeightRatio, lerpSpeed);
    smoothBend = ofLerp(smoothBend, targetBend, lerpSpeed);
    smoothArticulation = ofLerp(smoothArticulation, targetArticulation, lerpSpeed);
    smoothAirBend = ofLerp(smoothAirBend, targetAirBend, lerpSpeed);
    
    // 3. Construction FK avec valeurs lissées
    joints[0] = pos;
    float currentY = pos.y;
    float currentX = pos.x;
    float segH = (lengths[0] * numSegments * smoothHeightRatio) / numSegments;
    
    int zigZagSign = 1; // Pour alterner gauche/droite
    
    for(int i=0; i<numSegments; i++) {
        // Angle de base (vertical vers le haut = -PI/2, mais ici on bosse en relatif)
        float angle = 0; 
        
        // Ajout de la torsion (ZigZag)
        if(fabs(smoothBend) > 0.001f) {
            angle = smoothBend * zigZagSign;
            zigZagSign *= -1;
        }
        
        // Articulation spéciale atterrissage (Pli en 3 parties / Accordéon)
        if(smoothArticulation > 0.01f) {
            // Utilise les facteurs aléatoires générés à l'impact
            angle += smoothArticulation * randFoldFactors[i];
        }
        
        // Incurvation globale si saut
        angle += smoothAirBend * ((float)i / numSegments);

        // Calcul position segment suivant
        // On monte (Y négatif) avec un offset X selon l'angle
        float dx = sin(angle) * segH * 1.5f; // Exagère le X pour le style
        float dy = -cos(angle) * segH;
        
        currentX += dx;
        currentY += dy;
        
        joints[i+1].set(currentX, currentY);
    }
    
    // Calcul des points de contour pour le dessin
    for (int i = 0; i <= numSegments; i++) {
        ofVec2f p = joints[i];
        ofVec2f dir;

        if (i == 0) { // Base
            dir = joints[i+1] - joints[i];
        } else if (i == numSegments) { // Tête
            dir = joints[i] - joints[i-1];
        } else { // Jointures
            dir = joints[i+1] - joints[i-1];
        }

        ofVec2f perp = dir.getNormalized().getPerpendicular();
        float t = thickness[i];
        leftPoints[i] = p + perp * t;
        rightPoints[i] = p - perp * t;
        
        // Contrainte : Le contour ne doit jamais dépasser le sol (Y augmente vers le bas)
        if (leftPoints[i].y > groundY) leftPoints[i].y = groundY;
        if (rightPoints[i].y > groundY) rightPoints[i].y = groundY;
    }
}

//--------------------------------------------------------------
void CousinSauteur::draw() {
    // Dessin des poils (derrière)
    for(auto& h : hairs) h.draw();

    // Construction du polygone pour le corps
    ofPolyline contour;
    if (!leftPoints.empty() && !rightPoints.empty()) {
        // On utilise des curve vertex pour un rendu lisse
        contour.curveTo(leftPoints[0].x, leftPoints[0].y);
        for(const auto& p : leftPoints) contour.curveTo(p.x, p.y);
        contour.curveTo(leftPoints.back().x, leftPoints.back().y);
        
        contour.curveTo(rightPoints.back().x, rightPoints.back().y);
        for(int i = rightPoints.size() - 1; i >= 0; --i) contour.curveTo(rightPoints[i].x, rightPoints[i].y);
        contour.curveTo(rightPoints[0].x, rightPoints[0].y);
        contour.close();
    }

    // Dessin du corps
    ofPushStyle();
    
    // 1. Remplissage
    ofFill();
    ofSetColor(color);
    if(contour.size() > 0) contour.draw();

    // 2. Contour blanc
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(2);
    if(contour.size() > 0) contour.draw();
    
    // Oeil (sur le dernier segment)
    ofVec2f head = joints.back();
    ofFill();
    ofSetColor(255);
    ofDrawCircle(head, 12);
    ofSetColor(0);
    float lookX = (state == JUMPING) ? (vel.x > 0 ? 3 : -3) : 0;
    ofDrawCircle(head.x + lookX, head.y, 4);
    
    ofPopStyle();
}

bool CousinSauteur::isInside(float mx, float my) {
    // On teste la distance par rapport au centre du corps (environ 80px au dessus du pied)
    // pour que ce soit plus facile de le survoler
    ofVec2f centerBody = pos - ofVec2f(0, 80);
    return (centerBody.distance(ofVec2f(mx, my)) < 80);
}
