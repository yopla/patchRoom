#include "WingedWormSystem.h"

void WingedWormSystem::setup(int count) {
    agents.resize(count);
    for(auto& a : agents) {
        // Position aléatoire dans la pièce
        a.pos.set(ofRandom(-800, 800), ofRandom(200, 1200), ofRandom(-800, 800));
        a.target = a.pos;
        a.vel.set(0,0,0);
        a.speed = ofRandom(3, 6);
        a.wingSpeed = ofRandom(0.5, 1.2);
        a.color = ofColor::fromHsb(ofRandom(255), 150, 255);
        
        // Initialisation du corps
        a.numBodySegments = 3; // Corps tout petit
        for(int i=0; i<a.numBodySegments; i++) a.body.push_back(a.pos);
        
        // Initialisation du fil
        a.numThreadSegments = 5;
        for(int i=0; i<a.numThreadSegments; i++) {
            a.thread.push_back(a.pos);
            a.threadOld.push_back(a.pos);
        }
    }
}

void WingedWormSystem::update() {
    float time = ofGetElapsedTimef();
    ofVec3f boundsMin(-1200, 0, -1200);
    ofVec3f boundsMax(1200, 1400, 1200);

    for(auto& a : agents) {
        // 1. Déplacement (Vol libre)
        if(a.pos.distance(a.target) < 150 || ofRandom(1.0) < 0.01) {
            // Génération polaire pour éviter le centre sans boucle "do-while"
            float angle = ofRandom(TWO_PI);
            float radius = ofRandom(1000, 1100); // Entre 500 (centre exclu) et 1100 (murs)
            
            a.target.set(cos(angle) * radius,
                         ofRandom(boundsMin.y, boundsMax.y),
                         sin(angle) * radius);
        }
        
        ofVec3f dir = (a.target - a.pos).getNormalized();
        // Ajout de bruit pour un mouvement organique
        dir.x += ofSignedNoise(time, a.pos.y * 0.005) * 0.8;
        dir.y += ofSignedNoise(time + 100, a.pos.x * 0.005) * 0.8;
        dir.z += ofSignedNoise(time + 200, a.pos.z * 0.005) * 0.8;
        
        // Force de répulsion du centre (pour ne pas le traverser en ligne droite)
        ofVec2f flatPos(a.pos.x, a.pos.z);
        float distToCenter = flatPos.length();
        if(distToCenter < 500) {
            ofVec2f push = flatPos.getNormalized() * ofMap(distToCenter, 0, 500, 1.5, 0.0);
            dir.x += push.x;
            dir.z += push.y;
        }
        
        dir.normalize();
        
        a.vel = a.vel * 0.92 + dir * 0.08;
        a.pos += a.vel * a.speed;
        
        // 2. Mise à jour du corps (Follow)
        a.body[0] = a.pos;
        for(size_t i=1; i<a.body.size(); i++) {
            ofVec3f d = a.body[i] - a.body[i-1];
            if(d.length() > 4) { // Segments très rapprochés pour un petit corps
                d.normalize();
                a.body[i] = a.body[i-1] + d * 4;
            }
        }
        
        // 3. Mise à jour du fil (Verlet Physics + Gravité)
        ofVec3f gravity(0, -1.5, 0); // Gravité plus forte vers le sol
        ofVec3f anchor = a.body.back(); // Attaché à la queue du ver
        
        for(size_t i=0; i<a.thread.size(); i++) {
            ofVec3f& p = a.thread[i];
            ofVec3f& oldP = a.threadOld[i];
            
            ofVec3f vel = (p - oldP) * 0.95; // Friction
            oldP = p;
            p += vel + gravity;
            
            // Contrainte de distance
            ofVec3f target = (i==0) ? anchor : a.thread[i-1];
            ofVec3f diff = p - target;
            float dist = diff.length();
            if(dist > 10) {
                diff.normalize();
                p = target + diff * 10;
            }
        }
        
        // 4. Animation des ailes
        a.wingAngle = sin(time * 35.0 * a.wingSpeed) * 45.0;
    }
}

void WingedWormSystem::draw() {
    ofPushStyle();
    for(auto& a : agents) {
        // Dessin du corps
        ofSetColor(a.color);
        ofSetLineWidth(4);
        ofNoFill();
        ofBeginShape();
        for(auto& p : a.body) ofVertex(p);
        ofEndShape();
        
        // Dessin du fil
        ofSetColor(255, 200);
        ofSetLineWidth(2);
        ofBeginShape();
        ofVertex(a.body.back());
        for(auto& p : a.thread) ofVertex(p);
        ofEndShape();
        
        // Dessin de la Tête (Yeux + Ailes)
        ofPushMatrix();
        ofTranslate(a.pos);
        
        // Orientation selon la vitesse
        ofVec3f vel = a.vel.getNormalized();
        ofVec3f up(0,1,0);
        ofVec3f axis = up.cross(vel);
        float angle = up.angle(vel);
        ofRotate(angle, axis.x, axis.y, axis.z);
        
        // Yeux Blancs + Pupilles
        ofSetColor(255);
        ofDrawSphere(-3, 0, 3, 2.5); // Oeil G (réduit)
        ofDrawSphere(3, 0, 3, 2.5);  // Oeil D (réduit)
        ofSetColor(0);
        ofDrawSphere(-3, 0, 5, 1.0); // Pupille G
        ofDrawSphere(3, 0, 5, 1.0);  // Pupille D
        
        // Ailes (Triangles qui battent)
        ofSetColor(255, 255, 255, 180);
        
        ofPushMatrix();
        ofRotateYDeg(a.wingAngle);
        ofDrawTriangle(1, 0, 0, 12, 0, 6, 2, 0, -6); // Ailes réduites
        ofPopMatrix();
        
        ofPushMatrix();
        ofRotateYDeg(-a.wingAngle);
        ofDrawTriangle(-1, 0, 0, -12, 0, 6, -2, 0, -6); // Ailes réduites
        ofPopMatrix();
        
        ofPopMatrix();
    }
    ofPopStyle();
}