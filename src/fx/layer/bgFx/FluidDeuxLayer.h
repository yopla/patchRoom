#pragma once
#include "ofMain.h"
#include "LayerSystem.h"
#include "ColliderLayer.h"

struct FluidDeuxParticle {
    float x, y;
    float vx, vy;
    float fx, fy;
    float density;
    float densityNear;
    int gx, gy;
    int type;
    ofColor color;
};

struct FluidDeuxNeighbor {
    int p1Idx;
    int p2Idx;
    float distance;
    float nx, ny;
    float weight;
};

struct FluidDeuxGrid {
    vector<int> pIndices;
};

class FluidDeuxLayer : public BaseLayer {
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
    void calcForce();

    // Constants from AS3 Ref (Fluid 2.as)
    const float GRAVITY = 0.0f; // 0 in ref
    const float RANGE = 8.0f;
    const float RANGE2 = 64.0f; // 8*8
    const float DENSITY = 1.8f;
    const float PRESSURE = 1.0f;
    const float PRESSURE_NEAR = 1.0f;
    const float VISCOSITY = 0.1f;
    
    float simWidth, simHeight;
    float scale = 1.0f;
    float invGridSize;
    int numGridsX, numGridsY;

    vector<FluidDeuxParticle> particles;
    vector<FluidDeuxNeighbor> neighbors;
    int numNeighbors;
    
    vector<vector<FluidDeuxGrid>> grids;
    
    bool isPressed = false;
    ofVec2f mousePos;
    shared_ptr<ColliderLayer> collider;
};