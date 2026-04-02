#include "GolBox.h"

void GolBox::setup(float w, float h, float d) {
    box.set(w, h, d);
    box.setResolution(2);
    
    // Calcul de la résolution de la grille interne en fonction du CellSize
    // On utilise un ratio 4x3 pour déplier le cube en croix (Cubemap cross) afin de mapper
    // toute la matrice en une seule texture continue sur les murs.
    int baseW = 2048;
    int baseH = 1536; 
    gridW = baseW / cellSize;
    gridH = baseH / cellSize;
    
    pixelsFront.allocate(gridW, gridH, OF_PIXELS_RGB);
    pixelsBack.allocate(gridW, gridH, OF_PIXELS_RGB);
    
    currentSeed = ofRandom(10000000);
    
    reset();
    
    bool bWasArb = ofGetUsingArbTex();
    if(bWasArb) ofDisableArbTex(); // Force l'utilisation d'UVs normalisés (0..1)
    
    // Texture sans lissage pour un look "pixel net"
    tex.allocate(pixelsFront);
    tex.setTextureWrap(GL_REPEAT, GL_REPEAT);
    tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    if(bWasArb) ofEnableArbTex();
    
    // Dépliage du cube en croix 4x3 pour une continuité parfaite entre les murs voisins
    ofMesh& mesh = box.getMesh();
    for(int i = 0; i < mesh.getNumVertices(); i++) {
        ofVec3f n = mesh.getNormal(i);
        ofVec2f uv = mesh.getTexCoord(i);
        
        // On inverse U pour compenser le ofScale(-1, 1, 1) au moment du draw()
        float u = 1.0f - uv.x;
        float v = uv.y;
        
        // Placement de chaque face sur la grille globale 4x3
        if (n.z > 0.5f) {        // Front (1, 1)
            u = (u + 1.0f) / 4.0f; v = (v + 1.0f) / 3.0f;
        } else if (n.z < -0.5f) { // Back (3, 1)
            u = (u + 3.0f) / 4.0f; v = (v + 1.0f) / 3.0f;
        } else if (n.x > 0.5f) {  // Right (+X) -> affiche le FBO Gauche
            u = (u + 0.0f) / 4.0f; v = (v + 1.0f) / 3.0f;
        } else if (n.x < -0.5f) { // Left (-X) -> affiche le FBO Droit
            u = (u + 2.0f) / 4.0f; v = (v + 1.0f) / 3.0f;
        } else if (n.y > 0.5f) {  // Top (1, 0) -> V inversé pour l'axe Back/Front
            u = (u + 1.0f) / 4.0f; v = (1.0f - v) / 3.0f;
        } else if (n.y < -0.5f) { // Bottom (1, 2) -> V inversé pour l'axe Back/Front
            u = (u + 1.0f) / 4.0f; v = ((1.0f - v) + 2.0f) / 3.0f;
        }
        mesh.setTexCoord(i, ofVec2f(u, v));
    }
}

// Transpose le mouvement orthogonal avec respect parfait des arêtes du cube
ofVec2f GolBox::getNextManhattan(int x, int y, int dx, int dy) {
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
ofVec2f GolBox::getNext(int x, int y, int dx, int dy) {
    if (dx != 0 && dy != 0) {
        ofVec2f px = getNextManhattan(x, y, dx, 0);
        if (px.x != -1) return getNextManhattan(px.x, px.y, 0, dy);
        return ofVec2f(-1, -1);
    }
    return getNextManhattan(x, y, dx, dy);
}

void GolBox::reset() {
    unsigned char* dst = pixelsFront.getData();
    int total = gridW * gridH * 3; // 3 canaux (RGB)
    
    // Nettoyage de l'espace mort
    for(int i = 0; i < total; i++) {
        dst[i] = 0;
    }
    
    long backupSeed = ofRandom(10000000);
    ofSetRandomSeed(currentSeed);
    
    int S = gridW / 4;
    // Initialisation aléatoire uniquement sur les 6 faces valides (Indépendant pour R, G, B)
    for(int x = 0; x < gridW; x++) {
        for(int y = 0; y < gridH; y++) {
            int fX = x / S; int fY = y / S;
            if ((fY == 1) || (fY == 0 && fX == 1) || (fY == 2 && fX == 1)) {
                int idx = (y * gridW + x) * 3;
                dst[idx]   = (ofRandom(1.0f) > 0.5f) ? 255 : 0;
                dst[idx+1] = (ofRandom(1.0f) > 0.5f) ? 255 : 0;
                dst[idx+2] = (ofRandom(1.0f) > 0.5f) ? 255 : 0;
            }
        }
    }
    ofSetRandomSeed(backupSeed);
    pixelsBack = pixelsFront;
    tex.loadData(pixelsFront);
}

void GolBox::update() {
    frameCount++;
    if(frameCount % 2 != 0) return; // 30 FPS interne pour bien voir l'animation évoluer

    const unsigned char* src = pixelsFront.getData();
    unsigned char* dst = pixelsBack.getData();
    int w = gridW;
    int h = gridH;
    int S = w / 4;

    // On ne met à jour QUE les cellules valides
    for(int x = 0; x < w; x++) {
        for(int y = 0; y < h; y++) {
            int fX = x / S;
            int fY = y / S;
            if (!((fY == 1) || (fY == 0 && fX == 1) || (fY == 2 && fX == 1))) continue;

            int r = 0, g = 0, b = 0;
            int lx = x % S;
            int ly = y % S;

            // Fast Branch (~99% des pixels) : si on est loin des bords 3D, on utilise l'offset mémoire natif ultra-rapide
            if (lx > 0 && lx < S - 1 && ly > 0 && ly < S - 1) {
                int rowTop = (y - 1) * w * 3;
                int rowMid = y * w * 3;
                int rowBot = (y + 1) * w * 3;
                int xLeft = (x - 1) * 3;
                int xRight = (x + 1) * 3;
                int xMid = x * 3;

                r = (src[rowTop + xLeft] + src[rowTop + xMid] + src[rowTop + xRight] + 
                     src[rowMid + xLeft] + src[rowMid + xRight] + 
                     src[rowBot + xLeft] + src[rowBot + xMid] + src[rowBot + xRight]) / 255;
                g = (src[rowTop + xLeft + 1] + src[rowTop + xMid + 1] + src[rowTop + xRight + 1] + 
                     src[rowMid + xLeft + 1] + src[rowMid + xRight + 1] + 
                     src[rowBot + xLeft + 1] + src[rowBot + xMid + 1] + src[rowBot + xRight + 1]) / 255;
                b = (src[rowTop + xLeft + 2] + src[rowTop + xMid + 2] + src[rowTop + xRight + 2] + 
                     src[rowMid + xLeft + 2] + src[rowMid + xRight + 2] + 
                     src[rowBot + xLeft + 2] + src[rowBot + xMid + 2] + src[rowBot + xRight + 2]) / 255;
            } else {
                // Slow Branch (~1% des pixels) : on interroge la logique géométrique des arêtes
                for(int dy = -1; dy <= 1; dy++) {
                    for(int dx = -1; dx <= 1; dx++) {
                        if(dx == 0 && dy == 0) continue;
                        ofVec2f n = getNext(x, y, dx, dy);
                        if(n.x != -1) {
                            int nIdx = ((int)n.y * w + (int)n.x) * 3;
                            r += src[nIdx] / 255;
                            g += src[nIdx + 1] / 255;
                            b += src[nIdx + 2] / 255;
                        }
                    }
                }
            }

            int idx = (y * w + x) * 3;
            // Règles standards GOL (Naissance à 3, Survie à 2 ou 3)
            dst[idx]   = (src[idx] > 127)   ? ((r == 2 || r == 3) ? 255 : 0) : ((r == 3) ? 255 : 0);
            dst[idx+1] = (src[idx+1] > 127) ? ((g == 2 || g == 3) ? 255 : 0) : ((g == 3) ? 255 : 0);
            dst[idx+2] = (src[idx+2] > 127) ? ((b == 2 || b == 3) ? 255 : 0) : ((b == 3) ? 255 : 0);
        }
    }
    
    pixelsFront = pixelsBack;
    tex.loadData(pixelsFront); // Envoi au GPU
}

void GolBox::draw() {
    ofPushStyle();
    ofEnableDepthTest();
    ofSetColor(255);
    
    tex.bind();
    ofPushMatrix();
    ofTranslate(0, box.getHeight() / 2.0f - 100.0f, 0); 
    ofScale(-1, 1, 1);
    box.draw();
    ofPopMatrix();
    tex.unbind();
    
    ofPopStyle();
}
