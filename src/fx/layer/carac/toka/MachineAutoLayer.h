#pragma once
#include "ofMain.h"

struct AutoParticle {
    glm::vec2 pos, oldPos;
    glm::vec2 force;
    float mass = 1.0f;
    bool bFixed = false;
};

struct AutoConstraint {
    int p1, p2;
    float length;
    float stiffness = 1.0f;
    bool bVisible = true;
};

class MachineAutoLayer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();
    void mousePressed(float x, float y);

    void createCreature(float x, float y);
    
    // Physics
    void solveVerlet();
    void solveConstraints();
    void checkFloor();
    
    int addParticle(float x, float y, float mass=1.0f, bool fixed=false);
    void addConstraint(int i1, int i2, float stiffness=1.0f, bool visible=true);

    vector<AutoParticle> particles;
    vector<AutoConstraint> constraints;
    
    float sceneWidth;
    float groundY;
    
    // Motor
    int crankIdx;
    int thoraxIdx;
    int headIdx;
    float motorAngle;
    float motorSpeed;
    float direction; // 1.0 ou -1.0
    
    ofColor bodyColor;
};