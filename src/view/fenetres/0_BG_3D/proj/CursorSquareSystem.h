#pragma once
#include "ofMain.h"
#include "RoomWalls.h"

// On détaille les types pour savoir quel côté est touché
enum CursorHitType {
    HIT_NONE,
    HIT_FLOOR,
    HIT_WALL_FRONT,
    HIT_WALL_BACK,
    HIT_WALL_COUR,  // Côté Droit
    HIT_ROOF_COUR,  // Toit Droit
    HIT_WALL_JAR,   // Côté Gauche
    HIT_ROOF_JAR    // Toit Gauche
};

class CursorSquareSystem {
public:
    void setup();
    void updateRaycast(ofCamera& cam, RoomWalls& walls);
    void drawProjected(RoomWalls& walls);
    void drawReflet(ofMesh& meshTarget, ofVec3f refletNormal, ofVec3f offsetDir, float distToCorner);
    bool bDrawReflections = false; // False par défaut = Strictement le mur survolé

    float squareSize = 500.0f;
    bool isVisible = false;

private:
    ofVec3f currentPos;
    ofVec3f currentNormal;
    CursorHitType hitType = HIT_NONE;
    
    ofCamera projector;
    ofImage cursorImg;
};