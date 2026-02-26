#pragma once
#include "ofMain.h"
#include <vector>
#include <algorithm>

// --- CLASSES UTILITAIRES ---

class DomeFunction {
public:
    float alpha, beta, gamma;
    DomeFunction(float radius, float height, float depth);
    virtual ~DomeFunction() {}
    virtual float basicInverse(float y) { return 0; }
    float domeRadius(float height);
};

class CircFunction : public DomeFunction {
public:
    CircFunction(float r, float h, float d);
    float basicInverse(float y) override;
};

class Oscillator {
public:
    static const float K;
    static const float R;
    float x = 0, y = 0, vx = 0, vy = 0, mass = 1, z = 1, spring = 1;

    void update();
    void addForce(float fx, float fy);
};

class Disk {
public:
    float x, y, r;
    int n;
    vector<float> _vertices;
    vector<float> _uvData;
    float dRad;
    float rotation = 0;

    Disk(float w, float h, float trimX, float trimY, float trimR, int _n);
    void initializeUV(float w, float h, float trimX, float trimY, float trimR);
    void updateVerticesLocal();
    void moveTo(float targetX, float targetY);
    void mergeVertices(vector<float>& target);
    void mergeUVData(vector<float>& target);
    void updateGlobalVertices(vector<float>& target, int startIndex);
};

// --- DOME ---

class Dome {
public:
    ofImage bitmapData;
    float x, y, r, h;
    int n;

    vector<Disk> disks;
    vector<int> startIndices;
    vector<Oscillator> oscillators;
    vector<float> vertices;
    
    // CORRECTION : ofIndexType pour compatibilité Mesh
    vector<ofIndexType> indices; 
    
    vector<float> uvtData;

    DomeFunction* func = nullptr;
    bool isMouseOn = false; 
    
    ofMesh mesh;

    Dome(ofImage img, float _x, float _y, float _r, float _h, int polygon, int diskNumber);
    ~Dome();

    void buildIndices();
    void update(float localMX, float localMY);
    void display();
};

// --- JELLY CREATURE ---

class JellyCreature {
public:
    float x, y, w, h;
    ofImage img;
    Dome* currentDome = nullptr;

    bool isDrawingCircle = false;
    float localCircleX, localCircleY, circleR;

    JellyCreature(float _x, float _y, float _w, float _h, ofImage _sourceImg);
    ~JellyCreature();

    bool isInside(float mx, float my);
    void onPress(float mx, float my);
    void onRelease(float mx, float my);
    
    void createDome(float lx, float ly, float lr);
   void update(float mx, float my);
   void draw(float mx, float my);
};