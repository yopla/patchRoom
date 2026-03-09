#pragma once
#include "ofMain.h"
#include "LayerSystem.h"
#include "ColliderLayer.h"

struct FluidTroisParticle {
    float x, y;
    float vx, vy;
    float fx, fy;
    float density;
    float pressure;
    int gx, gy;
    int type;
    ofColor color;
};

struct FluidTroisNeighbor {
    int p1Idx;
    int p2Idx;
    float distance;
    float nx, ny;
    float weight;
    float ax, ay; // For adhesion force
};

struct FluidTroisGrid {
    vector<int> pIndices;
};

class FluidTroisLayer : public BaseLayer {
public:
    void setup(float w, float h, float scale);
    void setCollider(shared_ptr<ColliderLayer> c);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    void mousePressed(float x, float y, int button) override;
    void mouseReleased(float x, float y, int button) override;

private:
    void pour();
    void move();
    void updateGrids();
    void findNeighbors();
    void findNeighborsInGrid(int piIdx, int gx, int gy);
    void setPressure();
    void calcForce();
    void addParticles(int count);

    // Constants from AS3 Ref (Fluid3.as -> class Fluid2)
    const float GRAVITY = 0.0f;
    const float RANGE = 10.0f;
    const float RANGE2 = 100.0f; // 10*10
    const float DENSITY = 0.2f;
    const float PRESSURE = 2.0f;
    const float VISCOSITY = 0.05f;
    const float ADHESION = 0.8f;
    
    float simWidth, simHeight;
    float scale = 1.0f;
    float invGridSize;
    int numGridsX, numGridsY;

    vector<FluidTroisParticle> particles;
    vector<FluidTroisNeighbor> neighbors;
    int numNeighbors;
    
    vector<vector<FluidTroisGrid>> grids;
    
    bool isPressed = false;
    ofVec2f mousePos;
    shared_ptr<ColliderLayer> collider;
    
    ofFbo fluidFbo;
    ofFbo particleFbo;
};