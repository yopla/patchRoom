#include "OtarieCreature.h"

OtarieCreature::OtarieCreature(float x, float y) {
    basePos.set(x, y);
    numSegments = 12;
    gravity = 0.5;
    bounce = -0.9;
    
    // Init Ball
    ball.radius = 20;
    ball.pos.set(x, y - 400); // Start ball above
    ball.vel.set(ofRandom(-5, 5), 0);
    
    // Init Segments
    for(int i=0; i<numSegments; i++) {
        Segment s;
        s.width = 30; 
        s.height = 20;
        s.angle = -HALF_PI; // Pointing up
        s.pos.set(x, y - i * s.width);
        segments.push_back(s);
    }
}

void OtarieCreature::setCollider(shared_ptr<ColliderLayer> c) {
    collider = c;
}

void OtarieCreature::update(float mx, float my) {
    moveBall();
    
    // IK Reach
    ofVec2f target(ball.pos.x, ball.pos.y);
    
    // Forward pass (Head to Tail)
    // segments[0] is the head
    target = reach(segments[0], target.x, target.y);
    for(int i=1; i<numSegments; i++) {
        target = reach(segments[i], target.x, target.y);
    }
    
    // Backward pass (Tail to Head) - Positioning
    // Anchor the last segment to basePos
    segments[numSegments-1].pos = basePos;
    
    for(int i=numSegments-1; i>0; i--) {
        position(segments[i], segments[i-1]);
    }
    
    checkHit();
}

ofVec2f OtarieCreature::reach(Segment& seg, float x, float y) {
    float dx = x - seg.pos.x;
    float dy = y - seg.pos.y;
    seg.angle = atan2(dy, dx);
    
    float tx = x - cos(seg.angle) * seg.width;
    float ty = y - sin(seg.angle) * seg.width;
    
    return ofVec2f(tx, ty);
}

void OtarieCreature::position(Segment& segA, Segment& segB) {
    // segA is the parent (closer to base), segB is the child (closer to head)
    segB.pos = segA.getPin();
}

void OtarieCreature::moveBall() {
    ball.vel.y += gravity;
    
    // 1. Collisions avec ColliderLayer (Murs du niveau)
    if (collider) {
        float s = collider->scale;
        float r = ball.radius;
        
        // Prédiction X
        float nextX = ball.pos.x + ball.vel.x;
        float checkX = (ball.vel.x > 0) ? (nextX + r) : (nextX - r);
        if (collider->isWall(checkX / s, ball.pos.y / s)) {
            ball.vel.x *= bounce;
        }
        
        // Prédiction Y
        float nextY = ball.pos.y + ball.vel.y;
        float checkY = (ball.vel.y > 0) ? (nextY + r) : (nextY - r);
        if (collider->isWall(ball.pos.x / s, checkY / s)) {
            ball.vel.y *= bounce;
            ball.vel.x *= 0.95; // Frottement au sol
        }
    }

    ball.pos += ball.vel;
    
    // Boundaries (Local relative to base)
    float floorY = basePos.y;
    float ceilingY = basePos.y - 1000;
    float leftX = basePos.x - 600;
    float rightX = basePos.x + 600;
    
    if(ball.pos.x + ball.radius > rightX) {
        ball.pos.x = rightX - ball.radius;
        ball.vel.x *= bounce;
    } else if(ball.pos.x - ball.radius < leftX) {
        ball.pos.x = leftX + ball.radius;
        ball.vel.x *= bounce;
    }
    
    if(ball.pos.y + ball.radius > floorY) {
        ball.pos.y = floorY - ball.radius;
        ball.vel.y *= bounce;
    } else if(ball.pos.y - ball.radius < ceilingY) {
        ball.pos.y = ceilingY + ball.radius;
        ball.vel.y *= bounce;
    }
}

void OtarieCreature::checkHit() {
    Segment& head = segments[0];
    ofVec2f pin = head.getPin();
    float dist = pin.distance(ball.pos);
    
    if(dist < ball.radius + 15) { 
        ball.vel.x += ofRandom(-2, 2);
        ball.vel.y -= 2; // Kick up
    }
}

void OtarieCreature::draw() {
    ofPushStyle();
    
    // Draw Segments
    ofSetColor(255);
    for(auto& s : segments) {
        ofPushMatrix();
        ofTranslate(s.pos);
        ofRotateRad(s.angle);
        ofDrawRectRounded(0, -s.height/2, s.width, s.height, 5);
        ofSetColor(200, 0, 0);
        ofDrawCircle(0, 0, 2);
        ofDrawCircle(s.width, 0, 2);
        ofSetColor(255);
        ofPopMatrix();
    }
    
    // Draw Ball
    ofSetColor(255, 50, 50);
    ofDrawCircle(ball.pos, ball.radius);
    
    ofPopStyle();
}