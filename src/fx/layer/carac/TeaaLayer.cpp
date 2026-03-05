#include "TeaaLayer.h"
#include <algorithm>
#include <functional>
#include <queue>

//--------------------------------------------------------------
void TeaaLayer::setup(float w, float h, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    collider = col;
    
    cellSize = 20.0f; // Higher resolution
    gridW = ceil(w / cellSize);
    gridH = ceil(h / cellSize);
    
    // Resize optimization vector
    pathFinderNodes.resize(gridW * gridH);
    
    generate();
}

//--------------------------------------------------------------
void TeaaLayer::generate() {
    // 1. Init Grid with Walls (0)
    grid.assign(gridW, vector<int>(gridH, 0));
    enemies.clear();
    rooms.clear();
    
    // 2. Integrate Collider (3)
    if(collider) {
        for(int x=0; x<gridW; x++) {
            for(int y=0; y<gridH; y++) {
                // Convert to world space then to collider simulation space
                float wx = x * cellSize + cellSize * 0.5f;
                float wy = y * cellSize + cellSize * 0.5f;
                float simX = wx / collider->scale;
                float simY = wy / collider->scale;
                
                if(collider->isWall(simX, simY)) {
                    grid[x][y] = 3; // 3 = Collider Obstacle
                }
            }
        }
    }
    
    // 3. Generate Rooms (Complex Shapes & Varied Sizes)
    int numRoomTries = 400; // Try to place many rooms
    for(int i=0; i<numRoomTries; i++) {
        int w = ofRandom(3, 12); 
        int h = ofRandom(3, 12);
        
        // Force some large rooms
        if(ofRandom(1.0) < 0.1) { w = ofRandom(10, 20); h = ofRandom(10, 20); }
        
        // Force top/bottom passages (Lignes en salles haut et basses)
        int y = 0;
        if(ofRandom(1.0) < 0.3) {
            // Top or Bottom strip
            if(ofRandom(1.0) < 0.5) y = ofRandom(1, 6);
            else y = ofRandom(gridH - 8, gridH - 2);
        } else {
            y = ofRandom(1, gridH - h - 1);
        }
        
        int x = ofRandom(1, gridW - w - 1);
        
        // Check collision with Colliders (3)
        bool hitsCollider = false;
        for(int ix=x; ix<x+w; ix++) {
            for(int iy=y; iy<y+h; iy++) {
                if(ix >= 0 && ix < gridW && iy >= 0 && iy < gridH) {
                    if(grid[ix][iy] == 3) hitsCollider = true;
                }
            }
        }
        
        if(!hitsCollider) {
            createRoom(x, y, w, h);
        }
    }
    
    // 4. Connect Rooms (Non-linear)
    // Collect centers of all generated rooms
    vector<ofPoint> centers;
    for(auto& r : rooms) centers.push_back(ofPoint(r.x + r.w/2, r.y + r.h/2));
    
    // Sort by X to facilitate linear connection
    sort(centers.begin(), centers.end(), [](const ofPoint& a, const ofPoint& b){
        return a.x < b.x;
    });
    
    // Connect adjacent rooms in sorted list (Backbone)
    for(size_t i=1; i<centers.size(); i++) {
        ofPoint p1 = centers[i-1];
        ofPoint p2 = centers[i];
        
        // Simple L-shape corridor
        if(ofRandom(1.0) < 0.5) {
            createHCorridor(p1.x, p2.x, p1.y);
            createVCorridor(p1.y, p2.y, p2.x);
        } else {
            createVCorridor(p1.y, p2.y, p1.x);
            createHCorridor(p1.x, p2.x, p2.y);
        }
    }
    
    // Add random connections for loops (Non-linear)
    int extraConnections = centers.size() * 0.5;
    for(int i=0; i<extraConnections; i++) {
        if(centers.size() < 2) break;
        int idx1 = ofRandom(centers.size());
        int idx2 = ofRandom(centers.size());
        if(idx1 == idx2) continue;
        
        ofPoint p1 = centers[idx1];
        ofPoint p2 = centers[idx2];
        
        // Only connect if reasonably close to avoid long boring corridors
        if(p1.distance(p2) < gridW * 0.3) {
             if(ofRandom(1.0) < 0.5) {
                createHCorridor(p1.x, p2.x, p1.y);
                createVCorridor(p1.y, p2.y, p2.x);
            } else {
                createVCorridor(p1.y, p2.y, p1.x);
                createHCorridor(p1.x, p2.x, p2.y);
            }
        }
    }
    
    // Ensure horizontal passage at top and bottom (wrapping support)
    createHCorridor(0, gridW-1, 2); // Top passage
    createHCorridor(0, gridW-1, gridH-3); // Bottom passage
    
    // 3. Spawn Enemies (Fixed count)
    while(enemies.size() < numEnemies) {
        int rx = ofRandom(gridW);
        int ry = ofRandom(gridH);
        if(isWalkable(rx, ry)) {
            TeaaEnemy e;
            e.setup(rx, ry, cellSize);
            enemies.push_back(e);
        }
    }
    
    // 5. Calculate Room Exits (Room cells adjacent to Corridor cells)
    for(auto& r : rooms) {
        r.exits.clear();
        for(int i=r.x; i<r.x+r.w; i++) {
            for(int j=r.y; j<r.y+r.h; j++) {
                if(grid[i][j] == 2) { // Only check if it is still a room cell (not overwritten by corridor)
                    // Check neighbors for Corridor (1)
                    if( (i+1 < gridW && grid[i+1][j] == 1) ||
                        (i-1 >= 0 && grid[i-1][j] == 1) || 
                        (j+1 < gridH && grid[i][j+1] == 1) ||
                        (j-1 >= 0 && grid[i][j-1] == 1) ) {
                        r.exits.push_back(ofPoint(i, j));
                    }
                }
            }
        }
    }
    
    // 6. Build Mesh for optimized drawing
    mapMesh.clear();
    mapMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for(int x=0; x<gridW; x++) {
        for(int y=0; y<gridH; y++) {
            if(grid[x][y] == 0) continue;
            
            ofColor c;
            if(grid[x][y] == 3) c.set(180, 100, 220); // Collider
            else c.set(102, 153, 153); // Room/Corridor
            
            float px = x * cellSize;
            float py = y * cellSize;
            float s = cellSize;
            
            // Add Quad (2 triangles)
            // TL, TR, BL
            mapMesh.addVertex(ofVec3f(px, py, 0)); mapMesh.addColor(c);
            mapMesh.addVertex(ofVec3f(px+s, py, 0)); mapMesh.addColor(c);
            mapMesh.addVertex(ofVec3f(px, py+s, 0)); mapMesh.addColor(c);
            
            // TR, BR, BL
            mapMesh.addVertex(ofVec3f(px+s, py, 0)); mapMesh.addColor(c);
            mapMesh.addVertex(ofVec3f(px+s, py+s, 0)); mapMesh.addColor(c);
            mapMesh.addVertex(ofVec3f(px, py+s, 0)); mapMesh.addColor(c);
        }
    }
}

//--------------------------------------------------------------
void TeaaLayer::createRoom(int x, int y, int w, int h) {
    rooms.push_back({x, y, w, h});
    for(int i=x; i<x+w; i++) {
        for(int j=y; j<y+h; j++) {
            if(i>=0 && i<gridW && j>=0 && j<gridH && grid[i][j] != 3) grid[i][j] = 2; // 2 = Room, don't overwrite collider
        }
    }
}

//--------------------------------------------------------------
void TeaaLayer::createHCorridor(int x1, int x2, int y) {
    for(int x=std::min(x1,x2); x<=std::max(x1,x2); x++) {
        if(x>=0 && x<gridW && y>=0 && y<gridH && grid[x][y] != 3) grid[x][y] = 1;
    }
}

//--------------------------------------------------------------
void TeaaLayer::createVCorridor(int y1, int y2, int x) {
    for(int y=std::min(y1,y2); y<=std::max(y1,y2); y++) {
        if(x>=0 && x<gridW && y>=0 && y<gridH && grid[x][y] != 3) grid[x][y] = 1;
    }
}

//--------------------------------------------------------------
bool TeaaLayer::isWalkable(int x, int y) {
    // Wrap X coordinate
    int wx = (x % gridW + gridW) % gridW;
    if(y < 0 || y >= gridH) return false;
    return (grid[wx][y] == 1 || grid[wx][y] == 2); // 1 (Corridor) or 2 (Room), NOT 3 (Collider)
}

//--------------------------------------------------------------
TeaaRoom* TeaaLayer::getRoomAt(int x, int y) {
    for(auto& r : rooms) {
        if(r.contains(x, y)) return &r;
    }
    return nullptr;
}

//--------------------------------------------------------------
struct AStarNode {
    int x, y;
    float f;
    
    // Priority queue orders by greatest element, so we invert comparison for smallest f
    bool operator>(const AStarNode& other) const {
        return f > other.f;
    }
};

vector<ofPoint> TeaaLayer::findPath(int sx, int sy, int ex, int ey) {
    vector<ofPoint> path;
    if(!isWalkable(ex, ey)) return path;
    
    // Optimization: Reuse flat vector
    currentQueryId++;
    
    std::priority_queue<AStarNode, vector<AStarNode>, std::greater<AStarNode>> openSet;
    
    int startIdx = sx + sy * gridW;
    pathFinderNodes[startIdx].g = 0;
    pathFinderNodes[startIdx].queryId = currentQueryId;
    pathFinderNodes[startIdx].closed = false;
    
    openSet.push({sx, sy, 0});
    
    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};
    
    while(!openSet.empty()) {
        AStarNode current = openSet.top();
        openSet.pop();
        
        if(current.x == ex && current.y == ey) {
            // Reconstruct path
            int cx = ex, cy = ey;
            while(cx != sx || cy != sy) {
                path.push_back(ofPoint(cx, cy));
                TeaaPathNode& info = pathFinderNodes[cx + cy * gridW];
                int tx = info.px;
                int ty = info.py;
                cx = tx; cy = ty;
            }
            reverse(path.begin(), path.end());
            return path;
        }
        
        int cIdx = current.x + current.y * gridW;
        if(pathFinderNodes[cIdx].queryId == currentQueryId && pathFinderNodes[cIdx].closed) continue;
        
        pathFinderNodes[cIdx].closed = true;
        pathFinderNodes[cIdx].queryId = currentQueryId;
        
        for(int i=0; i<4; i++) {
            int nx = (current.x + dx[i] + gridW) % gridW; // Wrap X neighbor
            int ny = current.y + dy[i];
            
            int nIdx = nx + ny * gridW;
            
            // Initialize node if first time seen in this query
            if(pathFinderNodes[nIdx].queryId != currentQueryId) {
                pathFinderNodes[nIdx].queryId = currentQueryId;
                pathFinderNodes[nIdx].g = 1e9;
                pathFinderNodes[nIdx].closed = false;
            }
            
            if(isWalkable(nx, ny) && !pathFinderNodes[nIdx].closed) {
                float newG = pathFinderNodes[cIdx].g + 1;
                if(newG < pathFinderNodes[nIdx].g) {
                    pathFinderNodes[nIdx].g = newG;
                    pathFinderNodes[nIdx].px = current.x;
                    pathFinderNodes[nIdx].py = current.y;
                    
                    // Heuristic with wrapping distance
                    float distX = abs(nx - ex);
                    if(distX > gridW/2) distX = gridW - distX;
                    float h = distX + abs(ny - ey);
                    openSet.push({nx, ny, newG + h});
                }
            }
        }
    }
    return path;
}

//--------------------------------------------------------------
void TeaaLayer::update(float time) {
    if(!bActive) return;
    
    // Turn based logic (Simulating the AS3 frame/turn system)
    if(time - lastTurnTime > turnInterval) {
        lastTurnTime = time;

        int pathsComputed = 0;
        int maxPathsPerTurn = 100; // Increased to prevent stuck agents

        for(auto& e : enemies) {
            // 1. If no path or reached target, find new target
            if(e.path.empty() && pathsComputed < maxPathsPerTurn) {
                int attempts = 0;
                while(attempts < 10) {
                    int tx = ofRandom(gridW);
                    int ty = ofRandom(gridH);
                    // Ensure target is walkable and somewhat far
                    if(isWalkable(tx, ty) && (abs(tx - e.gx) + abs(ty - e.gy) > 10)) {
                        e.path = findPath(e.gx, e.gy, tx, ty);
                        pathsComputed++;
                        if(!e.path.empty()) break;
                    }
                    attempts++;
                }
            }
            
            // 2. Follow path
            if(!e.path.empty()) {
                ofPoint next = e.path[0];
                
                // Check collision with other enemies
                bool occupied = false;
                for(auto& other : enemies) {
                    if(&e != &other && other.gx == (int)next.x && other.gy == (int)next.y) occupied = true;
                }
                
                if(!occupied) {
                    // Determine direction for eyes
                    int dx = (int)next.x - e.gx;
                    if(dx == 1 || dx < -1) e.dir = 0; // Right or Wrap Right
                    else if(dx == -1 || dx > 1) e.dir = 2; // Left or Wrap Left
                    else if(next.y > e.gy) e.dir = 1;
                    else if(next.y < e.gy) e.dir = 3;
                    
                    e.gx = (int)next.x;
                    e.gy = (int)next.y;
                    e.path.erase(e.path.begin());
                    e.stuckCount = 0;
                } else {
                    // Deadlock prevention: if stuck, give up path to pick a new target
                    e.stuckCount++;
                    if(e.stuckCount > 5) {
                        e.path.clear();
                        e.stuckCount = 0;
                        
                        // Try to step aside randomly to unblock
                        int dx[] = {1, -1, 0, 0};
                        int dy[] = {0, 0, 1, -1};
                        int startDir = ofRandom(4);
                        for(int k=0; k<4; k++) {
                            int dir = (startDir + k) % 4;
                            int nx = (e.gx + dx[dir] + gridW) % gridW;
                            int ny = e.gy + dy[dir];
                            
                            bool isOcc = false;
                            for(auto& other : enemies) if(other.gx == nx && other.gy == ny) isOcc = true;
                            
                            if(isWalkable(nx, ny) && !isOcc) {
                                e.gx = nx; e.gy = ny;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Visual update (Lerp)
    for(auto& e : enemies) {
        // Handle wrapping for visual lerp
        float targetX = e.gx * cellSize;
        if(abs(targetX - e.pos.x) > simWidth * 0.5f) {
            if(targetX > e.pos.x) e.pos.x += simWidth;
            else e.pos.x -= simWidth;
        }
        e.update(cellSize);
        
        // Normalize visual pos
        if(e.pos.x < 0) e.pos.x += simWidth;
        if(e.pos.x > simWidth) e.pos.x -= simWidth;
    }
}

//--------------------------------------------------------------
void TeaaLayer::draw() {
    if(!bActive) return;
    
    ofPushStyle();
    
    // No centering needed if we fill the width
    float offsetX = 0;
    float offsetY = (simHeight - gridH * cellSize) * 0.5f;
    
    ofTranslate(offsetX, offsetY);
    
    // Draw Map Mesh (Optimized)
    ofSetColor(255);
    mapMesh.draw();
    
    // Draw Enemies
    for(auto& e : enemies) {
        e.draw(cellSize);
        
        // Draw ghosts for wrapping
        if(e.pos.x < 100) {
            ofPushMatrix(); ofTranslate(simWidth, 0); e.draw(cellSize); ofPopMatrix();
        }
        if(e.pos.x > simWidth - 100) {
            ofPushMatrix(); ofTranslate(-simWidth, 0); e.draw(cellSize); ofPopMatrix();
        }
    }
    
    ofPopStyle();
}

//--------------------------------------------------------------
void TeaaLayer::addAgent(float x, float y) {
    int gx = x / cellSize;
    int gy = y / cellSize;
    gx = (gx % gridW + gridW) % gridW; // Wrap
    
    if(isWalkable(gx, gy)) {
        TeaaEnemy e;
        e.setup(gx, gy, cellSize);
        enemies.push_back(e);
    }
}

//--------------------------------------------------------------
void TeaaLayer::mousePressed(float x, float y, int button) {
    addAgent(x, y);
}
