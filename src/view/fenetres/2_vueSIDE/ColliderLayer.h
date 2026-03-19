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
    int mapW, mapH, mapC;
    float mapSimOffsetY = 0.0f; // Offset Y pour la lecture du collider grand format
    
    // Gestion des murs procéduraux (TeaaLayer)
    void setCustomWalls(const vector<ofRectangle>& newWalls);
    void clearCustomWalls();

    void loadMap(string path);
    string currentMapPath = "GAB0/COLL.png";

private:
    void generateWalls();

    vector<ofRectangle> walls;
    vector<ofRectangle> randomWalls; // Sous-ensemble pour optimisation isWall
    ofPixels mapPixels;              // Pour optimisation isWall
    bool bHasMap = false;
    ofMesh wallMesh;
    vector<ofRectangle> customWalls;

};
