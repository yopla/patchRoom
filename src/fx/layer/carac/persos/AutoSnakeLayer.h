#pragma once
#include "ofMain.h"
#include "ColliderLayer.h"
#include <vector>
#include <list>

class AutoSnakeLayer {
public:
    void setup(float w, float h, shared_ptr<ColliderLayer> col);
    void update(float time);
    void draw();

private:
    struct Cell {
        int x, y;
        bool operator==(const Cell& o) const { return x == o.x && y == o.y; }
    };

    void spawnFood();
    bool findPathBFS(Cell start, Cell target, std::list<Cell>& outPath);
    void moveSnake();
    void drawWrapped(float offsetX);

    float simWidth, simHeight;
    shared_ptr<ColliderLayer> collider;

    std::vector<Cell> snake;
    Cell food;
    std::list<Cell> currentPath;

    int gridW, gridH;
    float cellSize;
    
    float lastMoveTime;
    float moveInterval;
};