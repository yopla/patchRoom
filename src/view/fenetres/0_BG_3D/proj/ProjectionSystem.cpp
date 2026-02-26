#include "ProjectionSystem.h"

//--------------------------------------------------------------
void ProjectionSystem::setup() {
    planColle.setup();
    beam.setup();
}

//--------------------------------------------------------------
void ProjectionSystem::update() {
    planColle.update();
    beam.update();
}

//--------------------------------------------------------------
void ProjectionSystem::drawPlanColle() {
    planColle.draw();
}

//--------------------------------------------------------------
void ProjectionSystem::drawBeamProjection(RoomWalls& walls, bool showRoof) {
    beam.drawProjection(walls, showRoof);
}

//--------------------------------------------------------------
void ProjectionSystem::drawProjectorDebug(RoomWalls& walls) {
    beam.drawDebug(walls);
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
void ProjectionSystem::keyPressed(int key) {
    planColle.keyPressed(key);
    beam.keyPressed(key);
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