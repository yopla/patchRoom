#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"
#include "ofxOpenCv.h"
#include "PhysicSamBase.h"

class PhysicSamLayer {
public:
    void setup(float w, float h, float s, shared_ptr<ColliderLayer> col);
    void update(float mx, float my, float time);
    void draw();
    void mousePressed(float x, float y, int button);
    void mouseDragged(float x, float y, int button);
    void mouseReleased(float x, float y, int button);

    void addBody(const ofImage& mask, const ofImage& sourceImg, float offsetY);
    void addGear(const ofImage& mask, const ofImage& sourceImg, float offsetY);
    void addSoftBody(const ofImage& mask, const ofImage& sourceImg, float offsetY);
    void addAliveBody(const ofImage& mask, const ofImage& sourceImg, float offsetY);
    void clear();

    float simWidth, simHeight, scale;
    shared_ptr<ColliderLayer> collider;
    
    // Paramètres de l'ombre du trou
    float holeShadowAngle = 45.0f; // 0 à 360 degrés
    float holeShadowDistance = 15.0f; // Taille / éloignement de l'ombre
    
    vector<shared_ptr<PhysicSamBase>> bodies;
    shared_ptr<PhysicSamBase> draggedBody = nullptr;
    ofVec2f dragOffset;
    
    bool bActive = false;
private:
    void createBody(const ofImage& mask, const ofImage& sourceImg, float offsetY, int bodyType);
};