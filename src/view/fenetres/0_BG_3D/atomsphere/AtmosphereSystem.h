#pragma once
#include "ofMain.h"

class AtmosphereSystem {
public:
    void setup();
    void update(float time);
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
    ofVideoPlayer video360;
    bool bIsVideo = false;
    
    // Boule à facettes
    ofMesh meshDiscoBall;
    float rotX = 0; // Rotation horizontale
    float rotY = 0; // Rotation verticale
    float rotZ = 0; // Rotation profondeur (Roll)
    float offsetY = 0; // Position verticale
    bool rot=false;
    float autoRotY = 0.0f;
    // Fonctions de génération interne
    void setupCheckerboard(int size, int numChecks);
    void createColoredSphere();
    void createDiscoBall(int res);
};