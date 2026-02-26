#pragma once
#include "ofMain.h"

struct FluidCell {
    float vx = 0;
    float vy = 0;
    float r = 0, g = 0, b = 0, a = 0;
    float pressure = 0;       
};

class FluidsCreature {
public:
    FluidsCreature(float _x, float _y, float _w, float _h, ofImage& _sharedSource);
    ~FluidsCreature();

    void update(float mx, float my);
    void draw();
    
    bool isInside(float mx, float my);
    void checkInput(float mx, float my);

    float x, y, w, h;
    
private:
    int gridWidth, gridHeight;
    float scale;
    
    vector<FluidCell> cells;
    vector<FluidCell> prevCells;
    
    // --- NOUVEAU : Mémoire des couleurs initiales ---
    vector<ofColor> initialColors; 

    ofImage fluidImage;
    
    float prevLocalX, prevLocalY;
    
    void advect();
    void project();
    
    // Nouvelle méthode pour régénérer les couleurs
    void regenerateColors(); 
    
    void colorFromImage(ofImage& img);
    
    int getIndex(int i, int j) {
        return i + j * (gridWidth + 2);
    }
};