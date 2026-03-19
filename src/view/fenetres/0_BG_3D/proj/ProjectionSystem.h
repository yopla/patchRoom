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
    
    ofVec3f getPlanCollePosition() const { return planColle.getPosition(); }
    void setPlanColleExternalHover(bool hovered) { planColle.setExternalHover(hovered); }

    // --- Accès pour Visualizer ---
    bool getShowBeams() const { return beamAlphaMode != 3; }
    int getBeamAlphaMode() const { return beamAlphaMode; }
    bool getShowPlanColle() const { return bShowPlanColle; }

    void movePlanColle(float dAngle, float dElevation) {
        planColle.addAngle(dAngle);
        planColle.addElevation(dElevation);
    }

private:
    PlanColleSystem planColle;
    BeamSystem beam;
    BeamSystem beam2;
    BeamSystem beam3;

    int beamAlphaMode = 3; // 0: 100%, 1: 75%, 2: 33%, 3: 0% (OFF par défaut)
    bool bShowPlanColle = true;
};