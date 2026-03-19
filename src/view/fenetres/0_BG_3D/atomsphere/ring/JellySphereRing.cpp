#include "JellySphereRing.h"

//--------------------------------------------------------------
void JellySphereRing::setup(float r, ofVec3f centerPos) {
    radius = r;
    center = centerPos;
    
    fbo.allocate(2048, 1024, GL_RGBA);
    fbo.begin(); ofClear(0,0,0,0); fbo.end();
    
    ofDisableArbTex();
    baseImage.load("1Forets.jpg"); // Image par défaut
    
    fbo.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    fbo.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
    
    resX = 180;
    resY = 90;
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

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
    isDrawing = false;
}

//--------------------------------------------------------------
void JellySphereRing::loadTexture(string path) {
    baseImage.load(path);
}

void JellySphereRing::clearJellies() {
    jellies.clear();
    isDrawing = false;
}

//--------------------------------------------------------------
void JellySphereRing::update(float localX, float localY) {
    lastLocalX = localX;
    lastLocalY = localY;
    
    if (isDrawing && localX >= 0 && localY >= 0) {
        currentX = localX;
        currentY = localY;
    }
    
    for (auto& j : jellies) {
        j->update(localX, localY);
    }
    
    fbo.begin();
    ofPushStyle();
    ofDisableAlphaBlending();
    ofSetColor(255);
    if (baseImage.isAllocated()) {
        baseImage.draw(0, 0, fbo.getWidth(), fbo.getHeight());
    } else {
        ofClear(0, 0, 0, 255);
    }
    ofEnableAlphaBlending(); // Re-active l'alpha pour dessiner les jellies
    
    for (auto& j : jellies) {
        j->draw(localX, localY);
    }
    
    if (isDrawing) {
        float r = ofDist(startX, startY, currentX, currentY);
        ofNoFill();
        ofSetColor(255, 255, 0);
        ofSetLineWidth(2);
        ofDrawCircle(startX, startY, r);
    }
    ofPopStyle();
    fbo.end();
}

//--------------------------------------------------------------
void JellySphereRing::draw() {
    ofPushStyle();
    ofEnableAlphaBlending();
    glDepthMask(GL_FALSE); 
    
    ofPushMatrix();
    ofTranslate(center);
    ofRotateYDeg(-90.0f);
    fbo.getTexture().bind();
    mesh.draw();
    fbo.getTexture().unbind();
    ofPopMatrix();
    
    glDepthMask(GL_TRUE);
    ofPopStyle();
}

//--------------------------------------------------------------
void JellySphereRing::mousePressed(float x, float y) {
    if (x < 0 || y < 0) return;
    
    // Si on clique sur un jelly existant, on le détruit
    for (int i = jellies.size() - 1; i >= 0; i--) {
        auto& j = jellies[i];
        // Hitbox Circulaire (beaucoup plus précis pour un Jelly rond)
        float cx = j->x + j->w / 2.0f;
        float cy = j->y + j->h / 2.0f;
        float jRadius = j->w / 2.0f;
        if (ofDist(x, y, cx, cy) <= jRadius) {
            jellies.erase(jellies.begin() + i);
            return; 
        }
    }
    
    // Sinon on démarre le dessin
    isDrawing = true;
    startX = x; startY = y;
    currentX = x; currentY = y;
}

//--------------------------------------------------------------
void JellySphereRing::mouseReleased(float x, float y) {
    if (!isDrawing) return;
    isDrawing = false;
    
    // Sécurité : Si on relâche la souris en dehors de la sphère 3D (x = -1000)
    // on utilise la dernière position valide enregistrée pendant le drag
    float endX = (x >= 0) ? x : currentX;
    float endY = (y >= 0) ? y : currentY;
    
    float r = ofDist(startX, startY, endX, endY);
    if (r > 10 && baseImage.isAllocated()) {
        float jX = startX - r;
        float jY = startY - r;
        float jW = 2 * r;
        float jH = 2 * r;
        
        float scaleX = baseImage.getWidth() / fbo.getWidth();
        float scaleY = baseImage.getHeight() / fbo.getHeight();
        
        int cx = ofClamp(jX * scaleX, 0, baseImage.getWidth() - 1);
        int cy = ofClamp(jY * scaleY, 0, baseImage.getHeight() - 1);
        int cw = ofClamp(jW * scaleX, 1, baseImage.getWidth() - cx);
        int ch = ofClamp(jH * scaleY, 1, baseImage.getHeight() - cy);
        
        ofImage subImg = baseImage;
        subImg.crop(cx, cy, cw, ch);
        
        auto jc = make_shared<JellyCreature>(jX, jY, jW, jH, subImg);
        jc->bDrawBaseImage = false; // Dessine uniquement le dôme physique, pas le carré plat en dessous
        jc->createDome(r, r, r);
        jellies.push_back(jc);
    }
}