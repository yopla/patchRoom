#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"

// --- ENEMY CLASS ---
class TeaaEnemy {
public:
    int gx, gy; // Grid coordinates
    ofVec2f pos; // Visual position
    ofColor color;
    int dir = 0; // 0:Right, 1:Down, 2:Left, 3:Up
    int targetGx = -1; // Target Grid X
    int targetGy = -1; // Target Grid Y
    vector<ofPoint> path;
    int stuckCount = 0;
    
    void setup(int x, int y, float cellSize) {
        gx = x; gy = y;
        pos.set(gx * cellSize, gy * cellSize);
        color.set(255, 0, 0); // Red like in AS3
        dir = (int)ofRandom(4);
        targetGx = -1; targetGy = -1;
        path.clear();
        stuckCount = 0;
    }
    
    void update(float cellSize) {
        // Smooth movement towards grid position
        ofVec2f target(gx * cellSize, gy * cellSize);
        pos += (target - pos) * 0.5f;
    }
    
    void draw(float cellSize) {
        ofSetColor(color);
        // Draw slightly smaller than cell for visibility
        ofDrawRectangle(pos.x + 4, pos.y + 4, cellSize - 8, cellSize - 8);
        
        // Eyes
        ofSetColor(255);
        if(dir==0) ofDrawRectangle(pos.x+cellSize-12, pos.y+8, 4, 4);
        else if(dir==1) ofDrawRectangle(pos.x+8, pos.y+cellSize-12, 4, 4);
        else if(dir==2) ofDrawRectangle(pos.x+8, pos.y+8, 4, 4);
        else if(dir==3) ofDrawRectangle(pos.x+8, pos.y+8, 4, 4);
    }
};

struct TeaaRoom {
    int x, y, w, h;
    vector<ofPoint> exits;
    bool contains(int px, int py) {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

struct TeaaPathNode {
    int px = -1, py = -1;
    float g = 0;
    int queryId = 0;
    bool closed = false;
    bool inOpenSet = false;
};

// --- LAYER CLASS ---
class TeaaLayer {
public:
    void setup(float w, float h, shared_ptr<ColliderLayer> collider);
    void update(float time);
    void draw();
    
    void generate(); // Regenerate dungeon (Enter key logic in AS3)
    void addAgent(float x, float y);
    void mousePressed(float x, float y, int button);
    
    int numEnemies = 50;
    shared_ptr<ColliderLayer> collider;

private:
    // Grid dimensions from AS3 ref (59x35)
    int gridW;
    int gridH;
    float cellSize = 20.0f; // Higher resolution
    
    // 0: Wall, 1: Corridor, 2: Room, 3: Collider
    vector<vector<int>> grid;
    vector<TeaaEnemy> enemies;
    vector<TeaaRoom> rooms;
    
    // Optimization
    ofMesh mapMesh;
    vector<TeaaPathNode> pathFinderNodes;
    int currentQueryId = 0;
    
    float simWidth, simHeight;
    
    // Turn system
    float lastTurnTime = 0;
    float turnInterval = 0.04f; // Speed of turns
    
    void createRoom(int x, int y, int w, int h);
    void createHCorridor(int x1, int x2, int y);
    void createVCorridor(int y1, int y2, int x);
    
    bool isWalkable(int x, int y);
    TeaaRoom* getRoomAt(int x, int y);
    vector<ofPoint> findPath(int sx, int sy, int ex, int ey);
};
