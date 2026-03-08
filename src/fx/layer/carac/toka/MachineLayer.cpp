#include "MachineLayer.h"

//--------------------------------------------------------------
void MachineLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    floorY = h; // Le sol est en bas de la zone

    // Création initiale au centre
    createMachine(w * 0.5f, h - 200);
}

//--------------------------------------------------------------
void MachineLayer::createMachine(float x, float y) {
    particles.clear();
    constraints.clear();
    motorAngle = 0;

    // --- CONSTRUCTION DE LA MACHINE ANIMALE ---
    // Structure inspirée d'un marcheur mécanique simple
    
    float scale = 1.5f;

    // 0. Corps Central (Triangle)
    int pBody1 = addParticle(x - 40*scale, y - 40*scale);
    int pBody2 = addParticle(x + 40*scale, y - 40*scale);
    int pBody3 = addParticle(x, y); // Centre bas
    bodyCenterIdx = pBody3;

    addConstraint(pBody1, pBody2);
    addConstraint(pBody2, pBody3);
    addConstraint(pBody3, pBody1);

    // 1. Le Moteur (Crank) - Particule cinématique (bFixed = true)
    // Elle tournera autour de pBody3
    crankIdx = addParticle(x, y, true); 
    
    // 2. Jambes (Legs)
    // On crée 2 jambes connectées au moteur et au corps
    
    for(int i=0; i<2; i++) {
        float dir = (i==0) ? -1.0f : 1.0f; // Gauche / Droite
        
        // Genou (Knee)
        int pKnee = addParticle(x + (60*scale * dir), y);
        
        // Pied (Foot)
        int pFoot = addParticle(x + (50*scale * dir), y + 100*scale);
        
        // Connexions
        // Le genou est relié au corps (Hanche)
        addConstraint((i==0 ? pBody1 : pBody2), pKnee);
        
        // Le genou est relié au moteur (C'est ce qui crée le mouvement)
        addConstraint(crankIdx, pKnee);
        
        // Le pied est relié au genou
        addConstraint(pKnee, pFoot);
        
        // Le pied est relié au corps (pour guider le mouvement)
        addConstraint(pBody3, pFoot);
        
        // Barre de support supplémentaire pour rigidifier le pied
        addConstraint((i==0 ? pBody1 : pBody2), pFoot, 0.5f, false);
    }
    
    // 3. Tête / Queue (Décoratif)
    int pHead = addParticle(x + 70*scale, y - 60*scale);
    addConstraint(pBody2, pHead);
    addConstraint(pBody3, pHead);
    
    int pTail = addParticle(x - 70*scale, y - 50*scale);
    addConstraint(pBody1, pTail);
    addConstraint(pBody3, pTail);
}

//--------------------------------------------------------------
int MachineLayer::addParticle(float x, float y, bool fixed) {
    MParticle p;
    p.pos.set(x, y);
    p.oldPos.set(x, y);
    p.bFixed = fixed;
    particles.push_back(p);
    return particles.size() - 1;
}

void MachineLayer::addConstraint(int i1, int i2, float stiff, bool visible) {
    MConstraint c;
    c.p1 = i1;
    c.p2 = i2;
    c.length = particles[i1].pos.distance(particles[i2].pos);
    c.stiffness = stiff;
    c.bVisible = visible;
    constraints.push_back(c);
}

//--------------------------------------------------------------
void MachineLayer::update(float mouseX, float mouseY, float time) {
    
    // 1. Animation du Moteur (Cinématique)
    motorAngle += motorSpeed;
    
    // Le point moteur tourne autour du centre du corps
    if(crankIdx != -1 && bodyCenterIdx != -1) {
        ofVec2f center = particles[bodyCenterIdx].pos;
        float radius = 30.0f; // Rayon du vilebrequin
        
        particles[crankIdx].pos.x = center.x + cos(motorAngle) * radius;
        particles[crankIdx].pos.y = center.y + sin(motorAngle) * radius;
        // On reset oldPos pour ne pas accumuler de vélocité physique sur ce point
        particles[crankIdx].oldPos = particles[crankIdx].pos; 
    }

    // 2. Interaction Souris (Attraction du corps)
    if(ofGetMousePressed()) {
        ofVec2f m(mouseX, mouseY);
        ofVec2f dir = m - particles[bodyCenterIdx].pos;
        float dist = dir.length();
        if(dist > 10 && dist < 400) {
            dir.normalize();
            particles[bodyCenterIdx].force += dir * 0.5f;
        }
    }

    // 3. Physique
    solveVerlet();
    solveConstraints();
    checkFloor();
    
    // Wrapping horizontal (La machine traverse l'écran)
    float cx = particles[bodyCenterIdx].pos.x;
    if(cx > simWidth + 100) {
        float offset = simWidth + 200;
        for(auto& p : particles) { p.pos.x -= offset; p.oldPos.x -= offset; }
    } else if(cx < -100) {
        float offset = simWidth + 200;
        for(auto& p : particles) { p.pos.x += offset; p.oldPos.x += offset; }
    }
}

//--------------------------------------------------------------
void MachineLayer::mousePressed(float x, float y, int button) {
    if (bodyCenterIdx != -1) {
        ofVec2f m(x, y);
        // Si on clique sur la machine (rayon approximatif de 150px)
        if (m.distance(particles[bodyCenterIdx].pos) < 150) {
            // Impulsion de saut (Force vers le haut)
            particles[bodyCenterIdx].force.y -= 20.0f; 
            // Petit décalage aléatoire pour dynamiser le saut
            particles[bodyCenterIdx].force.x += ofRandom(-5, 5);
        }
    }
}

//--------------------------------------------------------------
void MachineLayer::solveVerlet() {
    for(auto& p : particles) {
        if(p.bFixed) continue;
        
        ofVec2f vel = (p.pos - p.oldPos) * 0.99f; // Friction air
        p.oldPos = p.pos;
        
        // Gravité
        p.force.y += 0.4f;
        
        p.pos += vel + p.force;
        p.force.set(0,0);
    }
}

//--------------------------------------------------------------
void MachineLayer::solveConstraints() {
    // Plusieurs itérations pour rigidifier la structure
    for(int k=0; k<5; k++) {
        for(auto& c : constraints) {
            MParticle& p1 = particles[c.p1];
            MParticle& p2 = particles[c.p2];
            
            ofVec2f delta = p1.pos - p2.pos;
            float dist = delta.length();
            if(dist == 0) continue;
            
            float diff = (dist - c.length) / dist;
            ofVec2f offset = delta * 0.5f * diff * c.stiffness;
            
            if(!p1.bFixed) p1.pos -= offset;
            if(!p2.bFixed) p2.pos += offset;
            
            // Si l'un est fixe, l'autre prend tout
            if(p1.bFixed && !p2.bFixed) p2.pos -= offset; // Correction sens
            if(!p1.bFixed && p2.bFixed) p1.pos += offset; // Correction sens
        }
    }
}

//--------------------------------------------------------------
void MachineLayer::checkFloor() {
    for(auto& p : particles) {
        if(!p.bFixed && p.pos.y > floorY) {
            ofVec2f vel = p.pos - p.oldPos;
            
            p.pos.y = floorY;
            p.oldPos.y = p.pos.y + vel.y * 0.6f; // Friction sol
            p.oldPos.x = p.pos.x - vel.x * 0.6f; // Friction sol X (adhérence)
        }
    }
}

//--------------------------------------------------------------
void MachineLayer::draw() {
    ofPushStyle();
    ofSetLineWidth(6);
    
    // Dessin des contraintes (Barres)
    ofSetColor(255);
    for(auto& c : constraints) {
        if(c.bVisible) {
            ofDrawLine(particles[c.p1].pos, particles[c.p2].pos);
        }
    }
    
    // Dessin des particules (Jointures)
    ofFill();
    for(int i=0; i<particles.size(); i++) {
        if(i == crankIdx) ofSetColor(255, 0, 0); // Moteur en rouge
        else ofSetColor(255);
        
        ofDrawCircle(particles[i].pos, 5);
    }
    
    ofPopStyle();
}
