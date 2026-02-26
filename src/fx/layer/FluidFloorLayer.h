#pragma once
#include "ofMain.h"

struct FloorFluidCell {
    float vx = 0;
    float vy = 0;
    float r = 0, g = 0, b = 0, a = 0;
};

class FluidFloorLayer {
public:
    void setup(float w, float h, int resX = 256, int resY = 64);
    void update(float mx, float my);
    void draw(float x, float y);
        void addForce(float localX, float localY, float forceX, float forceY);

    // Pour régénérer le motif si besoin
    void resetPattern();
    
    // Toggle entre Checker et Image
    void toggleBackground();

private:
    float width, height;
    int gridWidth, gridHeight;
    float scaleX, scaleY;
    
    vector<FloorFluidCell> cells;
    vector<FloorFluidCell> prevCells;
    vector<ofColor> initialColors; 

    ofImage fluidImage;
    
    // Image de fond
    ofImage bgImage;
    bool bUseImage = false;
    
    float prevLocalX, prevLocalY;
    
    void advect();
    void project();
    void regenerateColors(); 
    void colorFromPattern();
    void colorFromImage();
    
    void checkInput(float mx, float my);
    
    // Helper pour l'index avec Wrapping horizontal
    int getIndex(int i, int j) {
        // Wrapping X
        if (i < 1) i = gridWidth;
        if (i > gridWidth) i = 1;
        // Clamping Y
        if (j < 1) j = 1;
        if (j > gridHeight) j = gridHeight;
        
        return i + j * (gridWidth + 2);
    }
};
