#pragma once
#include "ofMain.h"
#include "RoomWalls.h"

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    float size;
    ofColor color;
};

class ProjectionSystem {
public:
    void setup();
    void update();
    
    void drawPlanColle();
    void drawBeamProjection(RoomWalls& walls, bool showRoof);
    void drawProjectorDebug(RoomWalls& walls);

    void checkMouseIntersection(const ofCamera& cam);
    void keyPressed(int key);
    void updateTarget(const ofCamera& viewCam, RoomWalls& walls);

    ofNode planColleNode;
    float planAngle = 180.0f;
    float planElevation = 0.0f;
   
    bool isMouseOverPlan = false;
    float radius;
    float elevation;
    float azimuth;

    ofCamera projector;
    float targetBeamWidth = 500.0f;
    float targetBeamHeight = 500.0f;
    float currentDist;

private:
    ofImage imgBeam;
    
    // Système de particules
    ofFbo planContentFbo;      
    vector<Particle> particles;
    void updateParticles();
    
    // --- NOUVEAU : Contrôle du fond ---
    bool bSolidBackground = false; // False = 50%, True = 100%

    ofVec3f currentSmoothedNormal; 
    ofVec3f currentSmoothedPos;    
    ofVec3f lastHitPoint;          
    
    void updatePlanCollePosition();
};