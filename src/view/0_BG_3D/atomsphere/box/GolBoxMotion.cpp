#include "GolBoxMotion.h"

void GolBoxMotion::setup(float w, float h, float d) {
    box.set(w, h, d);
    box.setResolution(2);
    
    bool bWasArb = ofGetUsingArbTex();
    if(bWasArb) ofDisableArbTex();
    
    fbo.allocate(gridW * cellW, gridH * cellW, GL_RGB);
    fbo.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    if(bWasArb) ofEnableArbTex();
    
    // Dépliage du cube en croix 4x3 pour une continuité parfaite entre les murs voisins
    ofMesh& mesh = box.getMesh();
    for(int i = 0; i < mesh.getNumVertices(); i++) {
        ofVec3f n = mesh.getNormal(i);
        ofVec2f uv = mesh.getTexCoord(i);
        
        // On inverse U pour compenser le ofScale(-1, 1, 1) au moment du draw()
        float u = 1.0f - uv.x;
        float v = uv.y;
        
        if (n.z > 0.5f) { u = (u + 1.0f) / 4.0f; v = (v + 1.0f) / 3.0f; }
        else if (n.z < -0.5f) { u = (u + 3.0f) / 4.0f; v = (v + 1.0f) / 3.0f; }
        else if (n.x > 0.5f) { u = (u + 0.0f) / 4.0f; v = (v + 1.0f) / 3.0f; }
        else if (n.x < -0.5f) { u = (u + 2.0f) / 4.0f; v = (v + 1.0f) / 3.0f; }
        else if (n.y > 0.5f) { u = (u + 1.0f) / 4.0f; v = (1.0f - v) / 3.0f; } // Inversion V
        else if (n.y < -0.5f) { u = (u + 1.0f) / 4.0f; v = ((1.0f - v) + 2.0f) / 3.0f; } // Inversion V
        
        mesh.setTexCoord(i, ofVec2f(u, v));
    }
    
    table.assign(gridW, std::vector<std::shared_ptr<GolMCell>>(gridH, nullptr));
    currentSeed = ofRandom(10000000); // 1er chargement aléatoire
    reset();
}

// Transpose le mouvement orthogonal avec respect parfait des arêtes du cube
ofVec2f GolBoxMotion::getNextManhattan(int x, int y, int dx, int dy) {
    int S = gridW / 4;
    int nx = x + dx;
    int ny = y + dy;
    if (nx >= 0 && nx < gridW && ny >= 0 && ny < gridH) {
        if ((x / S) == (nx / S) && (y / S) == (ny / S)) return ofVec2f(nx, ny);
    }
    int fX = x / S, fY = y / S;
    int lx = x % S, ly = y % S;
    
    if (dx == 1 && lx == S - 1) { 
        if (fY == 1) return ofVec2f(((fX + 1) % 4) * S, 1*S + ly); 
        if (fY == 0 && fX == 1) return ofVec2f(2*S + (S - 1 - ly), 1*S); 
        if (fY == 2 && fX == 1) return ofVec2f(2*S + ly, 1*S + S - 1); 
    }
    else if (dx == -1 && lx == 0) { 
        if (fY == 1) return ofVec2f(((fX + 3) % 4) * S + S - 1, 1*S + ly); 
        if (fY == 0 && fX == 1) return ofVec2f(0*S + ly, 1*S); 
        if (fY == 2 && fX == 1) return ofVec2f(0*S + (S - 1 - ly), 1*S + S - 1); 
    }
    else if (dy == -1 && ly == 0) { 
        if (fY == 0 && fX == 1) return ofVec2f(3*S + (S - 1 - lx), 1*S); 
        if (fY == 2 && fX == 1) return ofVec2f(1*S + lx, 1*S + S - 1); 
        if (fY == 1 && fX == 0) return ofVec2f(1*S, 0*S + lx); 
        if (fY == 1 && fX == 1) return ofVec2f(1*S + lx, 0*S + S - 1); 
        if (fY == 1 && fX == 2) return ofVec2f(1*S + S - 1, 0*S + (S - 1 - lx)); 
        if (fY == 1 && fX == 3) return ofVec2f(1*S + (S - 1 - lx), 0*S); 
    }
    else if (dy == 1 && ly == S - 1) { 
        if (fY == 0 && fX == 1) return ofVec2f(1*S + lx, 1*S); 
        if (fY == 2 && fX == 1) return ofVec2f(3*S + (S - 1 - lx), 1*S + S - 1); 
        if (fY == 1 && fX == 0) return ofVec2f(1*S, 2*S + (S - 1 - lx)); 
        if (fY == 1 && fX == 1) return ofVec2f(1*S + lx, 2*S); 
        if (fY == 1 && fX == 2) return ofVec2f(1*S + S - 1, 2*S + lx); 
        if (fY == 1 && fX == 3) return ofVec2f(1*S + (S - 1 - lx), 2*S + S - 1); 
    }
    return ofVec2f(-1, -1);
}

// Résout le saut de voisinage complexe (Manhattan composé)
ofVec2f GolBoxMotion::getNext(int x, int y, int dx, int dy) {
    if (dx != 0 && dy != 0) {
        ofVec2f px = getNextManhattan(x, y, dx, 0);
        if (px.x != -1) return getNextManhattan(px.x, px.y, 0, dy);
        return ofVec2f(-1, -1);
    }
    return getNextManhattan(x, y, dx, dy);
}

void GolBoxMotion::reset() {
    allCells.clear();
    int S = gridW / 4;
    long backupSeed = ofRandom(10000000); // Sauvegarde l'état du flux aléatoire global
    ofSetRandomSeed(currentSeed);
    for(int i = 0; i < gridW; i++) {
        for(int j = 0; j < gridH; j++) {
            table[i][j] = nullptr;
            int fX = i / S; int fY = j / S;
            if ((fY == 1) || (fY == 0 && fX == 1) || (fY == 2 && fX == 1)) {
                if(ofRandom(1.0f) < 0.5f) {
                    auto c = std::make_shared<GolMCell>(i, j, cellW);
                    table[i][j] = c;
                    allCells.push_back(c);
                }
            }
        }
    }
    tim = 0;
    ofSetRandomSeed(backupSeed); // Restaure le flux aléatoire global
}

bool GolBoxMotion::dfs(const std::vector<std::vector<int>>& adj, int u, std::vector<int>& match, std::vector<bool>& used) {
    for(int v : adj[u]) {
        if(used[v]) continue;
        used[v] = true;
        if(match[v] < 0 || dfs(adj, match[v], match, used)) {
            match[v] = u;
            return true;
        }
    }
    return false;
}

void GolBoxMotion::update() {
    if(bPaused) return;

    for(auto& c : allCells) c->step();
    allCells.erase(std::remove_if(allCells.begin(), allCells.end(), [](const std::shared_ptr<GolMCell>& c){ return c->vanished; }), allCells.end());

    if(tim > period) {
        tim = 0;
        std::vector<std::vector<int>> stp(gridW, std::vector<int>(gridH, 0));
        for(int i = 0; i < gridW; i++){
            for(int j = 0; j < gridH; j++){
                if(table[i][j]) {
                    for(int dx = -1; dx <= 1; dx++){
                        for(int dy = -1; dy <= 1; dy++){
                            if(dx == 0 && dy == 0) continue;
                            ofVec2f n = getNext(i, j, dx, dy);
                            if(n.x != -1) stp[(int)n.x][(int)n.y]++;
                        }
                    }
                }
            }
        }

        std::vector<std::vector<int>> adj(gridW * gridH);
        for(int i = 0; i < gridW; i++){
            for(int j = 0; j < gridH; j++){
                if(table[i][j] && stp[i][j] != 2 && stp[i][j] != 3) {
                    for(int dx = -1; dx <= 1; dx++){
                        for(int dy = -1; dy <= 1; dy++){
                            if(dx == 0 && dy == 0) continue;
                            ofVec2f n = getNext(i, j, dx, dy);
                            if(n.x != -1) {
                                int nx = (int)n.x; int ny = (int)n.y;
                                if(!table[nx][ny] && stp[nx][ny] == 3) {
                                    adj[i * gridH + j].push_back(nx * gridH + ny);
                                }
                            }
                        }
                    }
                }
            }
        }

        std::vector<int> match(gridW * gridH, -1);
        for(int i = 0; i < gridW * gridH; i++){
            if(adj[i].empty()) continue;
            std::vector<bool> used(gridW * gridH, false);
            dfs(adj, i, match, used);
        }

        std::vector<std::vector<std::shared_ptr<GolMCell>>> nextTable(gridW, std::vector<std::shared_ptr<GolMCell>>(gridH, nullptr));
        std::vector<bool> dyingMatched(gridW * gridH, false);
        
        for(int v = 0; v < gridW * gridH; v++){
            if(match[v] != -1) {
                int u = match[v];
                dyingMatched[u] = true;
                int nx = v / gridH, ny = v % gridH;
                int ox = u / gridH, oy = u % gridH;
                auto c = table[ox][oy];
                c->move(nx, ny);
                nextTable[nx][ny] = c;
            }
        }

        for(int i = 0; i < gridW; i++){
            for(int j = 0; j < gridH; j++){
                int u = i * gridH + j;
                if(table[i][j]) {
                    if(stp[i][j] == 2 || stp[i][j] == 3) {
                        nextTable[i][j] = table[i][j];
                    } else if(!dyingMatched[u]) {
                        table[i][j]->del();
                    }
                } else {
                    int S = gridW / 4;
                    int fX = i / S; int fY = j / S;
                    if ((fY == 1) || (fY == 0 && fX == 1) || (fY == 2 && fX == 1)) {
                        if(stp[i][j] == 3 && match[u] == -1) {
                            auto c = std::make_shared<GolMCell>(i, j, cellW);
                            allCells.push_back(c);
                            nextTable[i][j] = c;
                        }
                    }
                }
            }
        }
        table = nextTable;
    }
    tim++;

    fbo.begin();
    ofClear(25, 25, 25, 255);
    ofSetColor(255);
    for(auto& c : allCells) c->draw();
    fbo.end();
}

void GolBoxMotion::draw() {
    ofPushStyle();
    ofEnableDepthTest();
    ofSetColor(255);
    
    fbo.getTexture().bind();
    ofPushMatrix();
    ofTranslate(0, box.getHeight() / 2.0f - 100.0f, 0); 
    ofScale(-1, 1, 1);
    box.draw();
    ofPopMatrix();
    fbo.getTexture().unbind();
    
    ofPopStyle();
}