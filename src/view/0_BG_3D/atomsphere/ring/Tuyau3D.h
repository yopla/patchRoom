#pragma once
#include "ofMain.h"
#include <functional>

class TuyauObj {
public:
    ofVec3f pos;
    ofMatrix4x4 rot;
    bool bIsJunction;
};

struct TransformState {
    ofVec3f P;
    ofMatrix4x4 M;
    float phase;
};

class Tuyau3D {
public:
    void setup();
    void update(float time);
    void draw();

    // Paramètres demandés
    int hueCycles = 4;
    int numRings = 10;
    
    // Le rayon du "trou" du tuyau (taille de la section)
    // Rendu grand pour englober la salle (2000 est > à la salle de 2400/2 puisque rayon > diamètre/2)
    float tubeRadius = 2000.0f; 

    // Propriétés des TuyauObj
    bool bDrawTuyauObj = true;
    int numExtraTuyauObjs = 25;
    
    bool bDrawArcs = true;
    void setDrawArcs(bool draw);
    bool bDrawCouture = false;

    // Contrôles de l'animation
    bool bIsPlaying = false;
    float phase = 0.0f;
    float speed = 1.0f; // Multiplicateur de vitesse
    void go();
    void pause();
    void reset();
    
    // Angles de rotation fixés par sliders
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    
    int currentJunction = -1;
    int targetJunction = 0;
    int currentPath = 0; // 0 = Main, 1 = Arc 1, 2 = Arc 2
    float currentT = 0.0f;
    bool bPauseAtJunction = true;
    int selectedPathOption = 0;
    
    // Points d'étape aléatoires (Waypoints)
    float wpMainT = -1.0f;
    int wpMainTargetJunc = -1;
    float wpArcT = -1.0f;
    int wpArcPath = -1;
    bool bAtWaypoint = false;
    
    void nextCarrefour();
    vector<string> getAvailablePaths();
    void selectPath(int index);
    
    ofVec3f currentP;
    ofMatrix4x4 currentMatrix;
    int state = 2; // 0=IDLE, 1=TRANS_START, 2=MOVING, 3=TRANS_END
    float transProgress = 0.0f;
    TransformState startTransState;
    TransformState endTransState;
    
    ofShader clipShader;
    bool bShaderLoaded = false;

    // Gestion des boites
    vector<TuyauObj> tuyauObjs;
    ofBoxPrimitive objBox;
    ofTexture texObjBW;
    ofTexture texObjColor;
    void generateTuyauObjs();
    void generateObjTextures();

private:
    ofVboMesh mesh;
    ofFbo textureFbo;
    
    void buildMesh();
    void generateTexture();
    ofVec3f getPathPoint(float t);
    ofVec3f getArcPoint(int arcIdx, float s);
    float getPhase(int path, int targetJunc, float t);
    TransformState evaluateTransform(int path, int targetJunc, float t_path);
};