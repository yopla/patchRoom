#pragma once
#include "ofMain.h"

class GolBox {
public:
    void setup(float w, float h, float d);
    void reset();
    void update();
    void draw();

    // Définition : La taille interne des "cases" (cellules) du jeu de la vie.
    // Plus c'est grand, plus les pixels seront gros (Pixel Art).
    int cellSize = 1; 

    ofBoxPrimitive box;
    ofTexture tex;
    ofPixels pixelsFront;
    ofPixels pixelsBack;
    
    int gridW, gridH;
    int frameCount = 0;

    long currentSeed = 0;

    ofVec2f getNextManhattan(int x, int y, int dx, int dy);
    ofVec2f getNext(int x, int y, int dx, int dy);
};