#include "FluidTroisLayer.h"

void FluidTroisLayer::setup(float w, float h, float s) {
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
    
    // Setup FBOs pour l'effet d'accumulation et de flou
    fluidFbo.allocate(simWidth, simHeight, GL_RGBA);
    fluidFbo.begin();
    ofClear(0, 0, 0, 255); // Fond noir opaque (comme la ref AS3)
    fluidFbo.end();
    
    // FBO plus petit pour créer le flou par downsampling
    particleFbo.allocate(simWidth * 0.5f, simHeight * 0.5f, GL_RGBA);
    particleFbo.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
    
    addParticles(2000); // Saturer l'espace dès le début
}

void FluidTroisLayer::setCollider(shared_ptr<ColliderLayer> c) {
    collider = c;
}

void FluidTroisLayer::update(float mouseX, float mouseY, float time) {
    mousePos.set(mouseX / scale, mouseY / scale);
    if(isPressed) pour();
    move();
}

void FluidTroisLayer::addParticles(int count) {
    for(int i=0; i<count; i++) {
        FluidTroisParticle p;
        p.x = ofRandom(simWidth);
        p.y = ofRandom(simHeight);
        p.vx = 0;
        p.vy = 0;
        p.type = (int)ofRandom(4);
        
        switch(p.type) {
            case 0: p.color.set(255, 255, 255); break; // white
            case 1: p.color.set(127, 127, 127); break; // grey
            case 2: p.color.set(0, 0, 0); break;       // black
            case 3: p.color.set(128, 0, 0); break;      // dark red
        }
        particles.push_back(p);
    }
}

void FluidTroisLayer::pour() {
    if(particles.size() > 5000) return;

    for(int i = -2; i <= 2; i++) {
        FluidTroisParticle p;
        p.x = mousePos.x + i * 5;
        p.y = mousePos.y;
        p.vx = 0;
        p.vy = 2;
        p.type = (int)ofRandom(4);
        
        switch(p.type) {
            case 0: p.color.set(255, 255, 255); break; // white
            case 1: p.color.set(127, 127, 127); break; // grey
            case 2: p.color.set(0, 0, 0); break;       // black
            case 3: p.color.set(128, 0, 0); break;      // dark red
        }
        
        particles.push_back(p);
    }
}

void FluidTroisLayer::move() {
    updateGrids();
    findNeighbors();
    setPressure();
    calcForce();
    
    for(auto& p : particles) {
        p.vx += p.fx;
        p.vy += p.fy + GRAVITY;
        
        p.x += p.vx;
        p.y += p.vy;

        // Collision avec ColliderLayer
        if(collider) {
            float r = 2.0f;
            float push = 0.5f;
            float bounce = 1.5f;

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
        float boundary_damping = 0.5f;
        if(p.x < 5) { p.vx += (5 - p.x - p.vx) * boundary_damping; p.x = 5; }
        if(p.x > simWidth - 5) { p.vx += (simWidth - 5 - p.x - p.vx) * boundary_damping; p.x = simWidth - 5; }
        if(p.y < 5) { p.vy += (5 - p.y - p.vy) * boundary_damping; p.y = 5; }
        if(p.y > simHeight - 5) { p.vy += (simHeight - 5 - p.y - p.vy) * boundary_damping; p.y = simHeight - 5; }
    }
}

void FluidTroisLayer::updateGrids() {
    for(int i=0; i<numGridsX; i++) {
        for(int j=0; j<numGridsY; j++) {
            grids[i][j].pIndices.clear();
        }
    }
    
    for(int i=0; i<particles.size(); i++) {
        FluidTroisParticle& p = particles[i];
        p.fx = p.fy = p.density = 0;
        p.gx = (int)(p.x * invGridSize);
        p.gy = (int)(p.y * invGridSize);
        
        if(p.gx < 0) p.gx = 0;
        if(p.gx >= numGridsX) p.gx = numGridsX - 1;
        if(p.gy < 0) p.gy = 0;
        if(p.gy >= numGridsY) p.gy = numGridsY - 1;
        
        grids[p.gx][p.gy].pIndices.push_back(i);
    }
}

void FluidTroisLayer::findNeighbors() {
    numNeighbors = 0;
    for(int i=0; i<particles.size(); i++) {
        FluidTroisParticle& p = particles[i];
        
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

void FluidTroisLayer::findNeighborsInGrid(int piIdx, int gx, int gy) {
    FluidTroisGrid& g = grids[gx][gy];
    FluidTroisParticle& pi = particles[piIdx];
    
    for(int pjIdx : g.pIndices) {
        if(pjIdx >= piIdx) continue;
        
        FluidTroisParticle& pj = particles[pjIdx];
        float dx = pi.x - pj.x;
        float dy = pi.y - pj.y;
        float distSq = dx*dx + dy*dy;
        
        if(distSq < RANGE2) {
            if(numNeighbors >= neighbors.size()) {
                neighbors.resize(neighbors.size() * 2);
            }
            
            FluidTroisNeighbor& n = neighbors[numNeighbors++];
            n.p1Idx = piIdx;
            n.p2Idx = pjIdx;
            n.distance = sqrt(distSq);
            
            n.weight = 1.0f - n.distance / RANGE;
            float temp = n.weight * n.weight * n.weight;
            
            pi.density += temp;
            pj.density += temp;
            
            n.nx = dx;
            n.ny = dy;
            if (n.distance > 0.001f) {
                float invDist = 1.0f / n.distance;
                n.nx *= invDist;
                n.ny *= invDist;
            }

            float a = atan2(n.ny, n.nx) - PI * 0.02f;
            n.ax = cos(a);
            n.ay = sin(a);
        }
    }
}

void FluidTroisLayer::setPressure() {
    for(auto& p : particles) {
        if(p.density < DENSITY) p.density = DENSITY;
        p.pressure = p.density - DENSITY;
    }
}

void FluidTroisLayer::calcForce() {
    for(int i=0; i<numNeighbors; i++) {
        FluidTroisNeighbor& n = neighbors[i];
        FluidTroisParticle& p1 = particles[n.p1Idx];
        FluidTroisParticle& p2 = particles[n.p2Idx];
        
        float total_density = p1.density + p2.density;
        if (total_density < 0.001f) continue;

        float pressureWeight = n.weight * (p1.pressure + p2.pressure) / total_density * PRESSURE;
        float viscosityWeight = n.weight / total_density * VISCOSITY;
        
        p1.fx += n.nx * pressureWeight;
        p1.fy += n.ny * pressureWeight;
        p2.fx -= n.nx * pressureWeight;
        p2.fy -= n.ny * pressureWeight;
        
        float rvx = p2.vx - p1.vx;
        float rvy = p2.vy - p1.vy;
        
        p1.fx += rvx * viscosityWeight;
        p1.fy += rvy * viscosityWeight;
        p2.fx -= rvx * viscosityWeight;
        p2.fy -= rvy * viscosityWeight;

        float f = (p1.type == p2.type ? -ADHESION : ADHESION) * n.weight;
        p1.fx += n.ax * f;
        p1.fy += n.ay * f;
        p2.fx -= n.ax * f;
        p2.fy -= n.ay * f;
    }
}

void FluidTroisLayer::draw() {
    // 1. Dessiner les particules dans le FBO temporaire (réduit pour le flou)
    particleFbo.begin();
    ofClear(0, 0, 0, 0); // Transparent
    ofPushStyle();
    ofScale(0.5f, 0.5f); // On dessine à l'échelle du FBO réduit
    for(auto& p : particles) {
        ofSetColor(p.color);
        ofDrawCircle(p.x, p.y, 3.0f); // Un peu plus gros pour bien mélanger
    }
    ofPopStyle();
    particleFbo.end();

    // 2. Accumuler dans le FBO persistant (Saturer l'espace)
    fluidFbo.begin();
    ofEnableAlphaBlending();
    // Dessiner le FBO flou par dessus l'existant
    particleFbo.draw(0, 0, simWidth, simHeight);
    ofDisableAlphaBlending();
    fluidFbo.end();

    // 3. Afficher le résultat final
    ofPushStyle();
    ofPushMatrix();
    ofScale(scale, scale);
    ofSetColor(255);
    fluidFbo.draw(0, 0);
    ofPopMatrix();
    ofPopStyle();
}

void FluidTroisLayer::mousePressed(float x, float y, int button) {
    isPressed = true;
    mousePos.set(x / scale, y / scale);
}

void FluidTroisLayer::mouseReleased(float x, float y, int button) {
    isPressed = false;
}