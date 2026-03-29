#pragma once
#include "ofMain.h"
#include "PhysicSamBase.h"
#include "ColliderLayer.h"
#include "SamUtils.h"

class AliveSam : public PhysicSamBase {
public:
    void setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) override;
    void updateInteraction() override;
    void draw() override;
    void wrap(float shiftX) override;

private:
    int numBones;

    // Motor control for animation, similar to AliveCreature
    vector<float> motorAngles;
    vector<float> angleLimits;
    vector<float> motorSpeeds;

    // Calculated shape (chain of segments)
    vector<ofVec2f> localNodes;
    vector<ofVec2f> worldNodes;
    vector<float> worldAngles;

private:
    // --- Softbody members from SoftbodySam ---
    float tessellationFineness = 10.0f;
    float rigidity = 0.05f;
    float edgeStiffness = 0.8f;
    float damping = 0.97f;
    
    ofVec2f lastRigidPos;
    float lastRigidAngle = 0.0f;

    // Particle system for deformation
    vector<glm::vec3> origVerts;
    vector<glm::vec3> currentVerts;
    vector<glm::vec3> lastVerts;
    
    vector<SamSpring> springs;
    vector<int> nodeVertexIndices;

public:
    shared_ptr<ColliderLayer> collider;
};
