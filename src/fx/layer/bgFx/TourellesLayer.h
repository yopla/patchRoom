#pragma once
#include "ofMain.h"

struct TourelleBullet {
    ofVec2f pos;
    ofVec2f vel;
    float radius;
    bool active;
};

struct TourelleMeteor {
    ofVec2f pos;
    ofVec2f vel;
    float radius;
    float rot;
    float omega;
    bool active;
    bool locked; // Pour éviter que 10 tourelles tirent sur le même
};

struct TourelleExplosion {
    ofVec2f pos;
    int t;
};

struct GroundImpact {
    ofVec2f pos;
    int t;
    float maxT;
    vector<ofVec2f> sparksVel;
};

struct TourelleBattery {
    ofVec2f pos;
    float theta;
    float vBullet;
    float omega;
    int algoType; // 1: FastestDestruction, 2: FastestShoot
};

class TourellesLayer {
public:
    void setup(float w, float h);
    void update(float time);
    void draw();

    float simWidth, simHeight;
    float G = 0.4f; // Gravité des balles et météores (mise à l'échelle)

private:
    vector<TourelleBattery> batteries;
    vector<TourelleMeteor> meteors;
    vector<TourelleBullet> bullets;
    vector<TourelleExplosion> explosions;
    vector<GroundImpact> impacts;
    
    int frameCount = 0;
    
    float getShortestDist(float current, float target, float w);
    void algoFastestDestruction(TourelleBattery& bat);
    void algoFastestShoot(TourelleBattery& bat);
    void shoot(const TourelleBattery& bat);
};