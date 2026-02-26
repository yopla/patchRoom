#pragma once
#include "ofMain.h"

class AtmosphereSystem {
public:
    void setup();
    void update();
    void draw(bool useTexture);
    void keyPressed(int key);
void loadTexture(string path);

    // Contrôles de visibilité
    bool bShowSphere = false;
    bool bShowDiscoBall = false;
    bool bShow360 = false; // <--- NOUVEAU

private:
    // Sphère d'environnement
    ofSpherePrimitive sphereEnvironnement;
    ofTexture textureDamier;
    ofMesh meshColoredSphere;
    
    // Texture 360
    ofImage texture360; // <--- NOUVEAU
    
    // Boule à facettes
    ofMesh meshDiscoBall;
    float rotX = 0; // Rotation horizontale
    float rotY = 0; // Rotation verticale
bool rot=true;
    // Fonctions de génération interne
    void setupCheckerboard(int size, int numChecks);
    void createColoredSphere();
    void createDiscoBall(int res);
};