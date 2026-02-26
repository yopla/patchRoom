#pragma once
#include "ofMain.h"

class ColliderLayer {
public:
    void setup(float simulationWidth, float simulationHeight, float displayScale);
    void draw();
    
    // Méthode rapide pour savoir si une position touche un mur (pour les Sauteurs)
    bool isWall(float x, float y);
    
    // Accès direct aux rectangles (pour la répulsion des poissons)
    const vector<ofRectangle>& getWalls() const;
    
    // Dimensions de la simulation
    float simWidth;
    float simHeight;
    float scale;

private:
    void generateWalls();

    vector<ofRectangle> walls;
    ofMesh wallMesh;
};
