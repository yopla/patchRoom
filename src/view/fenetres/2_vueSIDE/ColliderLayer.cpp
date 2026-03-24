#include "ColliderLayer.h"

//--------------------------------------------------------------
void ColliderLayer::setup(float simulationWidth, float simulationHeight, float displayScale) {
       ofSetRandomSeed(42);
    simWidth = simulationWidth;
    simHeight = simulationHeight;
    scale = displayScale;

    generateWalls();
}

//--------------------------------------------------------------
void ColliderLayer::loadMap(string path) {
    currentMapPath = path;
    generateWalls();
}

//--------------------------------------------------------------
void ColliderLayer::generateWalls() {
    walls.clear();
    randomWalls.clear();
    wallMesh.clear();
    wallMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // 1. Chargement de l'image et génération des colliders blancs
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
        mapPixels = mapImg.getPixels();
        bHasMap = true;

        // Optimisation : Mise en cache des dimensions pour éviter les appels dans isWall
        mapW = mapPixels.getWidth();
        mapH = mapPixels.getHeight();
        mapC = mapPixels.getNumChannels();
        
        int w = mapW;
        int h = mapH;
        
        // Génération de rectangles horizontaux pour les pixels blancs
        for(int y=0; y<h; y++) {
            for(int x=0; x<w; x++) {
                if(mapPixels.getColor(x,y).getBrightness() > 128) {
                    int startX = x;
                    while(x < w && mapPixels.getColor(x,y).getBrightness() > 128) x++;
                    walls.push_back(ofRectangle(startX, y - mapSimOffsetY, x - startX, 1));
                }
            }
        }
    } else {
        bHasMap = false;
        ofLogWarning("ColliderLayer") << currentMapPath << " introuvable !";
    }

    // Couleur des murs (Mauve comme dans ton ancien code Fish)
    ofColor wallColor(180, 100, 220, 127);
    ofColor whiteColor(255, 255, 255, 127);

    // 2. Ajout des 20 murs violets aléatoires
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
            if (data[index] > 128) return true;
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