#include "PlanColleSystem.h"

void PlanColleSystem::setup() {
    float w = 500;
    float h = 550;
    
    planContentFbo.allocate(w, h, GL_RGBA);
    
    int numParticles = 40;
    particles.clear();
    for(int i=0; i<numParticles; i++){
        Particle p;
        p.pos = glm::vec3(ofRandom(w), ofRandom(h), 0);
        p.vel = glm::vec3(ofRandom(-2,2), ofRandom(-2,2), 0);
        p.size = ofRandom(3, 8);
        
        if(ofRandom(1) > 0.5) p.color = ofColor(0, 255, 255);
        else p.color = ofColor(255, 255, 255);
        
        particles.push_back(p);
    }

    updatePosition();
}

void PlanColleSystem::update() {
    float speedAngle = 1.0f;
    if(ofGetKeyPressed('d') || ofGetKeyPressed('D')) planAngle -= speedAngle;
    if(ofGetKeyPressed('q') || ofGetKeyPressed('Q')) planAngle += speedAngle;
    if(ofGetKeyPressed('z') || ofGetKeyPressed('Z')) planElevation += speedAngle; 
    if(ofGetKeyPressed('s') || ofGetKeyPressed('S')) planElevation -= speedAngle;

    updatePosition();
    updateParticles();

    if (isMouseOverPlan) {
        ofVec3f worldPos = planColleNode.getGlobalPosition();
            
        float minX = -1200.0f; float maxX =  1200.0f; 
        float minZ = -1312.0f; float maxZ =  1312.0f; 
        float peakY = 1452.0f; float solY  =    0.0f; 
        
        radius = ofMap(worldPos.x, minX, maxX, 0.0f, 1.0f, true);
        elevation = ofMap(worldPos.z, minZ, maxZ, 0.0f, 1.0f, true);
        azimuth = ofMap(worldPos.y, peakY, solY, 0.0f, 1.0f, true);
    }
}

void PlanColleSystem::updateParticles() {
    float w = planContentFbo.getWidth();
    float h = planContentFbo.getHeight();
    glm::vec3 center(w/2, h/2, 0);

    for(int i=0; i<particles.size(); i++) {
        Particle &p = particles[i];

        if(isMouseOverPlan) {
            p.vel.x += ofRandom(-0.8, 0.8);
            p.vel.y += ofRandom(-0.8, 0.8);
            p.size = ofRandom(5, 12); 
        } else {
            p.size = ofMap(i, 0, particles.size(), 3, 8); 
        }

        glm::vec3 dirToCenter = center - p.pos;
        float distCenter = glm::length(dirToCenter);
        if(distCenter > 0) {
            dirToCenter = glm::normalize(dirToCenter);
            p.vel += dirToCenter * 0.03f; 
        }

        for(int j=0; j<particles.size(); j++) {
            if(i == j) continue;
            glm::vec3 diff = p.pos - particles[j].pos;
            float d = glm::length(diff);
            if(d > 0 && d < 60) {
                diff = glm::normalize(diff);
                float repulseForce = 1.0f / d; 
                p.vel += diff * repulseForce * 2.0f; 
            }
        }

        float friction = isMouseOverPlan ? 0.99f : 0.97f;
        p.vel *= friction;
        
        p.pos += p.vel;
        
        if(p.pos.x < 0) { p.pos.x = 0; p.vel.x *= -1; }
        if(p.pos.x > w) { p.pos.x = w; p.vel.x *= -1; }
        if(p.pos.y < 0) { p.pos.y = 0; p.vel.y *= -1; }
        if(p.pos.y > h) { p.pos.y = h; p.vel.y *= -1; }
    }

    planContentFbo.begin();
        int alphaVal = bSolidBackground ? 128 : 0; 
        ofClear(255, 255, 255, alphaVal);
        
        bool carreOuPas = false;
        if (carreOuPas) ofSetColor(0, 0, 0, alphaVal);
        if (carreOuPas) ofDrawRectangle(0, 0, w, h);

        ofEnableAlphaBlending();
        
        ofSetLineWidth(1);
        for(int i=0; i<particles.size(); i++) {
            for(int j=i+1; j<particles.size(); j++) {
                float d = glm::distance(particles[i].pos, particles[j].pos);
                if(d < 80) { 
                    float lineAlpha = ofMap(d, 0, 80, 255, 0);
                    ofSetColor(255, 255, 255, lineAlpha);
                    ofDrawLine(particles[i].pos, particles[j].pos);
                }
            }
        }

        for(auto& p : particles) {
            ofSetColor(p.color);
            ofDrawCircle(p.pos, p.size);
        }
        
        ofNoFill();
        ofSetColor(255, 50);
        ofDrawRectangle(0, 0, w, h);
        ofFill();
        
    planContentFbo.end();
}

void PlanColleSystem::updatePosition() {
    float rad = 1500.0f; 
    float theta = ofDegToRad(planAngle);     
    float phi   = ofDegToRad(planElevation); 
    
    float x = rad * cos(phi) * sin(theta);
    float y = rad * sin(phi);
    float z = rad * cos(phi) * cos(theta);

    planColleNode.setPosition(x, y, z);
    planColleNode.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
}

void PlanColleSystem::draw() {
    if(!planContentFbo.isAllocated()) return;
    
    ofEnableAlphaBlending();
    if (!bSolidBackground) glDepthMask(GL_FALSE);

    planColleNode.transformGL();
        
        ofRotateYDeg(180); 
        ofSetColor(255);
        planContentFbo.draw(-250, -275, 500, 550);
        
        if(isMouseOverPlan) {
            ofNoFill();
            ofSetLineWidth(5);
            ofSetColor(255, 0, 0);
            ofDrawRectangle(-250, -275, 0.1, 500, 550);
            
            ofSetColor(255, 255, 0); 
            ofFill();
        }
        
    planColleNode.restoreTransformGL();
    if (!bSolidBackground)    glDepthMask(GL_TRUE);
    ofDisableAlphaBlending();
}

void PlanColleSystem::checkMouseIntersection(const ofCamera& cam) {
    float w = 500.0f;
    float h = 550.0f;

    glm::vec3 rayOrigin = cam.getPosition();
    glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(ofGetMouseX(), ofGetMouseY(), 0));
    glm::vec3 rayDir = glm::normalize(mouseWorld - rayOrigin);

    ofMatrix4x4 planeTransform = planColleNode.getGlobalTransformMatrix();
    ofMatrix4x4 inverseTransform = planeTransform.getInverse();

    ofVec3f localRayOrigin = ofVec3f(rayOrigin) * inverseTransform;
    
    ofVec3f worldPointOnRay = rayOrigin + rayDir;
    ofVec3f localPointOnRay = worldPointOnRay * inverseTransform;
    ofVec3f localRayDir = (localPointOnRay - localRayOrigin).getNormalized();

    if (abs(localRayDir.z) < 1e-6) {
        isMouseOverPlan = false;
        return;
    }

    float t = -localRayOrigin.z / localRayDir.z;

    if (t < 0) {
        isMouseOverPlan = false;
        return;
    }

    ofVec3f localHit = localRayOrigin + localRayDir * t;

    float halfW = w / 2.0f;
    float halfH = h / 2.0f;

    if (localHit.x >= -halfW && localHit.x <= halfW &&
        localHit.y >= -halfH && localHit.y <= halfH) {
        isMouseOverPlan = true;
    } else {
        isMouseOverPlan = false;
    }
}

void PlanColleSystem::keyPressed(int key) {
    if(key == 'c' || key == 'C') {
        bSolidBackground = !bSolidBackground;
    }
}