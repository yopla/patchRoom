#pragma once
#include "ofMain.h"

class PosterSystem {
public:
    void setup(float roomWidth, float roomDepth, float heightFrontBack);
    void update();
    void draw(float roomWidth, float roomDepth);

private:
    void drawPosterSegmentOnWall(int wallIndex, float xOnWall, float texX, float drawW, float roomWidth, float roomDepth);

    ofImage posterImg;
    
    // Position et dimensions
    float posterGlobalX; 
    float posterY;       
    const float posterW = 600.0f;
    const float posterH = 450.0f;
    float posterSpeed = 15.0f;
    float totalPerimeter;
    float maxWallHeight; // Pour le clamp vertical
};