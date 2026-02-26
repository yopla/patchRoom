#include "ColliderLayer.h"

//--------------------------------------------------------------
void ColliderLayer::setup(float simulationWidth, float simulationHeight, float displayScale) {
    simWidth = simulationWidth;
    simHeight = simulationHeight;
    scale = displayScale;

    generateWalls();
}

//--------------------------------------------------------------
void ColliderLayer::generateWalls() {
    walls.clear();
    wallMesh.clear();
    wallMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // Couleur des murs (Mauve comme dans ton ancien code Fish)
    ofColor wallColor(180, 100, 220, 200);

    // On combine la logique : assez de murs pour les sauteurs, mais assez espacés pour les poissons
    int numWalls = 75; 

    for(int i=0; i<numWalls; i++) {
        float w = ofRandom(30, 50);
        float h = ofRandom(3, 6);
        float x = ofRandom(0, simWidth - w);
        // On évite les extrêmes haut/bas pour ne pas coincer les entités
        float y = ofRandom(50, simHeight - 5);

        ofRectangle rect(x, y, w, h);
        walls.push_back(rect);

        // Construction du Mesh pour le dessin (coordonnées REELLES = sim * scale)
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

        for(int k=0; k<6; k++) wallMesh.addColor(wallColor);
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
    // Pour 60 murs, une boucle simple est très rapide.
    for(auto& w : walls) {
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