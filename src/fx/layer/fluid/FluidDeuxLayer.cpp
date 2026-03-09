#include "FluidDeuxLayer.h"

void FluidDeuxLayer::setup(float w, float h, float s) {
    simWidth = w;
    simHeight = h;
    scale = s;
    
    invGridSize = 1.0f / RANGE;
    numGridsX = ceil(simWidth * invGridSize);
    numGridsY = ceil(simHeight * invGridSize);
    
    grids.resize(numGridsX);
    for(int i=0; i<numGridsX; i++) {
        grids[i].resize(numGridsY);
    }
    
    neighbors.resize(50000); 
}

void FluidDeuxLayer::setCollider(shared_ptr<ColliderLayer> c) {
    collider = c;
}

void FluidDeuxLayer::update(float mouseX, float mouseY, float time) {
    mousePos.set(mouseX / scale, mouseY / scale);
    if(isPressed) pour();
    move();
}

void FluidDeuxLayer::pour() {
    if(particles.size() > 3000) return;

    for(int i = -4; i <= 4; i++) {
        FluidDeuxParticle p;
        p.x = mousePos.x + i * 10;
        p.y = mousePos.y;
        p.vx = 0;
        p.vy = 5;
        p.type = (int)(ofGetFrameNum() / 10) % 5; 
        
        switch(p.type) {
            case 0: p.color.setHex(0x6060ff); break;
            case 1: p.color.setHex(0xff6000); break;
            case 2: p.color.setHex(0xff0060); break;
            case 3: p.color.setHex(0x00d060); break;
            case 4: p.color.setHex(0xd0d000); break;
        }
        
        particles.push_back(p);
    }
}

void FluidDeuxLayer::move() {
    updateGrids();
    findNeighbors();
    calcForce();
    
    for(auto& p : particles) {
        p.vy += GRAVITY;
        if(p.density > 0) {
            p.vx += p.fx / (p.density * 0.9f + 0.1f);
            p.vy += p.fy / (p.density * 0.9f + 0.1f);
        }
        
        p.x += p.vx;
        p.y += p.vy;

        // Collision avec ColliderLayer
        if(collider) {
            float r = 3.0f; // Rayon de détection
            float push = 1.0f; // Force de répulsion
            float bounce = 1.5f; // Facteur de rebond (v -= v*1.5 => v *= -0.5)

            if(collider->isWall(p.x - r, p.y)) {
                p.vx -= p.vx * bounce;
                p.x += push;
            }
            if(collider->isWall(p.x + r, p.y)) {
                p.vx -= p.vx * bounce;
                p.x -= push;
            }
            if(collider->isWall(p.x, p.y - r)) {
                p.vy -= p.vy * bounce;
                p.y += push;
            } else if(collider->isWall(p.x, p.y + r)) {
                p.vy -= p.vy * bounce;
                p.y -= push;
            }
        }
        
        // Boundaries
        if(p.x < 5) { p.vx += (5 - p.x - p.vx) * 0.5f; p.x = 5; }
        if(p.x > simWidth - 5) { p.vx += (simWidth - 5 - p.x - p.vx) * 0.5f; p.x = simWidth - 5; }
        if(p.y < 5) { p.vy += (5 - p.y - p.vy) * 0.5f; p.y = 5; }
        if(p.y > simHeight - 5) { p.vy += (simHeight - 5 - p.y - p.vy) * 0.5f; p.y = simHeight - 5; }
    }
}

void FluidDeuxLayer::updateGrids() {
    for(int i=0; i<numGridsX; i++) {
        for(int j=0; j<numGridsY; j++) {
            grids[i][j].pIndices.clear();
        }
    }
    
    for(int i=0; i<particles.size(); i++) {
        FluidDeuxParticle& p = particles[i];
        p.fx = p.fy = p.density = p.densityNear = 0;
        p.gx = (int)(p.x * invGridSize);
        p.gy = (int)(p.y * invGridSize);
        
        if(p.gx < 0) p.gx = 0;
        if(p.gx >= numGridsX) p.gx = numGridsX - 1;
        if(p.gy < 0) p.gy = 0;
        if(p.gy >= numGridsY) p.gy = numGridsY - 1;
        
        grids[p.gx][p.gy].pIndices.push_back(i);
    }
}

void FluidDeuxLayer::findNeighbors() {
    numNeighbors = 0;
    for(int i=0; i<particles.size(); i++) {
        FluidDeuxParticle& p = particles[i];
        
        int minX = std::max(0, p.gx - 1);
        int maxX = std::min(numGridsX - 1, p.gx + 1);
        int minY = std::max(0, p.gy - 1);
        int maxY = std::min(numGridsY - 1, p.gy + 1);
        
        for(int gx = minX; gx <= maxX; gx++) {
            for(int gy = minY; gy <= maxY; gy++) {
                findNeighborsInGrid(i, gx, gy);
            }
        }
    }
}

void FluidDeuxLayer::findNeighborsInGrid(int piIdx, int gx, int gy) {
    FluidDeuxGrid& g = grids[gx][gy];
    FluidDeuxParticle& pi = particles[piIdx];
    
    for(int pjIdx : g.pIndices) {
        if(pjIdx >= piIdx) continue;
        
        FluidDeuxParticle& pj = particles[pjIdx];
        float dx = pi.x - pj.x;
        float dy = pi.y - pj.y;
        float distSq = dx*dx + dy*dy;
        
        if(distSq < RANGE2) {
            if(numNeighbors >= neighbors.size()) {
                neighbors.resize(neighbors.size() * 2);
            }
            
            FluidDeuxNeighbor& n = neighbors[numNeighbors++];
            n.p1Idx = piIdx;
            n.p2Idx = pjIdx;
            n.distance = sqrt(distSq);
            n.nx = dx;
            n.ny = dy;
            
            n.weight = 1.0f - n.distance / RANGE;
            float density = n.weight * n.weight;
            
            pi.density += density;
            pj.density += density;
            
            float densityNear = density * n.weight * PRESSURE_NEAR;
            pi.densityNear += densityNear;
            pj.densityNear += densityNear;
            
            float invDist = 1.0f / n.distance;
            n.nx *= invDist;
            n.ny *= invDist;
        }
    }
}

void FluidDeuxLayer::calcForce() {
    for(int i=0; i<numNeighbors; i++) {
        FluidDeuxNeighbor& n = neighbors[i];
        FluidDeuxParticle& p1 = particles[n.p1Idx];
        FluidDeuxParticle& p2 = particles[n.p2Idx];
        
        float p;
        if(p1.type != p2.type)
            p = (p1.density + p2.density - DENSITY * 1.5f) * PRESSURE;
        else
            p = (p1.density + p2.density - DENSITY * 2.0f) * PRESSURE;
            
        float pn = (p1.densityNear + p2.densityNear) * PRESSURE_NEAR;
        float pressureWeight = n.weight * (p + n.weight * pn);
        float viscosityWeight = n.weight * VISCOSITY;
        
        float fx = n.nx * pressureWeight;
        float fy = n.ny * pressureWeight;
        
        fx += (p2.vx - p1.vx) * viscosityWeight;
        fy += (p2.vy - p1.vy) * viscosityWeight;
        
        p1.fx += fx;
        p1.fy += fy;
        p2.fx -= fx;
        p2.fy -= fy;
    }
}

void FluidDeuxLayer::draw() {
    ofPushStyle();
    ofPushMatrix();
    ofScale(scale, scale);
    for(auto& p : particles) {
        ofSetColor(p.color);
        // AS3 draws circles of size 8 (taille)
        ofDrawCircle(p.x, p.y, 4); 
    }
    ofPopMatrix();
    ofPopStyle();
}

void FluidDeuxLayer::mousePressed(float x, float y, int button) {
    isPressed = true;
    mousePos.set(x / scale, y / scale);
}

void FluidDeuxLayer::mouseReleased(float x, float y, int button) {
    isPressed = false;
}