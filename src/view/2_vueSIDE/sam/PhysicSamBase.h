#pragma once
#include "ofMain.h"

class PhysicSamBase {
public:
    virtual ~PhysicSamBase() = default;
    virtual void setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY);
    virtual void draw();
    virtual void applyForce(const ofVec2f& force, const ofVec2f& point);
    virtual void updateInteraction() = 0; // Géré par les sous-classes
    virtual void drawHole(float shadowAngle, float shadowDist);
    virtual void wrap(float shiftX);

    ofPolyline shape; // relative to pos
    ofVboMesh mesh;
    ofVboMesh holeMesh; // Mesh pour dessiner le trou d'origine
    ofImage texture;

    ofVec2f pos;
    ofVec2f origPos; // Position de création
    ofVec2f vel;
    float angle = 0;
    float angularVel = 0;
    
    ofVec2f forceAccum;
    float torqueAccum = 0;

    float mass;
    float invMass;
    float inertia;
    float invInertia;

    vector<ofVec2f> localNodes;
    float nodeRadius = 2.0f;
    float maxRadius = 0.0f;

    bool bIsDragged = false;
    ofVec2f dragPointLocal;
    ofVec2f dragTarget;
};