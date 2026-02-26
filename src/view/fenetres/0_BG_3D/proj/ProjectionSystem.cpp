#include "ProjectionSystem.h"

//--------------------------------------------------------------
void ProjectionSystem::setup() {
    planColle.setup();
    beam.setup();
    beam2.setup();
    beam3.setup("theatreoptique.jpg", 600, 300);
}

//--------------------------------------------------------------
void ProjectionSystem::update() {
    planColle.update();
    beam.update();
    beam2.update();
    beam3.update();
}

//--------------------------------------------------------------
void ProjectionSystem::drawPlanColle() {
    planColle.draw();
}

//--------------------------------------------------------------
void ProjectionSystem::drawBeamProjection(RoomWalls& walls, bool showRoof) {
    beam.drawProjection(walls, showRoof, -10.0f);  // Offset augmenté pour éviter le Z-fighting
    beam2.drawProjection(walls, showRoof, -20.0f); // Offset plus fort pour le 2ème
    beam3.drawProjection(walls, showRoof, -30.0f); // Offset encore plus fort pour le 3ème
}

//--------------------------------------------------------------
void ProjectionSystem::drawProjectorDebug(RoomWalls& walls) {
    beam.drawDebug(walls);
    beam2.drawDebug(walls);
    beam3.drawDebug(walls);
}

//--------------------------------------------------------------
void ProjectionSystem::checkMouseIntersection(const ofCamera& cam) {
    planColle.checkMouseIntersection(cam);
}

//--------------------------------------------------------------
void ProjectionSystem::updateTarget(const ofCamera& viewCam, RoomWalls& walls) {
    beam.updateTarget(viewCam, walls);
}

//--------------------------------------------------------------
void ProjectionSystem::updateTarget2(const ofCamera& viewCam, RoomWalls& walls) {
    beam2.updateTarget(viewCam, walls);
}

//--------------------------------------------------------------
void ProjectionSystem::updateTarget3(const ofCamera& viewCam, RoomWalls& walls) {
    beam3.updateTarget(viewCam, walls);
}

//--------------------------------------------------------------
void ProjectionSystem::keyPressed(int key) {
    planColle.keyPressed(key);
    beam.keyPressed(key);
    beam2.keyPressed(key);
    beam3.keyPressed(key);
}

//--------------------------------------------------------------
bool ProjectionSystem::isPlanColleHovered() const {
    return planColle.isHovering();
}

float ProjectionSystem::getPlanColleRadius() const {
    return planColle.getRadius();
}

float ProjectionSystem::getPlanColleElevation() const {
    return planColle.getElevation();
}

float ProjectionSystem::getPlanColleAzimuth() const {
    return planColle.getAzimuth();
}