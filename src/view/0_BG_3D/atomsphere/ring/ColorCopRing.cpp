#include "ColorCopRing.h"

//--------------------------------------------------------------
void ColorCopRing::setup(float r, ofVec3f centerPos, int numCops) {
    radius = r;
    center = centerPos;
    
    // Résolution suffisante pour voir la texture et creuser des trous nets
    fboTexture.allocate(2048, 1024, GL_RGBA);
    fboDisplay.allocate(2048, 1024, GL_RGBA);
    
    ofDisableArbTex();
    baseImage.load("Z_extra/1Forets.jpg"); // Image par défaut
    
    resetTexture();
    
    resX = 180;
    resY = 90;

    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // Génération de la sphère
    for (int iy = 0; iy <= resY; iy++) {
        float v = (float)iy / resY;
        float theta = v * PI;
        for (int ix = 0; ix <= resX; ix++) {
            float u = (float)ix / resX;
            float phi = u * TWO_PI;
            
            float xPos = radius * sin(theta) * cos(phi);
            float yPos = radius * cos(theta);
            float zPos = radius * sin(theta) * sin(phi);
            
            mesh.addVertex(ofVec3f(xPos, yPos, zPos));
            mesh.addTexCoord(ofVec2f(u, v));
            mesh.addColor(ofColor(255, 255, 255, 255));
        }
    }

    for (int iy = 0; iy < resY; iy++) {
        for (int ix = 0; ix < resX; ix++) {
            int i0 = iy * (resX + 1) + ix;
            int i1 = i0 + 1;
            int i2 = (iy + 1) * (resX + 1) + ix;
            int i3 = i2 + 1;
            mesh.addIndex(i0); mesh.addIndex(i2); mesh.addIndex(i1);
            mesh.addIndex(i1); mesh.addIndex(i2); mesh.addIndex(i3);
        }
    }
    
    // Initialisation des Color Cops
    cops.resize(numCops);
    for(int i=0; i<numCops; i++) {
        cops[i].pos.set(1024 + 200 * cos(i * TWO_PI / numCops), 512 + 200 * sin(i * TWO_PI / numCops));
        cops[i].rotation = 0;
        cops[i].state = 0;
        cops[i].scale = ofRandom(0.8f, 1.6f);
        cops[i].hasTarget = false;
    }
}

//--------------------------------------------------------------
void ColorCopRing::loadTexture(string path) {
    if(baseImage.load(path)) {
        resetTexture();
    }
}

//--------------------------------------------------------------
void ColorCopRing::resetTexture() {
    fboTexture.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(255, 255, 255, 255);
    if(baseImage.isAllocated()) {
        baseImage.draw(0, 0, fboTexture.getWidth(), fboTexture.getHeight());
    } else {
        ofClear(100, 150, 200, 255);
    }
    fboTexture.end();
    pointsToDestroy.clear();
    
    for(auto& cop : cops) {
        cop.state = 0;
        cop.hasTarget = false;
    }
}

//--------------------------------------------------------------
void ColorCopRing::mousePressed(float u, float v, float squareSize3D) {
    float cx = u * fboTexture.getWidth();
    float cy = v * fboTexture.getHeight();
    
    // Calcul de la taille du carré en pixels sur la texture équirectangulaire
    // Circonférence équatoriale = TWO_PI * radius
    // Hauteur (pôle à pôle) = PI * radius
    float pixelW = (squareSize3D / (TWO_PI * radius)) * fboTexture.getWidth();
    float pixelH = (squareSize3D / (PI * radius)) * fboTexture.getHeight();
    
    // Correction de la distorsion près des pôles pour que le carré physique reste constant
    float latScale = sin(v * PI);
    pixelW /= max(latScale, 0.05f); // Évite la division par zéro aux pôles
    
    // Génère des points à détruire circonscrits dans le carré du CursorSquareSystem
    int numPoints = 120;
    for(int i=0; i<numPoints; i++) {
        float px = cx + ofRandom(-pixelW / 2.0f, pixelW / 2.0f);
        float py = cy + ofRandom(-pixelH / 2.0f, pixelH / 2.0f);
        pointsToDestroy.push_back(ofVec2f(px, py));
    }
}

//--------------------------------------------------------------
void ColorCopRing::update(float dt) {
    for(auto& cop : cops) {
        if(cop.state == 0) { // IDLE
            if(!pointsToDestroy.empty()) {
                cop.targetPoint = pointsToDestroy.back();
                pointsToDestroy.pop_back();
                cop.hasTarget = true;
                
                float angle = ofRandom(TWO_PI);
                float dist = ofRandom(100, 300);
                cop.firingPos = cop.targetPoint + ofVec2f(cos(angle)*dist, sin(angle)*dist);
                cop.state = 1;
                cop.stateTimer = 0.0f;
            } else {
                // Balade aléatoire sur la sphère
                cop.pos.x += ofSignedNoise(cop.pos.x * 0.01f, ofGetElapsedTimef() + cop.scale) * 2.0f;
                cop.pos.y += ofSignedNoise(ofGetElapsedTimef() + cop.scale, cop.pos.y * 0.01f) * 2.0f;
                if(cop.pos.x < 0) cop.pos.x += fboTexture.getWidth();
                if(cop.pos.x > fboTexture.getWidth()) cop.pos.x -= fboTexture.getWidth();
                cop.pos.y = ofClamp(cop.pos.y, 0, fboTexture.getHeight());
                cop.rotation = ofLerpDegrees(cop.rotation, cop.rotation + ofRandom(-10, 10), 0.1f);
            }
        } else if(cop.state == 1) { // MOVING
            cop.stateTimer += dt;
            ofVec2f dirToFiring = cop.firingPos - cop.pos;
            
            // Wrap X handling (boucle horizontale)
            if (abs(dirToFiring.x) > fboTexture.getWidth()/2) {
                if (dirToFiring.x > 0) dirToFiring.x -= fboTexture.getWidth();
                else dirToFiring.x += fboTexture.getWidth();
            }

            // Rapprochement "Cubic Ease" AS3-like
            cop.pos += dirToFiring * 5.0f * dt;
            
            if(cop.pos.x < 0) cop.pos.x += fboTexture.getWidth();
            if(cop.pos.x > fboTexture.getWidth()) cop.pos.x -= fboTexture.getWidth();
            cop.pos.y = ofClamp(cop.pos.y, 0, fboTexture.getHeight());
            
            // Aiming at target
            ofVec2f aimDir = cop.targetPoint - cop.pos;
            if (abs(aimDir.x) > fboTexture.getWidth()/2) {
                if (aimDir.x > 0) aimDir.x -= fboTexture.getWidth();
                else aimDir.x += fboTexture.getWidth();
            }
            
            // Rotation type AS3 (bout vers le haut = 90 deg)
            cop.rotation = atan2(aimDir.y, aimDir.x) * RAD_TO_DEG + 90.0f;
            
            // On est en position pour tirer
            if(dirToFiring.length() < 10.0f || cop.stateTimer > 1.5f) {
                cop.state = 2; // SHOOT
                cop.stateTimer = 0.5f; 
            }
        } else if(cop.state == 2) { // SHOOTING
            cop.stateTimer -= dt;
            if(cop.stateTimer <= 0.0f) {
                // Trou dans la texture (Détruit les pixels, passe en transparent)
                fboTexture.begin();
                ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
                ofSetColor(255, 255, 255, 255); // Soustrait l'alpha complet
                ofDrawCircle(cop.targetPoint, 25.0f); // Taille du trou
                ofDisableBlendMode();
                fboTexture.end();
                
                cop.state = 0;
                cop.hasTarget = false;
            }
        }
    }
    
    // Compile les calques FBO
    fboDisplay.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(255, 255, 255, 255);
    fboTexture.draw(0,0);
    
    for(auto& cop : cops) {
        ofPushMatrix();
        ofTranslate(cop.pos);
        ofRotateDeg(cop.rotation);
        ofScale(cop.scale, cop.scale);
        
        // Dessin du Laser (Tir)
        if(cop.state == 2) {
            ofVec2f aimDir = cop.targetPoint - cop.pos;
            if (abs(aimDir.x) > fboTexture.getWidth()/2) {
                if (aimDir.x > 0) aimDir.x -= fboTexture.getWidth();
                else aimDir.x += fboTexture.getWidth();
            }
            float dist = aimDir.length();
            float progress = 1.0f - (cop.stateTimer / 0.5f);
            float laserLen = dist * progress;
            
            ofSetColor(255, 255, 200, 255);
            ofSetLineWidth(3);
            ofDrawLine(0, 0, 0, -laserLen / cop.scale); // Pointe vers l'avant de la tourelle
            ofSetLineWidth(1);
        }
        
        // Dessin AS3 du Cop
        ofSetColor(240, 240, 240, 255);
        ofFill();
        ofBeginShape();
        ofVertex(0, 0);       // Tête
        ofVertex(-8, 24);     // Arrière Gauche
        ofVertex(8, 24);      // Arrière Droite
        ofEndShape();
        
        ofPopMatrix();
    }
    fboDisplay.end();
}

//--------------------------------------------------------------
void ColorCopRing::draw() {
    ofPushStyle();
    ofEnableAlphaBlending();
    glDepthMask(GL_FALSE); 
    
    ofPushMatrix();
    ofTranslate(center);
    ofRotateYDeg(-90.0f);
    
    fboDisplay.getTexture().bind();
    mesh.draw();
    fboDisplay.getTexture().unbind();
    
    ofPopMatrix();
    
    glDepthMask(GL_TRUE);
    ofPopStyle();
}