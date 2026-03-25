#pragma once
#include "ofMain.h"

class EatMapLayer {
public:
    void setup(float simulationWidth, float simulationHeight, float displayScale);
    void draw();
    
    bool isWall(float x, float y);
    void explode(float x, float y, float radius);
    void loadMap(string path);
    void reset();
    
    float simWidth;
    float simHeight;
    float scale;
    int mapW, mapH, mapC;
    float mapSimOffsetY = 0.0f;
    
    string currentMapPath = "";
    ofPixels mapPixels;
    ofImage mapImage;
    bool bHasMap = false;
    ofPixels originalPixels;
};