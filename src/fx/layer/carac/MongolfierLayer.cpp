#include "MongolfierLayer.h"

void MongolfierLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    center.set(w/2, h/2);
    
    parts.clear();
    for (int i = 0; i < 4; i++) {
        MPart p;
        p.pos.x = center.x + cos(i * 0.3) * 32;
        p.pos.y = center.y + sin(i * 0.3) * 32;
        p.vel.set(0,0);
        parts.push_back(p);
    }
}

bool MongolfierLayer::isWall(float ax, float ay) {
    if (ax < 0) return true;
    if (ay < 0) return true;
    if (ax > simWidth) return true;
    if (ay > simHeight) return true;
    return false;
}

float MongolfierLayer::getMag(float ax, float ay) {
    return sqrt(ax*ax + ay*ay);
}

void MongolfierLayer::update(float mouseX, float mouseY) {
    center.set(mouseX, mouseY);
    
    int num = parts.size();
    float t = 0.2;
    
    // 1. Interaction entre particules (Springs)
    for (int i = 0; i < num; i++) {
        MPart& a = parts[i];
        MPart& b = (i + 1 == num) ? parts[0] : parts[i+1];
        
        float ang = atan2(b.pos.y - a.pos.y, b.pos.x - a.pos.x);
        
        a.vel.x += cos(ang) * t;
        a.vel.y += sin(ang) * t;
    }
    
    // 2. Physique globale
    t = 0.5;
    for (int i = 0; i < num; i++) {
        MPart& a = parts[i];
        
        a.vel.y += 0.1; // Gravité
        
        float ang = atan2(center.y - a.pos.y, center.x - a.pos.x);
        float d = getMag(a.pos.x - center.x, a.pos.y - center.y);
        
        if (d > 80) {
            a.vel.x += cos(ang) * t;
            a.vel.y += sin(ang) * t;
        } else if (d < 70) {
            a.vel.x += cos(ang) * -t;
            a.vel.y += sin(ang) * -t;
        }
        
        a.vel *= 0.95; // Friction
        
        // Collisions
        if (a.vel.x > 0 && isWall(a.pos.x + a.vel.x, a.pos.y)) a.vel.x *= -0.5;
        if (a.vel.x < 0 && isWall(a.pos.x + a.vel.x, a.pos.y)) a.vel.x *= -0.5;
        if (a.vel.y > 0 && isWall(a.pos.x, a.pos.y + a.vel.y)) a.vel.y *= -0.5;
        if (a.vel.y < 0 && isWall(a.pos.x, a.pos.y + a.vel.y)) a.vel.y *= -0.5;
        
        a.pos += a.vel;
    }
}

void MongolfierLayer::draw() {
    ofPushStyle();
    ofSetLineWidth(2);
    ofSetColor(0); 
    
    ofNoFill();
    ofDrawCircle(center, 32);
    
    int num = parts.size();
    
    // Lignes et cercles
    for (int i = 0; i < num; i++) {
        MPart& a = parts[i];
        MPart& b = (i + 1 == num) ? parts[0] : parts[i+1];
        ofDrawLine(a.pos, b.pos);
        ofDrawLine(a.pos, center);
        ofDrawCircle(a.pos, 4);
    }
    
    // Forme remplie
    ofSetColor(0xF3, 0x35, 0x24, 128); 
    ofFill();
    ofBeginShape();
    for (int i = 0; i < num; i++) {
        ofVertex(parts[i].pos);
    }
    ofEndShape(true);
    
    ofPopStyle();
}