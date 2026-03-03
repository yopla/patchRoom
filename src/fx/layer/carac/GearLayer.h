#pragma once
#include "ofMain.h"

struct Gear {
    ofVec2f pos;
    float radius;
    float angle;        // Rotation actuelle
    float angularVel;   // Vitesse de rotation
    int teethCount;
    ofColor color;
    
    // Pour la simulation de connexion
    vector<int> connectedIndices; 
};

struct FallingSquare {
    ofVec2f pos;
    ofVec2f vel;
    float size;
    float angle;
    float rotSpeed;
    bool bDead;
};

class GearLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time);
    void draw();

    void addSquare(); // Fait tomber un carré manuellement ou auto

    float simWidth;
    float simHeight;
    
    vector<Gear> gears;
    vector<FallingSquare> squares;
    
private:
    void solveGearConstraints();
    void drawGear(const Gear& g);
};
