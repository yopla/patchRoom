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
void ColliderLayer::generateWalls() {
    walls.clear();
    randomWalls.clear();
    wallMesh.clear();
    wallMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // 1. Chargement de COLL.png et génération des colliders blancs
    ofImage mapImg;
    if(mapImg.load("GAB0/COLL.png")) {
        mapImg.resize(simWidth, simHeight);
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
                    walls.push_back(ofRectangle(startX, y, x - startX, 1));
                }
            }
        }
    } else {
        bHasMap = false;
        ofLogWarning("ColliderLayer") << "COLL.png introuvable !";
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
    // 1. Vérifier les limites du monde (Sol et Plafond)
    if (y < 0) return true;            // Plafond
    if (y >= simHeight) return true;   // Sol

    // 2. Vérifier les obstacles
    // Optimisation : Vérification directe des pixels pour l'image
    if (bHasMap) {
        int ix = (int)x;
        int iy = (int)y;
        // Accès direct au pointeur brut pour éviter le coût de getColor()
        if (ix >= 0 && ix < mapW && iy >= 0 && iy < mapH) {
            const unsigned char* data = mapPixels.getData();
            int index = (iy * mapW + ix) * mapC;
            // On suppose que si c'est blanc/clair, c'est un mur (on check le canal R ou la brillance)
            if (data[index] > 128) return true;
        }
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