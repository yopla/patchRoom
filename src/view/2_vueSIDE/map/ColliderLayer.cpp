#include "ColliderLayer.h"

//--------------------------------------------------------------
void ColliderLayer::setup(float simulationWidth, float simulationHeight, float displayScale) {
       ofSetRandomSeed(42);
    simWidth = simulationWidth;
    simHeight = simulationHeight;
    scale = displayScale;

    loadMap(currentMapPath);
}

//--------------------------------------------------------------
void ColliderLayer::loadMap(string path) {
    currentMapPath = path;
    ofImage mapImg;
    if(mapImg.load(currentMapPath)) {
        float imgW = mapImg.getWidth();
        float imgH = mapImg.getHeight();
        float imgRatio = imgW / imgH;

        // Support dynamique: Ancienne map (Ratio ~6.8) vs Nouvelle full size (Ratio ~2.1)
        // On se base sur le ratio plutot que la hauteur absolue pour eviter les bugs 
        // si on glisse une image deja petite ou a la mauvaise echelle.
        bool isFullSize = (imgRatio < 4.0f);
        float offsetWorldY = isFullSize ? 912.0f : 0.0f;
        mapSimOffsetY = offsetWorldY / scale;
        
        // Redimensionnement proportionnel parfait basé sur la largeur (simWidth)
        float targetHeight = simWidth / imgRatio;
        mapImg.resize(simWidth, targetHeight);
        mapImg.setImageType(OF_IMAGE_COLOR_ALPHA);
        mapPixels = mapImg.getPixels();
        originalPixels = mapPixels;
        bHasMap = true;

        // Optimisation : Mise en cache des dimensions pour éviter les appels dans isWall
        mapW = mapPixels.getWidth();
        mapH = mapPixels.getHeight();
        mapC = mapPixels.getNumChannels();
    } else {
        bHasMap = false;
        ofLogWarning("ColliderLayer") << currentMapPath << " introuvable !";
    }

    generateWalls();
}

//--------------------------------------------------------------
void ColliderLayer::reset() {
    if (bHasMap && originalPixels.isAllocated()) {
        mapPixels = originalPixels;
        generateWalls();
    }
}

//--------------------------------------------------------------
void ColliderLayer::generateWalls() {
    walls.clear();
    randomWalls.clear();
    wallMesh.clear();
    wallMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    if (bHasMap) {
        int w = mapW;
        int h = mapH;
        
        // Génération de rectangles horizontaux pour les pixels blancs
        for(int y=0; y<h; y++) {
            for(int x=0; x<w; x++) {
                ofColor c = mapPixels.getColor(x, y);
                if(c.getBrightness() > 128 && c.a > 128) {
                    int startX = x;
                    while(x < w) {
                        ofColor nextC = mapPixels.getColor(x, y);
                        if(nextC.getBrightness() > 128 && nextC.a > 128) x++;
                        else break;
                    }
                    walls.push_back(ofRectangle(startX, y - mapSimOffsetY, x - startX, 1));
                }
            }
        }
    }

    // Couleur des murs (Mauve comme dans ton ancien code Fish)
    ofColor wallColor(180, 100, 220, 127);
    ofColor whiteColor(255, 255, 255, 127);

    // 2. Ajout des 20 murs violets aléatoires
    bool addRndWall = false;
        if (addRndWall) {
        int numWalls = 20; 

        for(int i=0; i<numWalls; i++) {
            float w = ofRandom(30, 50);
            float h = ofRandom(3, 6);
            float x = ofRandom(0, simWidth - w);
            // On évite les extrêmes haut/bas pour ne pas coincer les entités
            float y = ofRandom(50, simHeight - 5);

            ofRectangle rect(x, y, w, h);
            walls.push_back(rect);
            randomWalls.push_back(rect);
        }
    }

        // 3. Construction du Mesh (Murs blancs de l'image + Murs violets)
    size_t numImageWalls = walls.size() - randomWalls.size();

    for(size_t i=0; i<walls.size(); i++) {
        ofRectangle& rect = walls[i];
        ofColor c = (i < numImageWalls) ? whiteColor : wallColor;
        
        float x = rect.x;
        float y = rect.y;
        float w = rect.width;
        float h = rect.height;

        float rx = x * scale;
        float ry = y * scale;
        float rw = w * scale;
        float rh = h * scale;

        // 2 Triangles par rectangle
        wallMesh.addVertex(ofVec3f(rx, ry, 0));
        wallMesh.addVertex(ofVec3f(rx + rw, ry, 0));
        wallMesh.addVertex(ofVec3f(rx, ry + rh, 0));

        wallMesh.addVertex(ofVec3f(rx + rw, ry, 0));
        wallMesh.addVertex(ofVec3f(rx + rw, ry + rh, 0));
        wallMesh.addVertex(ofVec3f(rx, ry + rh, 0));

       // for(int k=0; k<6; k++) wallMesh.addColor(wallColor);
        for(int k=0; k<6; k++) wallMesh.addColor(c);
    }
}

//--------------------------------------------------------------
// Vérifie si un point (x,y) en coordonnées SIMULATION est dans un mur
//--------------------------------------------------------------
bool ColliderLayer::isWall(float x, float y) {
    if (bHasMap) {
        int ix = (int)x;
        int iy = (int)(y + mapSimOffsetY);
        
        // Si on sort totalement de l'image (plafond global ou sol global)
        if (iy < 0 || iy >= mapH) return true;
        
        if (ix >= 0 && ix < mapW) {
            const unsigned char* data = mapPixels.getData();
            int index = (iy * mapW + ix) * mapC;
            if (mapC == 4) {
                if (data[index] > 128 && data[index + 3] > 128) return true;
            } else {
                if (data[index] > 128) return true;
            }
        }
    } else {
        // Comportement par défaut sans map (Limites strictes de la vue 1472)
        if (y < 0) return true;
        if (y >= simHeight) return true;
    }

    // Vérification des murs dynamiques (violets)
    for(auto& w : randomWalls) {
        if(w.inside(x, y)) return true;
    }
    return false;
}

//--------------------------------------------------------------
const vector<ofRectangle>& ColliderLayer::getWalls() const {
    return walls;
}

//--------------------------------------------------------------
void ColliderLayer::draw() {
    ofEnableAlphaBlending();
    wallMesh.draw();
    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void ColliderLayer::drawBrush(float x, float y, float radius, int colorType) {
    if (!bHasMap) {
        mapW = simWidth;
        mapH = simHeight;
        mapC = 4;
        mapPixels.allocate(mapW, mapH, OF_IMAGE_COLOR_ALPHA);
        mapPixels.setColor(ofColor(0, 0, 0, 0));
        originalPixels = mapPixels;
        mapSimOffsetY = 0;
        bHasMap = true;
    }
    int cx = (int)x;
    int cy = (int)(y + mapSimOffsetY);
    int r = (int)radius;
    bool modified = false;
    ofColor col = (colorType == 1) ? ofColor(255, 255, 255, 255) : ofColor(0, 0, 0, 0); // Blanc ou Transparent
    
    for (int iy = cy - r; iy <= cy + r; iy++) {
        for (int ix = cx - r; ix <= cx + r; ix++) {
            if (ix >= 0 && ix < mapW && iy >= 0 && iy < mapH) {
                if ((ix - cx) * (ix - cx) + (iy - cy) * (iy - cy) <= r * r) {
                    mapPixels.setColor(ix, iy, col);
                    modified = true;
                }
            }
        }
    }
    if (modified) {
        generateWalls();
    }
}