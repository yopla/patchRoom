#include "AutoSnakeLayer.h"
#include <queue>
#include <map>

void AutoSnakeLayer::setup(float w, float h, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    collider = col;

    cellSize = 40.0f; // Taille de la case
    gridW = ceil(simWidth / cellSize);
    gridH = ceil(simHeight / cellSize);

    // Init du serpent au centre
    snake.clear();
    int startX = gridW / 2;
    int startY = gridH / 2;
    snake.push_back({startX, startY});
    snake.push_back({startX, startY - 1});
    snake.push_back({startX, startY - 2});

    spawnFood();

    lastMoveTime = 0.0f;
    moveInterval = 0.05f; // Vitesse de déplacement du serpent (plus bas = plus rapide)
}

void AutoSnakeLayer::spawnFood() {
    vector<Cell> freeCells;
    for (int y = 0; y < gridH; y++) {
        for (int x = 0; x < gridW; x++) {
            bool isFree = true;
            for (auto& s : snake) {
                if (s.x == x && s.y == y) {
                    isFree = false;
                    break;
                }
            }
            if (isFree) freeCells.push_back({x, y});
        }
    }
    if (!freeCells.empty()) {
        food = freeCells[ofRandom(freeCells.size())];
    }
}

bool AutoSnakeLayer::findPathBFS(Cell start, Cell target, std::list<Cell>& outPath) {
    std::queue<Cell> q;
    std::map<int, int> cameFrom; 
    std::vector<bool> visited(gridW * gridH, false);

    auto getIndex = [&](Cell c) { return c.y * gridW + c.x; };

    q.push(start);
    visited[getIndex(start)] = true;
    cameFrom[getIndex(start)] = -1;

    bool found = false;
    while(!q.empty()) {
        Cell curr = q.front();
        q.pop();

        if (curr == target) {
            found = true;
            break;
        }

        // Voisins possibles: UP, DOWN, LEFT, RIGHT (avec wrap horizontal)
        Cell neighbors[4] = {
            {(curr.x + 1) % gridW, curr.y}, 
            {(curr.x - 1 + gridW) % gridW, curr.y}, 
            {curr.x, curr.y + 1},
            {curr.x, curr.y - 1}
        };

        for(int i=0; i<4; i++) {
            Cell n = neighbors[i];
            if (n.y >= 0 && n.y < gridH) { // Pas de wrap vertical
                int nIdx = getIndex(n);
                if (!visited[nIdx]) {
                    // Evite le corps du serpent
                    bool isBody = false;
                    for(size_t j = 0; j < snake.size() - 1; j++) {
                        if (snake[j] == n) {
                            isBody = true; break;
                        }
                    }
                    if (!isBody) {
                        visited[nIdx] = true;
                        cameFrom[nIdx] = getIndex(curr);
                        q.push(n);
                    }
                }
            }
        }
    }

    if (found) {
        outPath.clear();
        int currIdx = getIndex(target);
        while(currIdx != -1 && currIdx != getIndex(start)) {
            outPath.push_front({currIdx % gridW, currIdx / gridW});
            currIdx = cameFrom[currIdx];
        }
        return true;
    }
    return false;
}

void AutoSnakeLayer::update(float time) {
    if (time - lastMoveTime >= moveInterval) {
        lastMoveTime = time;
        moveSnake();
    }
}

void AutoSnakeLayer::moveSnake() {
    if (snake.empty()) return;
    Cell head = snake.front();
    
    if (currentPath.empty()) {
        if (!findPathBFS(head, food, currentPath)) {
            // Bloqué ! Reset du snake.
            setup(simWidth, simHeight, collider);
            return;
        }
    }

    if (!currentPath.empty()) {
        Cell nextPos = currentPath.front();
        currentPath.pop_front();

        if (nextPos == food) {
            snake.insert(snake.begin(), nextPos);
            spawnFood();
            currentPath.clear();
        } else {
            for (size_t i = snake.size() - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0] = nextPos;
        }
    }
}

void AutoSnakeLayer::draw() {
    float wrapThreshold = 300.0f;
    vector<ofVec2f> offsets = {ofVec2f(0, 0)};
    
    if (!snake.empty()) {
        float headX = snake.front().x * cellSize;
        if (headX < wrapThreshold) offsets.push_back(ofVec2f(simWidth, 0));
        if (headX > simWidth - wrapThreshold) offsets.push_back(ofVec2f(-simWidth, 0));
    }

    for (auto& off : offsets) {
        drawWrapped(off.x);
    }
}

void AutoSnakeLayer::drawWrapped(float offsetX) {
    ofPushStyle();
    
    // Draw Food
    ofSetColor(255, 50, 50); // Rouge vif pour la pomme
    ofDrawRectangle(food.x * cellSize + offsetX + 4, food.y * cellSize + 4, cellSize - 8, cellSize - 8);

    // Draw Snake
    for (size_t i = 0; i < snake.size(); i++) {
        if (i == 0) ofSetColor(255); // Tête blanche
        else ofSetColor(0);          // Corps noir
        
        float drawX = snake[i].x * cellSize + offsetX;
        ofFill();
        ofDrawRectangle(drawX + 2, snake[i].y * cellSize + 2, cellSize - 4, cellSize - 4);
        
        // Jointures visuelles
        if (i > 0) {
            Cell prev = snake[i-1];
            Cell curr = snake[i];
            if (abs(prev.x - curr.x) <= 1 && abs(prev.y - curr.y) <= 1) {
                float px = (prev.x + curr.x) * cellSize * 0.5f + offsetX;
                float py = (prev.y + curr.y) * cellSize * 0.5f;
                ofSetColor(0);
                ofDrawRectangle(px + 4, py + 4, cellSize - 8, cellSize - 8);
            }
        }
    }
    ofPopStyle();
}