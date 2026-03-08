#include "PinceFoireLayer.h"

//--------------------------------------------------------------
void PinceFoireLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    clawPos.set(w/2, 200);
    clawVel.set(0,0);
    clawJawAngle = 0;
    isMouseDown = false;
    
    // Create prizes (balls and boxes simulated as circles for simplicity)
    prizes.clear();
    for(int i=0; i<20; i++) {
        Prize p;
        // Random position at the bottom
        p.pos.set(ofRandom(w * 0.3, w * 0.7), h - ofRandom(50, 250));
        p.vel.set(0,0);
        p.radius = ofRandom(25, 45);
        
        // Random colors inspired by arcade prizes
        if (i % 2 == 0) p.color = ofColor::fromHsb(ofRandom(0, 50), 200, 255); // Reds/Oranges
        else p.color = ofColor::fromHsb(ofRandom(100, 180), 150, 255); // Greens/Blues
        
        p.isGrabbed = false;
        p.id = i;
        prizes.push_back(p);
    }
}

//--------------------------------------------------------------
void PinceFoireLayer::update(float mouseX, float mouseY, float time) {
    // 1. Claw Movement Logic (Inspired by Ref AS3)
    // Mouse X controls horizontal velocity
    float targetVX = 0;
    float deadZone = 20.0f;
    
    if(mouseX > clawPos.x + deadZone) targetVX = 8.0f;
    else if(mouseX < clawPos.x - deadZone) targetVX = -8.0f;
    
    // Smooth horizontal movement
    clawVel.x = ofLerp(clawVel.x, targetVX, 0.1f);
    
    // Mouse Click controls Vertical state
    if(isMouseDown) {
        clawVel.y = 6.0f; // Go Down
        // Close jaws progressively
        clawJawAngle = ofLerp(clawJawAngle, 45.0f, 0.08f);
    } else {
        clawVel.y = -4.0f; // Go Up
        // Open jaws progressively
        clawJawAngle = ofLerp(clawJawAngle, 0.0f, 0.08f);
    }
    
    clawPos += clawVel;
    
    // Constraints (Claw bounds)
    if(clawPos.y < 100) { clawPos.y = 100; clawVel.y = 0; }
    if(clawPos.y > simHeight - 120) { clawPos.y = simHeight - 120; } // Don't hit floor directly
    if(clawPos.x < 100) clawPos.x = 100;
    if(clawPos.x > simWidth - 100) clawPos.x = simWidth - 100;

    // 2. Prizes Physics
    for(auto& p : prizes) {
        updatePrize(p);
    }
    resolveCollisions();
}

//--------------------------------------------------------------
void PinceFoireLayer::updatePrize(Prize& p) {
    if(p.isGrabbed) {
        // Follow claw (offset to be inside jaws)
        ofVec2f grabPos = clawPos + ofVec2f(0, 50);
        p.pos = grabPos;
        p.vel.set(0,0);
        
        // Drop if jaws open too much
        if(clawJawAngle < 25.0f) {
            p.isGrabbed = false;
            p.vel = clawVel * 0.5f; // Inherit some velocity
        }
    } else {
        // Gravity
        p.vel.y += 0.6f;
        p.pos += p.vel;
        
        // Floor collision
        if(p.pos.y > simHeight - p.radius) {
            p.pos.y = simHeight - p.radius;
            p.vel.y *= -0.5f; // Dampened bounce
            p.vel.x *= 0.8f;  // Friction
        }
        
        // Walls
        if(p.pos.x < p.radius) { p.pos.x = p.radius; p.vel.x *= -0.5f; }
        if(p.pos.x > simWidth - p.radius) { p.pos.x = simWidth - p.radius; p.vel.x *= -0.5f; }
        
        // Check if we can be grabbed
        checkGrab(p);
    }
}

//--------------------------------------------------------------
void PinceFoireLayer::checkGrab(Prize& p) {
    if(isMouseDown && clawJawAngle > 35.0f) {
        ofVec2f grabCenter = clawPos + ofVec2f(0, 50);
        float dist = p.pos.distance(grabCenter);
        // If close enough to the "sweet spot"
        if(dist < 60.0f) {
            p.isGrabbed = true;
        }
    }
}

//--------------------------------------------------------------
void PinceFoireLayer::resolveCollisions() {
    // Simple circle-circle collision
    for(int i=0; i<prizes.size(); i++) {
        for(int j=i+1; j<prizes.size(); j++) {
            Prize& p1 = prizes[i];
            Prize& p2 = prizes[j];
            
            if(p1.isGrabbed || p2.isGrabbed) continue; // Ignore grabbed objects collision for simplicity
            
            float dist = p1.pos.distance(p2.pos);
            float minDist = p1.radius + p2.radius;
            
            if(dist < minDist && dist > 0) {
                ofVec2f dir = (p1.pos - p2.pos).getNormalized();
                float overlap = minDist - dist;
                
                // Push apart
                p1.pos += dir * overlap * 0.5f;
                p2.pos -= dir * overlap * 0.5f;
                
                // Exchange momentum (very simplified)
                ofVec2f v1 = p1.vel;
                p1.vel = p2.vel * 0.8f;
                p2.vel = v1 * 0.8f;
            }
        }
    }
}

//--------------------------------------------------------------
void PinceFoireLayer::draw() {
    ofPushStyle();
    
    // 1. Draw Prizes
    for(auto& p : prizes) {
        ofSetColor(p.color);
        ofDrawCircle(p.pos, p.radius);
        
        // Shine effect
        ofSetColor(255, 100);
        ofDrawCircle(p.pos.x - p.radius*0.3, p.pos.y - p.radius*0.3, p.radius*0.2);
        
        // Outline
        ofSetColor(0, 50);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawCircle(p.pos, p.radius);
        ofFill();
    }
    
    // 2. Draw Claw Machine
    ofSetColor(50);
    ofSetLineWidth(4);
    
    // String
    ofDrawLine(clawPos.x, 0, clawPos.x, clawPos.y);
    
    // Claw Body
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(80);
    ofDrawRectangle(clawPos.x, clawPos.y, 60, 30);
    
    // Jaws
    ofPushMatrix();
    ofTranslate(clawPos.x, clawPos.y);
    
    ofSetColor(180, 180, 200); // Metallic
    
    // Left Jaw
    ofPushMatrix();
    ofTranslate(-20, 10);
    ofRotateDeg(-clawJawAngle); // Opens outwards
    ofDrawRectangle(0, 30, 12, 60); // Arm
    ofTranslate(0, 60);
    ofRotateDeg(40); // Hook inward
    ofDrawRectangle(0, 10, 12, 25); // Tip
    ofPopMatrix();
    
    // Right Jaw
    ofPushMatrix();
    ofTranslate(20, 10);
    ofRotateDeg(clawJawAngle);
    ofDrawRectangle(0, 30, 12, 60);
    ofTranslate(0, 60);
    ofRotateDeg(-40);
    ofDrawRectangle(0, 10, 12, 25);
    ofPopMatrix();
    
    ofPopMatrix();
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofPopStyle();
}

//--------------------------------------------------------------
void PinceFoireLayer::mousePressed(float x, float y, int button) {
    isMouseDown = true;
}

//--------------------------------------------------------------
void PinceFoireLayer::mouseReleased(float x, float y, int button) {
    isMouseDown = false;
}