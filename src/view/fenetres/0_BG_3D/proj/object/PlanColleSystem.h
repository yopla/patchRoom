#pragma once

#include "ofMain.h"

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    float size;
    ofColor color;
};

class PlanColleSystem {
public:
    void setup();
    void update();
    void draw();
    void checkMouseIntersection(const ofCamera& cam);
    void keyPressed(int key);

    void setExternalHover(bool hover) { isExternalHover = hover; }
    ofVec3f getPosition() const { return planColleNode.getGlobalPosition(); }
    bool isHovering() const { return isMouseOverPlan || isExternalHover; }
    float getRadius() const { return radius; }
    float getElevation() const { return elevation; }
    float getAzimuth() const { return azimuth; }

    void addAngle(float a) { planAngle += a; }
    void addElevation(float e) { planElevation += e; }

private:
    void updateParticles();
    void updatePosition();

    ofFbo planContentFbo;
    vector<Particle> particles;
    ofNode planColleNode;

    float planAngle = 0.0f;
    float planElevation = 0.0f;
    bool bSolidBackground = false;

    bool isMouseOverPlan = false;
    bool isExternalHover = false;

    float radius = 0.0f;
    float elevation = 0.0f;
    float azimuth = 0.0f;
};