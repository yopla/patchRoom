#include "ChainCrea.h"

//--------------------------------------------------------------
// IKChain Implementation
//--------------------------------------------------------------
void IKChain::setup(float startX, float startY, int numSegs, float len, float w) {
    segments.clear();
    for(int i=0; i<numSegs; i++) {
        auto seg = make_shared<IKSegment>(len, w);
        seg->x = startX;
        seg->y = startY;
        segments.push_back(seg);
    }
}

ofVec2f IKChain::reach(shared_ptr<IKSegment> seg, float xPos, float yPos) {
    float dx = xPos - seg->x;
    float dy = yPos - seg->y;
    float angle = atan2(dy, dx);
    seg->angle = angle;
    
    float w = cos(angle) * seg->len;
    float h = sin(angle) * seg->len;
    
    return ofVec2f(xPos - w, yPos - h);
}

void IKChain::position(shared_ptr<IKSegment> segA, shared_ptr<IKSegment> segB) {
    // segA se place à la fin (pin) de segB
    segA->x = segB->getPin().x;
    segA->y = segB->getPin().y;
}

void IKChain::update(float targetX, float targetY) {
    if(segments.empty()) return;
    
    // 1. Drag from head (index 0) towards target
    ofVec2f p(targetX, targetY);
    
    p = reach(segments[0], p.x, p.y);
    
    for(size_t i=1; i<segments.size(); i++) {
        p = reach(segments[i], p.x, p.y);
    }
    
    // 2. Fix from tail (anchor) back to head
    // On propage la position depuis l'ancre (dernier segment) vers la tête
    for(int i = segments.size() - 1; i > 0; i--) {
        position(segments[i-1], segments[i]);
    }
}

void IKChain::draw() {
    ofPushStyle();
    ofSetColor(255);
    
    for(auto& seg : segments) {
        ofPushMatrix();
        ofTranslate(seg->x, seg->y);
        ofRotateRad(seg->angle);
        
        float h = seg->width;
        float w = seg->len;
        
        ofFill();
        // Dessin style capsule
        ofDrawCircle(0, 0, h*0.5f);
        ofDrawCircle(w, 0, h*0.5f);
        ofDrawRectangle(0, -h*0.5f, w, h);
        
        ofPopMatrix();
    }
    ofPopStyle();
}

//--------------------------------------------------------------
// ChainCrea Layer Implementation
//--------------------------------------------------------------
void ChainCrea::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    chains.clear();
    
    int numRopes = 6; // Grille 6x6
    float spacing = 250.0f;
    float startX = w * 0.5f - ((numRopes-1) * spacing) * 0.5f;
    float startY = h * 0.5f - ((numRopes-1) * spacing) * 0.5f;
    
    for(int i=0; i<numRopes; i++) {
        float x = startX + i * spacing;
        for(int j=0; j<numRopes; j++) {
            float y = startY + j * spacing;
            
            IKChain chain;
            // 15 segments, longueur 25, épaisseur 4
            chain.setup(x, y, 15, 25, 4);
            chains.push_back(chain);
        }
    }
}

void ChainCrea::update(float mouseX, float mouseY, float time) {
    for(auto& chain : chains) {
        chain.update(mouseX, mouseY);
    }
}

void ChainCrea::draw() {
    for(auto& chain : chains) {
        chain.draw();
    }
}