#include "Avoider4Layer.h"

void Avoider4Layer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    player.pos.set(w * 0.5f, h - 300.0f);
    player.radius = 10.0f;
}

float Avoider4Layer::getMint(float rx, float ry, float rvx, float rvy) {
    float vSq = rvx*rvx + rvy*rvy;
    if (vSq < 0.01f) return 0.0f;
    return -(rx*rvx + ry*rvy) / vSq;
}

float Avoider4Layer::getMind2(float rx, float ry, float rvx, float rvy, float t) {
    float xx = rx + rvx * t;
    float yy = ry + rvy * t;
    return xx*xx + yy*yy;
}

float Avoider4Layer::calcSurvivalTime(float px, float py, float vx, float vy, int delay, int depth) {
    float minc = 9999.0f;
    float infd = (12.0f + player.radius) * (12.0f + player.radius);
    
    for (auto& b : bullets) {
        float rx = (b.pos.x + b.vel.x * delay) - px;
        if (rx > simWidth * 0.5f) rx -= simWidth;
        else if (rx < -simWidth * 0.5f) rx += simWidth;
        
        if (abs(rx) > 800.0f) continue; // Tunnel vision pour CPU
        
        // Culling vertical strict pour éviter de calculer les balles derrière
        if (b.pos.y > py + 200.0f) continue; 
        
        float ry = (b.pos.y + b.vel.y * delay) - py;
        float rvx = b.vel.x - vx;
        float rvy = b.vel.y - vy;
        
        float t = getMint(rx, ry, rvx, rvy);
        if (t > 0) {
            float d2 = getMind2(rx, ry, rvx, rvy, t);
            if (d2 < infd) {
                t -= sqrt((infd - d2) / (rvx*rvx + rvy*rvy));
                if (t < minc) minc = t;
            }
        }
    }
    
    if (minc > 45.0f || depth <= 0) return minc;
    
    float maxSub = 0.0f;
    int interval = 5;
    int steps = minc / interval;
    if (steps <= 0) steps = 1;
    int maxt = steps * interval;
    
    float nextPx = px + vx * maxt;
    float nextPy = py + vy * maxt;
    if (nextPx < 0) nextPx += simWidth;
    if (nextPx > simWidth) nextPx -= simWidth;
    
    if (nextPy < simHeight - 600.0f || nextPy > simHeight - 50.0f) return minc; // Bloque les murs fictifs
    
    float moves[9][2] = {{0,0}, {12,0}, {0,12}, {-12,0}, {0,-12}, {8,8}, {-8,8}, {-8,-8}, {8,-8}};
    for (int i=0; i<9; i++) {
        float st = calcSurvivalTime(nextPx, nextPy, moves[i][0], moves[i][1], delay + maxt, depth - 1);
        if (st > maxSub) maxSub = st;
    }
    
    return maxt + maxSub;
}

int Avoider4Layer::getBestMove() {
    float moves[9][2] = {{0,0}, {12,0}, {0,12}, {-12,0}, {0,-12}, {8,8}, {-8,8}, {-8,-8}, {8,-8}};
    float maxTime = -1.0f;
    int bestIdx = 0;
    
    for (int i=0; i<9; i++) {
        float testPy = player.pos.y + moves[i][1];
        if (testPy < simHeight - 600.0f || testPy > simHeight - 50.0f) continue;
        
        float st = calcSurvivalTime(player.pos.x, player.pos.y, moves[i][0], moves[i][1], 0, 3); // Depth 3
        if (st > maxTime) {
            maxTime = st;
            bestIdx = i;
        }
    }
    return bestIdx;
}

void Avoider4Layer::update(float time) {
    frameCount++;
    
    int numSpawn = 2; // Pluie très intense et ultra-ciblée
    for(int i = 0; i < numSpawn; i++) {
        Av4Bullet b;
        float spawnX = player.pos.x + ofRandom(-800, 800);
        if (spawnX < 0) spawnX += simWidth;
        if (spawnX >= simWidth) spawnX -= simWidth;
        b.pos.set(spawnX, -50);
        b.vel.set(ofRandom(-4.0f, 4.0f), ofRandom(6.0f, 12.0f));
        b.radius = 12.0f;
        bullets.push_back(b);
    }
    
    int moveIdx = getBestMove();
    float moves[9][2] = {{0,0}, {12,0}, {0,12}, {-12,0}, {0,-12}, {8,8}, {-8,8}, {-8,-8}, {8,-8}};
    player.pos.x += moves[moveIdx][0];
    player.pos.y += moves[moveIdx][1];
    
    if (player.pos.x < 0) player.pos.x += simWidth;
    if (player.pos.x > simWidth) player.pos.x -= simWidth;
    player.pos.y = ofClamp(player.pos.y, simHeight - 600.0f, simHeight - 50.0f); // Contraint au fond de l'écran
    
    for (int i = bullets.size()-1; i>=0; i--) {
        bullets[i].pos += bullets[i].vel;
        if (bullets[i].pos.x < 0) bullets[i].pos.x += simWidth;
        if (bullets[i].pos.x > simWidth) bullets[i].pos.x -= simWidth;
        if (bullets[i].pos.y > simHeight + 50) bullets.erase(bullets.begin() + i);
    }
}

void Avoider4Layer::draw() {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    vector<ofVec2f> offsets = {ofVec2f(0, 0), ofVec2f(simWidth, 0), ofVec2f(-simWidth, 0)};
    for (auto& off : offsets) {
        // Halo et Pluie
        for (auto& b : bullets) {
            if (abs(b.pos.x + off.x - player.pos.x) > 1500) continue;
            ofSetColor(100, 150, 150, 150); 
            ofDrawCircle(b.pos + off, b.radius + 3.0f);
            ofSetColor(255); 
            ofDrawCircle(b.pos + off, b.radius * 0.6f);
        }
        
        // Joueur (Halo bleu clair éclatant)
        ofSetColor(0, 100, 255, 200);
        ofDrawCircle(player.pos + off, player.radius + 6.0f);
        ofSetColor(150, 200, 255); // Cœur coloré !
        ofDrawCircle(player.pos + off, player.radius);
    }
    ofPopStyle();
}