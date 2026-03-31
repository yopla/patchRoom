#include "ExternalKrakenSystem.h"

//--------------------------------------------------------------
void ExternalTentacle::setup(int index, float angle, float len, int n) {
    angleOffset = angle;
    length = len;
    numNodes = n;
    nodes.resize(numNodes);
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // Room dimensions (from RoomWalls.h)
    const float roomW = 2400.0f;
    const float roomD = 2624.0f;
    const float hFrontBack = 1472.0f;
    const float hCour = 1072.0f;
    const float hJar = 784.0f;
    const float peakY = 1452.0f; // heightFrontBack - 20
    const float peakX = 252.0f;  // 1452 - 1200

    // Assign each tentacle to a surface (2 tentacles per surface)
    int surface = index % 6;

    switch(surface) {
        case 0: // Front Wall (z = -d/2)
            targetPoint.set(ofRandom(-roomW/2, roomW/2), ofRandom(0, hFrontBack), -roomD/2);
            break;
        case 1: // Back Wall (z = +d/2)
            targetPoint.set(ofRandom(-roomW/2, roomW/2), ofRandom(0, hFrontBack), roomD/2);
            break;
        case 2: // Cour Wall (right, x = +w/2)
            targetPoint.set(roomW/2, ofRandom(0, hCour), ofRandom(-roomD/2, roomD/2));
            break;
        case 3: // Jar Wall (left, x = -w/2)
            targetPoint.set(-roomW/2, ofRandom(0, hJar), ofRandom(-roomD/2, roomD/2));
            break;
        case 4: // Top Cour (right roof panel)
            {
                float rX = ofRandom(peakX, roomW/2);
                float rZ = ofRandom(-roomD/2, roomD/2);
                float rY = ofMap(rX, peakX, roomW/2, peakY, hCour);
                targetPoint.set(rX, rY, rZ);
            }
            break;
        case 5: // Top Jar (left roof panel)
            {
                float rX = ofRandom(-roomW/2, peakX);
                float rZ = ofRandom(-roomD/2, roomD/2);
                float rY = ofMap(rX, -roomW/2, peakX, hJar, peakY);
                targetPoint.set(rX, rY, rZ);
            }
            break;
    }
}

//--------------------------------------------------------------
void ExternalTentacle::update(float time, ofVec3f headPos, float growth) {
    // On utilise le point cible 3D stable défini dans setup()
    ofVec3f targetPos = targetPoint;
    
    // Point de contrôle pour courber le tentacule vers l'extérieur
    // (Pour éviter qu'il ne traverse tout droit la room)
    ofVec3f midPoint = (headPos + targetPos) * 0.5f;
    midPoint.normalize();
    midPoint *= 3000.0f; // On le pousse loin
    
    for(int i=0; i<numNodes; i++) {
        float pct = (float)i / (float)(numNodes-1);
        
        // On limite le pct par la croissance (growth)
        // Si growth = 0.5, le tentacule n'est dessiné qu'à moitié
        float effectivePct = pct * growth;
        
        // Interpolation quadratique de Bézier (Head -> Mid -> Target)
        ofVec3f p1 = headPos.getInterpolated(midPoint, effectivePct);
        ofVec3f p2 = midPoint.getInterpolated(targetPos, effectivePct);
        ofVec3f pos = p1.getInterpolated(p2, effectivePct);
        
        float x = pos.x;
        float y = pos.y;
        float z = pos.z;
        
        // Ondulation
        // float wave = sin(time * 2.0f + i * 0.2f) * 50.0f * growth;
        
        // Ajout de bruit pour un aspect organique
        x += ofSignedNoise(time * 0.5, i * 0.1) * 100.0f;
        z += ofSignedNoise(time * 0.5, i * 0.1 + 100) * 100.0f;

        nodes[i].set(x, y, z);
    }
    
    // Génération du Mesh (Tube géant)
    mesh.clear();
    int sides = 8; 
    float baseRadius = 80.0f; // Tentacules très épais
    
    for(int i=0; i<numNodes; i++) {
        float pct = (float)i / (float)(numNodes-1);
        float r = baseRadius * (1.0f - pct * 0.5f); // S'affine un peu vers le haut
        
        ofVec3f forward;
        if(i < numNodes - 1) forward = (nodes[i+1] - nodes[i]).getNormalized();
        else forward = (nodes[i] - nodes[i-1]).getNormalized();
        
        ofVec3f right = forward.getCrossed(ofVec3f(0,1,0)).getNormalized();
        ofVec3f up = right.getCrossed(forward).getNormalized();
        
        for(int j=0; j<=sides; j++) {
            float ang = (float)j / (float)sides * TWO_PI;
            ofVec3f p = nodes[i] + (right * cos(ang) + up * sin(ang)) * r;
            mesh.addVertex(p);
            // Couleur : Vert sombre / Cyan toxique
            mesh.addColor(ofColor(20, 80 + pct*100, 60 + pct*50)); 
        }
    }
    
    // Indices
    for(int i=0; i<numNodes-1; i++) {
        for(int j=0; j<sides; j++) {
            int curr = i * (sides+1) + j;
            int next = curr + (sides+1);
            mesh.addIndex(curr); mesh.addIndex(next); mesh.addIndex(curr+1);
            mesh.addIndex(curr+1); mesh.addIndex(next); mesh.addIndex(next+1);
        }
    }
}

void ExternalTentacle::draw() {
    mesh.draw();
}

//--------------------------------------------------------------
void ExternalKrakenSystem::setup() {
    head.setRadius(300);
    head.setResolution(16);

    int numTentacles = 12; // 12 tentacules pour bien entourer
    for(int i=0; i<numTentacles; i++) {
        ExternalTentacle t;
        float angle = (TWO_PI / numTentacles) * i;
        t.setup(i, angle, 0, 60); // Longueur gérée dans update
        tentacles.push_back(t);
    }
}

void ExternalKrakenSystem::start(float time) {
    startTime = time;
    
    // 1. Choisir une direction d'apparition aléatoire
    float angle = ofRandom(TWO_PI);
    float distFar = 4000.0f;  // Un peu moins loin
    float distNear = 2200.0f; // Assez proche pour être menaçant
    
    // 2. Position de départ (Loin et en dessous)
    float startY = -4000.0f;
    headStartPos.set(cos(angle) * distFar, startY, sin(angle) * distFar);
    
    // 3. Position cible (Plus proche, centré en Y)
    headTargetPos.set(cos(angle) * distNear, 0, sin(angle) * distNear);
}

void ExternalKrakenSystem::update(float time) {
    // Animation d'émergence
    float animTime = time - startTime;
    float moveDuration = 4.0f;
    float pct = ofClamp(animTime / moveDuration, 0.0f, 1.0f);
    
    // Ease Out Cubic pour le mouvement de la tête
    float ease = 1.0f - pow(1.0f - pct, 3.0f);

    ofVec3f currentHeadPos = headStartPos.getInterpolated(headTargetPos, ease);
    // Petit flottement continu
    currentHeadPos.y += sin(time * 0.5f) * 50.0f;
    head.setPosition(currentHeadPos);

    // Croissance des tentacules (légèrement retardée par rapport à la tête)
    float growth = ofClamp((animTime - 0.5f) / 3.5f, 0.0f, 1.0f);
    growth = pow(growth, 0.5f); // Ease Out pour sortir vite
    
    for(auto& t : tentacles) {
        t.update(time, head.getPosition(), growth);
    }
}

void ExternalKrakenSystem::draw() {
    ofPushStyle();
    ofSetColor(20, 80, 60); // Tête vert sombre
    head.draw();
    for(auto& t : tentacles) {
        t.draw();
    }
    ofPopStyle();
}