#include "MachineAutoLayer.h"

//--------------------------------------------------------------
void MachineAutoLayer::setup(float w, float h) {
    sceneWidth = w;
    groundY = h;
    
    motorAngle = 0.0f;
    motorSpeed = 0.25f; // Vitesse de marche
    direction = 1.0f;
    bodyColor = ofColor(200, 100, 80);
    
    // Création de l'insecte au centre
    createCreature(w * 0.5f, h - 150);
}

//--------------------------------------------------------------
void MachineAutoLayer::createCreature(float x, float y) {
    particles.clear();
    constraints.clear();
    
    float scale = 1.4f;
    
    // 1. CORPS SEGMENTÉ (Tête -> Thorax -> Abdomen)
    int pHead = addParticle(x + 40*scale, y - 50*scale, 0.8f);
    int pThorax = addParticle(x, y - 40*scale, 1.2f);
    int pAbdomen = addParticle(x - 50*scale, y - 30*scale, 1.0f);
    
    headIdx = pHead;
    thoraxIdx = pThorax;
    
    // Colonne vertébrale (semi-rigide)
    addConstraint(pHead, pThorax, 0.9f);
    addConstraint(pThorax, pAbdomen, 0.9f);
    // Muscle long pour tenir la posture
    addConstraint(pHead, pAbdomen, 0.5f, false);
    
    // 2. MOTEUR (Crank) - Le "coeur" mécanique
    crankIdx = addParticle(x, y - 40*scale, 0.1f, true); 
    
    // 3. PATTES (Hybrides)
    for(int i=0; i<2; i++) {
        // Patte Avant (liée au Thorax) et Patte Arrière (liée à l'Abdomen)
        int parent = (i==0) ? pThorax : pAbdomen;
        float legOffset = (i==0) ? 0 : -PI; // Déphasage pour la marche
        
        // Genou
        int pKnee = addParticle(x, y, 0.5f);
        // Pied
        int pFoot = addParticle(x, y + 80*scale, 0.5f);
        
        // Connexions anatomiques
        addConstraint(parent, pKnee, 1.0f); // Cuisse
        addConstraint(pKnee, pFoot, 1.0f);  // Tibia
        addConstraint(parent, pFoot, 0.05f, false); // Muscle mou de rappel
        
        // MOTEUR -> GENOU (La bielle mécanique)
        // C'est ce lien qui transforme la rotation en mouvement de patte
        addConstraint(crankIdx, pKnee, 1.0f, true);
    }
    
    // 4. ORGANES SENSORIELS (Antenne / Queue)
    int pAntenna = addParticle(x + 60*scale, y - 80*scale, 0.1f);
    addConstraint(pHead, pAntenna, 0.2f);
    
    int pTail = addParticle(x - 80*scale, y - 20*scale, 0.2f);
    addConstraint(pAbdomen, pTail, 0.3f);
}

//--------------------------------------------------------------
int MachineAutoLayer::addParticle(float x, float y, float mass, bool fixed) {
    AutoParticle p;
    p.pos = glm::vec2(x, y);
    p.oldPos = glm::vec2(x, y);
    p.mass = mass;
    p.bFixed = fixed;
    particles.push_back(p);
    return particles.size() - 1;
}

void MachineAutoLayer::addConstraint(int i1, int i2, float stiffness, bool visible) {
    AutoConstraint c;
    c.p1 = i1;
    c.p2 = i2;
    c.length = glm::distance(particles[i1].pos, particles[i2].pos);
    c.stiffness = stiffness;
    c.bVisible = visible;
    constraints.push_back(c);
}

//--------------------------------------------------------------
void MachineAutoLayer::update() {
    // 1. MOTEUR ORGANIQUE
    // Variation de vitesse (fatigue/énergie)
    float noise = ofNoise(ofGetFrameNum() * 0.01f);
    float currentSpeed = motorSpeed * (0.8f + noise * 0.4f);
    
    motorAngle += currentSpeed * direction;
    
    if(crankIdx != -1 && thoraxIdx != -1) {
        glm::vec2 center = particles[thoraxIdx].pos;
        float radius = 30.0f;
        
        // Le moteur tourne autour du Thorax
        particles[crankIdx].pos.x = center.x + cos(motorAngle) * radius;
        particles[crankIdx].pos.y = center.y + sin(motorAngle) * radius;
        particles[crankIdx].oldPos = particles[crankIdx].pos;
    }
    
    // 2. COMPORTEMENT (Regard)
    if(headIdx != -1) {
        // La tête oscille un peu pour "chercher"
        float lookY = sin(ofGetFrameNum() * 0.05f) * 2.0f;
        particles[headIdx].force.y += lookY;
    }
    
    // 3. PHYSIQUE
    solveVerlet();
    solveConstraints();
    checkFloor();
    
    // 4. NAVIGATION (Rebond bords)
    if(thoraxIdx != -1) {
        float cx = particles[thoraxIdx].pos.x;
        float margin = 150.0f;
        
        if(cx > sceneWidth - margin && direction > 0) {
            direction = -1.0f; // Demi-tour
        }
        else if(cx < margin && direction < 0) {
            direction = 1.0f; // Demi-tour
        }
    }
}

//--------------------------------------------------------------
void MachineAutoLayer::solveVerlet() {
    for(auto& p : particles) {
        if(p.bFixed) continue;
        
        glm::vec2 vel = (p.pos - p.oldPos) * 0.97f; // Friction air
        p.oldPos = p.pos;
        
        // Gravité
        p.force.y += 0.5f * p.mass;
        
        p.pos += vel + p.force;
        p.force = glm::vec2(0,0);
    }
}

//--------------------------------------------------------------
void MachineAutoLayer::solveConstraints() {
    for(int k=0; k<5; k++) { // 5 itérations
        for(auto& c : constraints) {
            AutoParticle& p1 = particles[c.p1];
            AutoParticle& p2 = particles[c.p2];
            
            glm::vec2 delta = p1.pos - p2.pos;
            float dist = glm::length(delta);
            if(dist == 0) continue;
            
            float diff = (dist - c.length) / dist;
            glm::vec2 offset = delta * 0.5f * diff * c.stiffness;
            
            if(!p1.bFixed) p1.pos -= offset;
            if(!p2.bFixed) p2.pos += offset;
            
            if(p1.bFixed && !p2.bFixed) p2.pos -= offset;
            if(!p1.bFixed && p2.bFixed) p1.pos += offset;
        }
    }
}

//--------------------------------------------------------------
void MachineAutoLayer::checkFloor() {
    for(auto& p : particles) {
        if(!p.bFixed && p.pos.y > groundY) {
            glm::vec2 vel = p.pos - p.oldPos;
            p.pos.y = groundY;
            p.oldPos.y = p.pos.y + vel.y * 0.5f; // Friction sol Y
            p.oldPos.x = p.pos.x - vel.x * 0.8f; // Friction sol X (Grip fort pour marcher)
        }
    }
}

//--------------------------------------------------------------
void MachineAutoLayer::draw() {
    ofPushStyle();
    
    // 1. SQUELETTE (Mécanique)
    ofSetLineWidth(3);
    ofSetColor(80);
    for(auto& c : constraints) {
        if(c.bVisible) {
            ofDrawLine(particles[c.p1].pos, particles[c.p2].pos);
        }
    }
    
    // 2. CARAPACE (Organique)
    for(int i=0; i<particles.size(); i++) {
        if(i == crankIdx) {
            ofSetColor(255, 50, 50); // Moteur rouge
            ofDrawCircle(particles[i].pos, 5);
        }
        else if(i == headIdx || i == thoraxIdx) {
            // Grosses parties du corps
            ofSetColor(bodyColor);
            ofDrawCircle(particles[i].pos, 15);
            // Reflet
            ofSetColor(255, 100);
            ofDrawCircle(particles[i].pos.x - 3, particles[i].pos.y - 3, 5);
        }
        else {
            // Articulations
            ofSetColor(bodyColor.r * 0.8, bodyColor.g * 0.8, bodyColor.b * 0.8);
            ofDrawCircle(particles[i].pos, 6);
        }
    }
    
    ofPopStyle();
}

//--------------------------------------------------------------
void MachineAutoLayer::mousePressed(float x, float y) {
    if(thoraxIdx != -1) {
        float dist = glm::distance(glm::vec2(x, y), particles[thoraxIdx].pos);
        if(dist < 100) {
            direction *= -1.0f; // Changement de direction manuel
            bodyColor.setHue(ofRandom(255));
            
            // Petit saut pour le feedback
            particles[thoraxIdx].force.y -= 15.0f;
            particles[headIdx].force.y -= 10.0f;
        }
    }
}