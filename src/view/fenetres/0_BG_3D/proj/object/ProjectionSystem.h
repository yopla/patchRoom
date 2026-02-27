#pragma once

#include "ofMain.h"
#include "RoomWalls.h"
#include "PlanColleSystem.h"
#include "BeamSystem.h"

class ProjectionSystem {
public:
    void setup();
    void update();
    
    void drawPlanColle();
    void drawBeamProjection(RoomWalls& walls, bool showRoof);
    void drawProjectorDebug(RoomWalls& walls);
    
    void checkMouseIntersection(const ofCamera& cam);
    void updateTarget(const ofCamera& viewCam, RoomWalls& walls);
    void updateTarget2(const ofCamera& viewCam, RoomWalls& walls);
    void updateTarget3(const ofCamera& viewCam, RoomWalls& walls);
    
    void keyPressed(int key);

    // --- Accès pour OSC ---
    bool isPlanColleHovered() const;
    float getPlanColleRadius() const;
    float getPlanColleElevation() const;
    float getPlanColleAzimuth() const;
private:
    PlanColleSystem planColle;
    BeamSystem beam;
    BeamSystem beam2;
    BeamSystem beam3;
};