#include "AutoSnakeBox.h"

void AutoSnakeBox::setup(float w, float h, float d) {
    // On définit la box très légèrement plus grande (+4) que la Room pour éviter le Z-fight
    // tout en gardant les cases visuellement alignées de façon parfaite sur les arêtes.
    box.set(w + 4.0f, h + 4.0f, d + 4.0f);
    box.setResolution(2);
    
    bool bWasArb = ofGetUsingArbTex();
    if(bWasArb) ofDisableArbTex();
    
    fbo.allocate(gridW * cellW, gridH * cellW, GL_RGB);
    fbo.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    if(bWasArb) ofEnableArbTex();
    
    // Dépliage du cube en croix 4x3
    ofMesh& mesh = box.getMesh();
    for(int i = 0; i < mesh.getNumVertices(); i++) {
        ofVec3f n = mesh.getNormal(i);
        ofVec2f uv = mesh.getTexCoord(i);
        
        // On inverse U pour compenser le ofScale(-1, 1, 1) au moment du draw()
        float u = 1.0f - uv.x; 
        float v = uv.y;

        if (n.z > 0.5f) { u = (u + 1.0f) / 4.0f; v = (v + 1.0f) / 3.0f; }       // Front
        else if (n.z < -0.5f) { u = (u + 3.0f) / 4.0f; v = (v + 1.0f) / 3.0f; } // Back
        else if (n.x > 0.5f) { u = (u + 0.0f) / 4.0f; v = (v + 1.0f) / 3.0f; }  // Right (+X) -> affiche le FBO Gauche
        else if (n.x < -0.5f) { u = (u + 2.0f) / 4.0f; v = (v + 1.0f) / 3.0f; } // Left (-X) -> affiche le FBO Droit
        else if (n.y > 0.5f) { 
            u = (u + 1.0f) / 4.0f; 
            v = (1.0f - v) / 3.0f; 
        }  // Top (Inversion de V pour l'axe Back/Front)
        else if (n.y < -0.5f) { 
            u = (u + 1.0f) / 4.0f; 
            v = ((1.0f - v) + 2.0f) / 3.0f; 
        } // Bottom (Inversion de V pour l'axe Back/Front)
        
        mesh.setTexCoord(i, ofVec2f(u, v));
    }
    
    reset();
}

void AutoSnakeBox::reset() {
    snake.clear();
    int S = gridW / 4;
    int cx = 1 * S + S / 2; // Centre du mur Front
    int cy = 1 * S + S / 2;
    snake.push_back(ofVec2f(cx, cy));
    spawnPoint();
    drawToFbo();
}

void AutoSnakeBox::spawnPoint() {
    int S = gridW / 4;
    std::vector<ofVec2f> valid;
    for(int x = 0; x < gridW; x++) {
        for(int y = 0; y < gridH; y++) {
            int faceX = x / S;
            int faceY = y / S;
            if ((faceY == 1) || (faceY == 0 && faceX == 1) || (faceY == 2 && faceX == 1)) {
                ofVec2f p(x,y);
                bool body = false;
                for(auto& s : snake) if(s == p) { body = true; break; }
                if(!body) valid.push_back(p);
            }
        }
    }
    if(!valid.empty()) point = valid[ofRandom(valid.size())];
}

// Calcule l'intersection Rayon-Cube pour placer l'objectif manuellement
void AutoSnakeBox::setTargetFromRay(ofVec3f rayOrigin, ofVec3f rayDir) {
    // On compense les transformations appliquées lors du draw() de la box (translate & scale)
    // Le vrai centre Y de la room est à (hauteur_box - padding_de_4) / 2
    ofVec3f offset(0, (box.getHeight() - 4.0f) / 2.0f, 0);
    ofVec3f originLocal = rayOrigin - offset;
    originLocal.x *= -1; 
    ofVec3f dirLocal = rayDir;
    dirLocal.x *= -1;
    dirLocal.normalize();
    
    ofMesh& mesh = box.getMesh();
    float minT = 100000.0f;
    bool hit = false;
    ofVec2f hitUV;
    
    // Algorithme d'intersection Möller–Trumbore sur les faces du cube
    for(int i = 0; i < mesh.getNumIndices(); i += 3) {
        int i0 = mesh.getIndex(i); int i1 = mesh.getIndex(i+1); int i2 = mesh.getIndex(i+2);
        ofVec3f v0 = mesh.getVertex(i0); ofVec3f v1 = mesh.getVertex(i1); ofVec3f v2 = mesh.getVertex(i2);
        
        ofVec3f v0v1 = v1 - v0; ofVec3f v0v2 = v2 - v0;
        ofVec3f pvec = dirLocal.getCrossed(v0v2);
        float det = v0v1.dot(pvec);
        if (fabs(det) < 0.00001) continue;
        
        float invDet = 1.0f / det;
        ofVec3f tvec = originLocal - v0;
        float u = tvec.dot(pvec) * invDet;
        if (u < 0 || u > 1) continue;
        
        ofVec3f qvec = tvec.getCrossed(v0v1);
        float v = dirLocal.dot(qvec) * invDet;
        if (v < 0 || u + v > 1) continue;
        
        float t = v0v2.dot(qvec) * invDet;
        if(t > 0 && t < minT) {
            minT = t; hit = true;
            ofVec2f uv0 = mesh.getTexCoord(i0); ofVec2f uv1 = mesh.getTexCoord(i1); ofVec2f uv2 = mesh.getTexCoord(i2);
            hitUV = uv0 * (1.0f - u - v) + uv1 * u + uv2 * v; // Interpolation barycentrique des UVs
        }
    }
    
    if (hit) {
        int targetX = ofClamp(hitUV.x * gridW, 0, gridW - 1);
        int targetY = ofClamp(hitUV.y * gridH, 0, gridH - 1);
        ofVec2f p(targetX, targetY);
        
        bool isBody = false;
        for(auto& s : snake) if(s == p) { isBody = true; break; }
        if(!isBody) { point = p; drawToFbo(); }
    }
}

// Logique mathématique avancée gérant les coutures de la croix 4x3 du FBO
// pour que les bordures donnent l'illusion d'un parcours 3D continu sur les 6 faces.
ofVec2f AutoSnakeBox::getNext(int x, int y, int dx, int dy) {
    int S = gridW / 4;
    
    // Déplacement standard à l'intérieur d'une même face du FBO
    int nx = x + dx;
    int ny = y + dy;
    if (nx >= 0 && nx < gridW && ny >= 0 && ny < gridH) {
        int faceX = x / S, faceY = y / S;
        int nFaceX = nx / S, nFaceY = ny / S;
        if (faceX == nFaceX && faceY == nFaceY) {
            return ofVec2f(nx, ny);
        }
    }
    
    int faceX = x / S, faceY = y / S;
    int lx = x % S, ly = y % S;
    
    // Cartographie EXPLICITE et PARFAITE de toutes les arêtes pour le cube 3D
    if (dx == 1 && lx == S - 1) { // Sortie Droite
        if (faceY == 1) return ofVec2f(((faceX + 1) % 4) * S, 1*S + ly); 
        if (faceY == 0 && faceX == 1) return ofVec2f(2*S + (S - 1 - ly), 1*S); 
        if (faceY == 2 && faceX == 1) return ofVec2f(2*S + ly, 1*S + S - 1); 
    }
    else if (dx == -1 && lx == 0) { // Sortie Gauche
        if (faceY == 1) return ofVec2f(((faceX + 3) % 4) * S + S - 1, 1*S + ly); 
        if (faceY == 0 && faceX == 1) return ofVec2f(0*S + ly, 1*S); 
        if (faceY == 2 && faceX == 1) return ofVec2f(0*S + (S - 1 - ly), 1*S + S - 1); 
    }
    else if (dy == -1 && ly == 0) { // Sortie Haut
        if (faceY == 0 && faceX == 1) return ofVec2f(3*S + (S - 1 - lx), 1*S); 
        if (faceY == 2 && faceX == 1) return ofVec2f(1*S + lx, 1*S + S - 1); 
        if (faceY == 1 && faceX == 0) return ofVec2f(1*S, 0*S + lx); 
        if (faceY == 1 && faceX == 1) return ofVec2f(1*S + lx, 0*S + S - 1); 
        if (faceY == 1 && faceX == 2) return ofVec2f(1*S + S - 1, 0*S + (S - 1 - lx)); 
        if (faceY == 1 && faceX == 3) return ofVec2f(1*S + (S - 1 - lx), 0*S); 
    }
    else if (dy == 1 && ly == S - 1) { // Sortie Bas
        if (faceY == 0 && faceX == 1) return ofVec2f(1*S + lx, 1*S); 
        if (faceY == 2 && faceX == 1) return ofVec2f(3*S + (S - 1 - lx), 1*S + S - 1); 
        if (faceY == 1 && faceX == 0) return ofVec2f(1*S, 2*S + (S - 1 - lx)); 
        if (faceY == 1 && faceX == 1) return ofVec2f(1*S + lx, 2*S); 
        if (faceY == 1 && faceX == 2) return ofVec2f(1*S + S - 1, 2*S + lx); 
        if (faceY == 1 && faceX == 3) return ofVec2f(1*S + (S - 1 - lx), 2*S + S - 1); 
    }
    return ofVec2f(-1, -1);
}

std::vector<ofVec2f> AutoSnakeBox::findPath() {
    if(snake.empty()) return {};
    ofVec2f start = snake.front(), goal = point;
    
    std::vector<int> cameFrom(3072, -1);
    std::vector<int> gScore(3072, -1);
    std::vector<bool> isBody(3072, false);
    
    auto getIndex = [&](ofVec2f p) { return (int)p.y * gridW + (int)p.x; };
    auto getPoint = [&](int idx) { return ofVec2f(idx % gridW, idx / gridW); };
    
    for(size_t i=0; i<snake.size()-1; i++) isBody[getIndex(snake[i])] = true;
    
    std::deque<int> openSet;
    int startIdx = getIndex(start), goalIdx = getIndex(goal);
    
    openSet.push_back(startIdx);
    gScore[startIdx] = 0;
    
    bool found = false;
    while(!openSet.empty()) {
        int currIdx = openSet.front(); openSet.pop_front();
        if(currIdx == goalIdx) { found = true; break; }
        
        ofVec2f curr = getPoint(currIdx);
        ofVec2f dirs[4] = {ofVec2f(1,0), ofVec2f(-1,0), ofVec2f(0,1), ofVec2f(0,-1)};
        
        for(auto& d : dirs) {
            ofVec2f nextP = getNext(curr.x, curr.y, d.x, d.y);
            if(nextP.x != -1) {
                int nIdx = getIndex(nextP);
                if(!isBody[nIdx] && gScore[nIdx] == -1) {
                    gScore[nIdx] = gScore[currIdx] + 1;
                    cameFrom[nIdx] = currIdx;
                    openSet.push_back(nIdx);
                }
            }
        }
    }
    
    if(found) {
        std::vector<ofVec2f> path; int trace = goalIdx;
        while(trace != startIdx) { path.push_back(getPoint(trace)); trace = cameFrom[trace]; }
        std::reverse(path.begin(), path.end());
        return path;
    }
    return {};
}

ofVec2f AutoSnakeBox::fallbackMove() {
    ofVec2f curr = snake.front();
    ofVec2f dirs[4] = {ofVec2f(1,0), ofVec2f(-1,0), ofVec2f(0,1), ofVec2f(0,-1)};
    for(auto& d : dirs) {
        ofVec2f nextP = getNext(curr.x, curr.y, d.x, d.y);
        if(nextP.x != -1) {
            bool body = false;
            for(size_t i=0; i<snake.size()-1; i++) if(snake[i] == nextP) { body = true; break; }
            if(!body) return nextP;
        }
    }
    return ofVec2f(-1, -1);
}

void AutoSnakeBox::update() {
    if(bPaused || snake.empty() || frameCount++ % speed != 0) return;
    
    std::vector<ofVec2f> path = findPath();
    ofVec2f nextMove = !path.empty() ? path[0] : fallbackMove();
    
    if(nextMove.x == -1) { reset(); return; } // Si le serpent est coincé, il meurt et repart à zero
    
    snake.push_front(nextMove);
    if(nextMove == point) spawnPoint();
    else snake.pop_back();
    
    drawToFbo();
}

void AutoSnakeBox::drawToFbo() {
    fbo.begin(); ofClear(25, 25, 25, 255);
    int S = gridW / 4;
    for(int x=0; x<gridW; x++) for(int y=0; y<gridH; y++) {
        if ((y/S == 1) || (y/S == 0 && x/S == 1) || (y/S == 2 && x/S == 1)) {
            ofSetColor(((x+y)%2==0)?35:45); ofDrawRectangle(x*cellW, y*cellW, cellW, cellW);
        }
    }
    for(size_t i=0; i<snake.size(); i++) {
        ofColor c;
        c.setHsb(fmod(i * (255.0f / 50.0f), 255.0f), 200, 255); // Dégradé HSB qui boucle sur 50 cases
        if (i == 0) c = ofColor(255); // La tête en blanc pour se repérer
        
        ofSetColor(c);
        ofDrawRectangle(snake[i].x * cellW + 1, snake[i].y * cellW + 1, cellW - 2, cellW - 2);
        
        ofSetColor(0); // Texte en noir pour être lisible sur l'arc-en-ciel
        ofPushMatrix();
        ofTranslate(snake[i].x * cellW + 1, snake[i].y * cellW + 11);
        ofScale(0.6f, 0.6f); // On réduit la taille du texte pour qu'il rentre dans la case (16px)
        ofDrawBitmapString(ofToString(i), 0, 0);
        ofPopMatrix();
    }
    ofSetColor(255, 255, 0); ofDrawRectangle(point.x * cellW + 1, point.y * cellW + 1, cellW - 2, cellW - 2);
    fbo.end();
}

void AutoSnakeBox::draw() {
    ofPushStyle(); ofEnableDepthTest(); ofSetColor(255);
    fbo.getTexture().bind();
    ofPushMatrix();
    ofTranslate(0, (box.getHeight() - 4.0f) / 2.0f, 0); 
    ofScale(-1, 1, 1);
    box.draw();
    ofPopMatrix();
    fbo.getTexture().unbind();
    ofPopStyle();
}