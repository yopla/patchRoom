#pragma once
#include "ofMain.h"
#include <vector>
#include <memory>
#include <cmath>

struct GolMCell {
    float x, y, size, width;
    float xTo, yTo, sTo;
    bool vanishing, vanished;
    
    GolMCell(float _x, float _y, float _w) : x(_x), y(_y), size(0), width(_w), xTo(_x), yTo(_y), sTo(_w), vanishing(false), vanished(false) {}
    
    void move(float nx, float ny) { 
        // Empêche la particule de traverser l'écran visuellement si elle boucle (Wrap)
        if (std::abs(nx - x) > 1.5f) x = nx; 
        if (std::abs(ny - y) > 1.5f) y = ny; 
        xTo = nx; yTo = ny; 
    }
    void del() { vanishing = true; sTo = 0; }
    void step() {
        x += (xTo - x) / 3.0f;
        y += (yTo - y) / 3.0f;
        size += (sTo - size) / 4.0f;
        if(vanishing && size <= 1.0f) vanished = true;
    }
    void draw() {
        float drawX = x * width + 1 + (width - size) / 2.0f;
        float drawY = y * width + 1 + (width - size) / 2.0f;
        ofDrawRectRounded(drawX, drawY, std::max(0.0f, size - 2.0f), std::max(0.0f, size - 2.0f), 3);
    }
};

class GolBoxMotion {
public:
    void setup(float w, float h, float d);
    void reset();
    void update();
    void draw();

    bool dfs(const std::vector<std::vector<int>>& adj, int u, std::vector<int>& match, std::vector<bool>& used);

    ofVec2f getNextManhattan(int x, int y, int dx, int dy);
    ofVec2f getNext(int x, int y, int dx, int dy);

    ofBoxPrimitive box;
    ofFbo fbo;
    
    int gridW = 80;
    int gridH = 60;
    float cellW = 20.0f;
    int period = 10;
    int tim = 0;

    std::vector<std::vector<std::shared_ptr<GolMCell>>> table;
    std::vector<std::shared_ptr<GolMCell>> allCells;
    
    bool bPaused = false;
    long currentSeed = 0;
};