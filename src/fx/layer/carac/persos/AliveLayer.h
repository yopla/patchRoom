#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

class AliveCreature {
public:
    void setup(float x, float y);
    void update(shared_ptr<ColliderLayer> collider, float simWidth, float simHeight);
    void draw(float scale);

    // Physique globale du corps
    ofVec2f pos;
    ofVec2f vel;
    float angle;
    float angularVel;
    
    // Paramètres moteurs similaires à alive.as
    float motorAngles[3];
    float angleLimits[3];
    float motorSpeeds[3];
    
    // Forme calculée (4 rectangles)
    ofVec2f localNodes[4];
    ofVec2f worldNodes[4];
    float worldAngles[4];
    
    ofColor color;
    
    bool bIsDragged = false;
    ofVec2f dragTarget;
};

class AliveLayer {
public:
    void setup(float w, float h, float s, shared_ptr<ColliderLayer> col);
    void update(float mx, float my, float time);
    void draw();
    void addCreature(float x, float y);
    
    void mousePressed(float x, float y, int button);
    void mouseDragged(float x, float y, int button);
    void mouseReleased(float x, float y, int button);

    float simWidth;
    float simHeight;
    float scale;
    shared_ptr<ColliderLayer> collider;
    vector<shared_ptr<AliveCreature>> creatures;
    shared_ptr<AliveCreature> draggedCreature = nullptr;
};