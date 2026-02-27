#include "FishSchoolLayer.h"
#include <algorithm> 

//--------------------------------------------------------------
void FishSchoolLayer::setup(float realSceneWidth, float targetRealHeight, shared_ptr<ColliderLayer> colliders) {
    this->colliderLayer = colliders;

    // --- CORRECTION MAJEURE : SYNCHRONISATION DES ESPACES ---
    if(colliderLayer) {
        // On récupère l'échelle et les dimensions du "Maître" (ColliderLayer)
        // pour que la position (100, 100) d'un poisson tombe au même endroit que le mur (100, 100).
        this->simWidth = colliderLayer->simWidth;
        this->simHeight = colliderLayer->simHeight;
        this->scale = colliderLayer->scale;
    } 
    else {
        // Fallback (Ancien comportement si pas de collider)
        float simReferenceHeight = 600.0f; 
        scale = targetRealHeight / simReferenceHeight;
        simHeight = simReferenceHeight;
        simWidth = realSceneWidth / scale;
    }

    // --- AJUSTEMENT DES PARAMETRES PHYSIQUES ---
    // Comme l'échelle de simulation a changé (simWidth est passé de ~7000 à 2048),
    // une distance de 50px est maintenant beaucoup plus grande proportionnellement.
    // On réduit légèrement le RANGE et les vitesses pour garder le comportement "Sardine".
    RANGE = 25.0f; // Était 50.0f
    
    // Grille spatiale
    cols = ceil(simWidth / RANGE);
    rows = ceil(simHeight / RANGE);
    
    grid.resize(cols);
    for(int i=0; i<cols; i++){
        grid[i].resize(rows);
    }

    // Sardines
    // On adapte le nombre de poissons à la nouvelle largeur logique
    int startCount = (int)(simWidth * 0.8); 
    if(startCount > 1200) startCount = 1200;

    for(int i=0; i<startCount; i++){
        Boid p;
        p.type = SARDINE;
        p.pos.set(ofRandom(simWidth), ofRandom(simHeight));
        
        // Eviter de spawner dans un mur
        if(colliderLayer) {
            bool inside = true;
            int safeGuard = 0;
            while(inside && safeGuard < 100) {
                if(!colliderLayer->isWall(p.pos.x, p.pos.y)) inside = false;
                else p.pos.set(ofRandom(simWidth), ofRandom(simHeight));
                safeGuard++;
            }
        }

        p.vel.set(ofRandom(-0.5, 0.5), ofRandom(-0.5, 0.5));
        p.acc.set(0, 0);
        p.color.set(200, 200, 255, 200); 
        particles.push_back(p);
    }
    
    // Requins
    for(int i=0; i<4; i++){
        Boid p;
        p.type = SHARK;
        p.pos.set(ofRandom(simWidth), ofRandom(simHeight));
        p.vel.set(ofRandom(-0.5, 0.5), ofRandom(-0.5, 0.5));
        p.color.set(255, 100, 100, 255); 
        particles.push_back(p);
    }
}

//--------------------------------------------------------------
void FishSchoolLayer::addShark(float realMouseX, float realMouseY) {
    Boid p;
    p.type = SHARK;
    // Conversion correcte Souris -> Simulation
    p.pos.set(realMouseX / scale, realMouseY / scale);
    p.vel.set(ofRandom(-0.5, 0.5), ofRandom(-0.5, 0.5));
    p.color.set(255, 100, 100, 255);
    particles.push_back(p);
}

void FishSchoolLayer::addSardine(float realMouseX, float realMouseY) {
    if(particles.size() >= maxParticles) return;
    Boid p;
    p.type = SARDINE;
    p.pos.set(realMouseX / scale, realMouseY / scale);
    p.vel.set(ofRandom(-0.5, 0.5), ofRandom(-0.5, 0.5));
    p.color.set(200, 200, 255, 200);
    particles.push_back(p);
}

//--------------------------------------------------------------
void FishSchoolLayer::update() {
    // OPTIMISATION : Une seule passe de physique par frame suffit.
    // Cela réduit de 50% la charge CPU de cette couche.
    updateGrid();
    calculateNeighbors();
    moveParticles();
}

//--------------------------------------------------------------
void FishSchoolLayer::updateGrid() {
    for(int i=0; i<cols; i++){
        for(int j=0; j<rows; j++){
            grid[i][j].clear();
        }
    }
    for(auto& p : particles) {
        int mx = (int)(p.pos.x / RANGE);
        int my = (int)(p.pos.y / RANGE);
        if(mx < 0) mx = 0; if(mx >= cols) mx = cols - 1;
        if(my < 0) my = 0; if(my >= rows) my = rows - 1;
        grid[mx][my].push_back(&p);
        p.mapX = mx;
        p.mapY = my;
    }
}

//--------------------------------------------------------------
void FishSchoolLayer::calculateNeighbors() {
     for(auto& pi : particles) {
        pi.neighbor = nullptr;
        pi.distSqToNeighbor = RANGE * RANGE; 

        int mx = pi.mapX;
        int my = pi.mapY;
        int minX = std::max(0, mx - 1);
        int maxX = std::min(cols, mx + 2);
        int minY = std::max(0, my - 1);
        int maxY = std::min(rows, my + 2);

        for(int x = minX; x < maxX; x++) {
            for(int y = minY; y < maxY; y++) {
                for(Boid* pj : grid[x][y]) {
                    if(&pi == pj) continue; 
                    float dx = pj->pos.x - pi.pos.x;
                    float dy = pj->pos.y - pi.pos.y;
                    float d2 = dx*dx + dy*dy;

                    if(pi.type == SHARK) {
                        if(pj->type == SARDINE) {
                            if(pi.neighbor == nullptr || pi.neighbor->type == SHARK || d2 < pi.distSqToNeighbor) {
                                pi.neighbor = pj; pi.distSqToNeighbor = d2; pi.dxToNeighbor = dx; pi.dyToNeighbor = dy;
                            }
                            if(d2 < 64) pj->life = 0; 
                        } else if(pj->type == SHARK && pi.neighbor == nullptr) {
                             if(d2 < pi.distSqToNeighbor) {
                                 pi.neighbor = pj; pi.distSqToNeighbor = d2; pi.dxToNeighbor = dx; pi.dyToNeighbor = dy;
                             }
                        }
                    } else { 
                        if(pj->type == SHARK) {
                            if(pi.neighbor == nullptr || pi.neighbor->type == SARDINE || d2 < pi.distSqToNeighbor) {
                                pi.neighbor = pj; pi.distSqToNeighbor = d2; pi.dxToNeighbor = dx; pi.dyToNeighbor = dy;
                            }
                        } else if(pj->type == SARDINE) {
                            if(pi.neighbor == nullptr || (pi.neighbor->type == SARDINE && d2 < pi.distSqToNeighbor)) {
                                if(ofRandom(1.0) > 0.5) { 
                                    pi.neighbor = pj; pi.distSqToNeighbor = d2; pi.dxToNeighbor = dx; pi.dyToNeighbor = dy;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void FishSchoolLayer::moveParticles() {
    for(int i = particles.size() - 1; i >= 0; i--){
        if(particles[i].life <= 0) particles.erase(particles.begin() + i);
    }

    for(auto& p : particles) {
        if(p.type == SARDINE) behaviorSardine(p);
        else behaviorShark(p);

        // Limites Sol/Plafond
        wall(p, 0.8f, RANGE);

        // --- EVITEMENT DES MURS ---
        avoidWalls(p);

        float maxSpeed = (p.type == SARDINE) ? 0.08f : 0.09f;
        float minSpeed = (p.type == SARDINE) ? 0.0f : 0.03f;
        
        // Boost de vitesse léger car on a réduit l'échelle
        // Mais comme on a réduit RANGE, on garde des vitesses faibles pour l'illusion.
        
        limitSpeed(p, minSpeed, maxSpeed);

        p.vel += p.acc;
        p.pos += p.vel;
        p.vel *= DRAG; 
        p.acc.set(0, 0); 

        // Wrapping X
        if(p.pos.x < 0) p.pos.x += simWidth;
        if(p.pos.x > simWidth) p.pos.x -= simWidth;
        
        if(p.pos.y < 1) { p.pos.y = 1; p.vel.y = 0; }
        if(p.pos.y > simHeight - 1) { p.pos.y = simHeight - 1; p.vel.y = 0; }
    }
}

//--------------------------------------------------------------
void FishSchoolLayer::avoidWalls(Boid& p) {
    if(!colliderLayer) return;

    // Paramètres adaptés à la nouvelle échelle
    float margin = 5.0f; 
    float pushStrength = 0.25f;

    const vector<ofRectangle>& walls = colliderLayer->getWalls();

    for(const auto& w : walls) {
        // Test rapide AABB avec marge
        if(p.pos.x > w.x - margin && p.pos.x < w.x + w.width + margin &&
           p.pos.y > w.y - margin && p.pos.y < w.y + w.height + margin) {
            
            ofVec2f center = w.getCenter();
            ofVec2f dir = p.pos - center;
            
            float distSq = dir.lengthSquared();
            if(distSq > 0.001f) {
                dir.normalize();
                p.acc += dir * pushStrength;
            }
        }
    }
}

// ... comportements standards ...

void FishSchoolLayer::behaviorSardine(Boid& p) {
    if(p.neighbor && p.neighbor->type == SHARK) {
        escape(p, 0.08f); 
    } else {
        thrust(p, 0.05f, 0.2f); 
        if(p.neighbor && p.neighbor->type == SARDINE) {
            guide(p, 0.01f, 0.5f); 
        }
    }
}

void FishSchoolLayer::behaviorShark(Boid& p) {
    if(p.neighbor && p.neighbor->type == SARDINE) {
        chase(p, 0.1f); 
    } else {
        thrust(p, 0.08f, 1.0f);
        if(p.neighbor && p.neighbor->type == SHARK) {
            guide(p, 0, 0.5f); 
        }
    }
}

void FishSchoolLayer::thrust(Boid& p, float f, float r) {
    float vx2 = r * (1.0 - 2.0 * ofRandom(1.0)) + p.vel.x;
    float vy2 = r * (1.0 - 2.0 * ofRandom(1.0)) + p.vel.y;
    float mag = sqrt(vx2*vx2 + vy2*vy2);
    if(mag > 0) {
        float v = f * ofRandom(1.0) / mag;
        p.acc.x += vx2 * v; p.acc.y += vy2 * v;
    }
}

void FishSchoolLayer::guide(Boid& p, float pull, float push) {
    float d = sqrt(p.distSqToNeighbor);
    if(d > 0.001f) {
        float f = -push / (d*d) + pull;
        p.acc.x += p.dxToNeighbor * f; p.acc.y += p.dyToNeighbor * f;
    }
}

void FishSchoolLayer::escape(Boid& p, float f) {
    float r = f / sqrt(p.distSqToNeighbor + 0.001f);
    p.acc.x -= p.dxToNeighbor * r; p.acc.y -= p.dyToNeighbor * r;
}

void FishSchoolLayer::chase(Boid& p, float f) {
    p.acc.x += p.dxToNeighbor * f; p.acc.y += p.dyToNeighbor * f;
}

void FishSchoolLayer::wall(Boid& p, float push, float r) {
    if(p.pos.y < r) p.acc.y += push / p.pos.y;
    else if(p.pos.y > simHeight - r) p.acc.y -= push / (simHeight - p.pos.y);
}

void FishSchoolLayer::limitSpeed(Boid& p, float minSpeed, float maxSpeed) {
    float f2 = p.acc.x * p.acc.x + p.acc.y * p.acc.y;
    if(f2 > 0) {
        if(f2 < minSpeed*minSpeed) {
            float r = minSpeed / sqrt(f2); p.acc *= r;
        } else if(f2 > maxSpeed*maxSpeed) {
            float r = maxSpeed / sqrt(f2); p.acc *= r;
        }
    }
}

//--------------------------------------------------------------
void FishSchoolLayer::draw() {
    // Les murs sont dessinés par le ColliderLayer dans Scene2D.
    // On ne dessine que les poissons.
    
    // OPTIMISATION : Utilisation de meshes statiques pour éviter l'allocation/désallocation
    // de mémoire à chaque frame (très coûteux pour 1200+ particules).
    static ofMesh sardineMesh;
    static ofMesh sharkMesh;
    
    sardineMesh.clear(); // Reset les vertices mais garde la capacité mémoire
    sharkMesh.clear();
    
    if(sardineMesh.getMode() != OF_PRIMITIVE_TRIANGLES) {
        sardineMesh.setMode(OF_PRIMITIVE_TRIANGLES);
        sharkMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    }

    for(auto& p : particles) {
        float x = p.pos.x * scale;
        float y = p.pos.y * scale;
        
        if(p.type == SARDINE) {
            float s = 1.5f * scale; 
            sardineMesh.addVertex(ofVec3f(x, y, 0));
            sardineMesh.addVertex(ofVec3f(x+s, y, 0));
            sardineMesh.addVertex(ofVec3f(x, y+s, 0));
            sardineMesh.addVertex(ofVec3f(x+s, y, 0));
            sardineMesh.addVertex(ofVec3f(x+s, y+s, 0));
            sardineMesh.addVertex(ofVec3f(x, y+s, 0));
            for(int k=0; k<6; k++) sardineMesh.addColor(p.color);
        } else {
            float s = 4.0f * scale; 
            sharkMesh.addVertex(ofVec3f(x-s/2, y-s/2, 0));
            sharkMesh.addVertex(ofVec3f(x+s/2, y-s/2, 0));
            sharkMesh.addVertex(ofVec3f(x-s/2, y+s/2, 0));
            sharkMesh.addVertex(ofVec3f(x+s/2, y-s/2, 0));
            sharkMesh.addVertex(ofVec3f(x+s/2, y+s/2, 0));
            sharkMesh.addVertex(ofVec3f(x-s/2, y+s/2, 0));
            for(int k=0; k<6; k++) sharkMesh.addColor(p.color);
        }
    }
    
    sardineMesh.draw();
    sharkMesh.draw();
}

int FishSchoolLayer::getSardineCount() {
    int count = 0;
    for(auto& p : particles) if(p.type == SARDINE) count++;
    return count;
}
int FishSchoolLayer::getSharkCount() {
    int count = 0;
    for(auto& p : particles) if(p.type == SHARK) count++;
    return count;
}