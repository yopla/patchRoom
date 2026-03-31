#include "WallWalkerLayer.h"

//--------------------------------------------------------------
void WallWalkerLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    scale = s;
    collider = col;
    
    walkers.clear();
    // Create 50 creatures
    for(int i=0; i<50; i++) {
        WallWalker w;
        // Spawn au milieu, un peu vers le bas
        w.pos.set(simWidth * 0.5f + ofRandom(-100, 100), simHeight * 0.6f + ofRandom(-100, 100));
        w.dir = (int)ofRandom(4);
        w.isJumping = true; // Start in air to find a wall
        w.vel.set(0, 0);
        // Colors inspired by "Red Walkers" from ref
        w.color = ofColor(255, ofRandom(50, 100), 50); 
        w.jumpTimer = ofRandom(100.0f); // Random offset
        w.turnCount = 0;
        w.stuckTimer = 0;
        w.lastPos = w.pos;
        walkers.push_back(w);
    }
}

//--------------------------------------------------------------
bool WallWalkerLayer::isWall(float x, float y) {
    if(!collider) return false;
    // Handle wrapping for X
    if(x < 0) x += simWidth;
    if(x >= simWidth) x -= simWidth;
    return collider->isWall(x, y);
}

//--------------------------------------------------------------
void WallWalkerLayer::jump(WallWalker& w) {
    w.isJumping = true;
    
    // Jump perpendicular to current movement (away from wall)
    // Logic: Wall is always to the "Right" of movement vector
    // We map the 8 directions to the 4 main wall normals
    ofVec2f normal;
    
    // 0,1 (Right/CornerDown) -> Wall Down -> Jump Up
    if(w.dir == 0 || w.dir == 1) normal.set(0, -1);
    // 2,3 (Down/CornerLeft) -> Wall Left -> Jump Right
    else if(w.dir == 2 || w.dir == 3) normal.set(1, 0);
    // 4,5 (Left/CornerUp) -> Wall Up -> Jump Down
    else if(w.dir == 4 || w.dir == 5) normal.set(0, 1);
    // 6,7 (Up/CornerRight) -> Wall Right -> Jump Left
    else normal.set(-1, 0);

    // Add randomness to jump direction
    normal.rotate(ofRandom(-20, 20));
    
    w.vel = normal * ofRandom(8.0f, 15.0f);
}

//--------------------------------------------------------------
void WallWalkerLayer::update(float mouseX, float mouseY, float time) {
    for(auto& w : walkers) {
        updateWalker(w);
    }
}

//--------------------------------------------------------------
void WallWalkerLayer::updateWalker(WallWalker& w) {
    if(w.isJumping) {
        // --- AIRBORNE STATE ---
        w.pos += w.vel;
        w.vel.y += 0.4f; // Gravity
        w.vel *= 0.99f;  // Air friction
        
        // Check for landing
        if(isWall(w.pos.x, w.pos.y)) {
            w.isJumping = false;
            
            // Back up out of wall
            w.pos -= w.vel; 
            w.vel.set(0,0);
            
            // Determine wall orientation to set direction
            float d = 4.0f;
            bool wRight = isWall(w.pos.x + d, w.pos.y);
            bool wLeft  = isWall(w.pos.x - d, w.pos.y);
            bool wDown  = isWall(w.pos.x, w.pos.y + d);
            bool wUp    = isWall(w.pos.x, w.pos.y - d);
            
            // Set direction to walk along the wall (Clockwise logic)
            if(wDown) w.dir = 0;       // Wall down -> Move Right
            else if(wLeft) w.dir = 2;  // Wall Left -> Move Down
            else if(wUp) w.dir = 4;    // Wall Up -> Move Left
            else if(wRight) w.dir = 6; // Wall Right -> Move Up
            else w.dir = 0;
            w.turnCount = 0; // Reset du compteur de tours au atterrissage
        }
        
        // Floor safety
        if(w.pos.y > simHeight) {
            w.pos.y = simHeight - 2;
            w.isJumping = false;
            w.dir = 0; 
            w.vel.set(0,0);
        }
        
    } else {
        // --- WALL FOLLOWING STATE ---
        // Logic: 8-State Machine (0-7) handling convex and concave corners

        // Random Jump
        if(ofRandom(1.0f) < 0.005f) { // 0.5% chance per frame
            jump(w);
            return;
        }

        // Radius for collision checks
        float r = w.size; 
        float speed = w.speed;
        
        // State Machine based on liUv.as (Red Walkers)
        // Even states (0, 2, 4, 6) are moving along a surface
        // Odd states (1, 3, 5, 7) are moving around a convex corner
        
        if (w.dir == 0) { // Moving Right (Wall Down)
            if (isWall(w.pos.x + r + speed, w.pos.y)) { // Wall Ahead -> Concave Corner
                w.dir = 6; // Switch to Moving Up
            } else if (isWall(w.pos.x, w.pos.y + r + 2)) { // Wall Below -> Continue
                w.pos.x += speed;
                if (isWall(w.pos.x, w.pos.y + r - 1)) w.pos.y -= 1; // Slope adjustment
            } else { // No Wall Below -> Convex Corner
                w.dir = 1; // Switch to Corner Down
            }
        }
        else if (w.dir == 1) { // Corner Down (Moving Down to find wall on Left)
            w.turnCount++;
            if(w.turnCount > 20) { jump(w); return; }

            if (!isWall(w.pos.x - r - 2, w.pos.y)) { // No Wall Left yet
                w.pos.y += speed;
            } else { // Found Wall Left
                w.dir = 2; // Switch to Moving Down
                w.turnCount = 0;
            }
        }
        else if (w.dir == 2) { // Moving Down (Wall Left)
            if (isWall(w.pos.x, w.pos.y + r + speed)) { // Wall Ahead -> Concave
                w.dir = 0; // Switch to Moving Right
            } else if (isWall(w.pos.x - r - 2, w.pos.y)) { // Wall Left -> Continue
                w.pos.y += speed;
                if (isWall(w.pos.x - r + 1, w.pos.y)) w.pos.x += 1; // Slope
            } else { // No Wall Left -> Convex
                w.dir = 3; // Switch to Corner Left
            }
        }
        else if (w.dir == 3) { // Corner Left (Moving Left to find wall Up)
            w.turnCount++;
            if(w.turnCount > 20) { jump(w); return; }

            if (!isWall(w.pos.x, w.pos.y - r - 2)) { // No Wall Up yet
                w.pos.x -= speed;
            } else { // Found Wall Up
                w.dir = 4; // Switch to Moving Left
                w.turnCount = 0;
            }
        }
        else if (w.dir == 4) { // Moving Left (Wall Up)
            if (isWall(w.pos.x - r - speed, w.pos.y)) { // Wall Ahead -> Concave
                w.dir = 2; // Switch to Moving Down
            } else if (isWall(w.pos.x, w.pos.y - r - 2)) { // Wall Up -> Continue
                w.pos.x -= speed;
                if (isWall(w.pos.x, w.pos.y - r + 1)) w.pos.y += 1; // Slope
            } else { // No Wall Up -> Convex
                w.dir = 5; // Switch to Corner Up
            }
        }
        else if (w.dir == 5) { // Corner Up (Moving Up to find wall Right)
            w.turnCount++;
            if(w.turnCount > 20) { jump(w); return; }

            if (!isWall(w.pos.x + r + 2, w.pos.y)) { // No Wall Right yet
                w.pos.y -= speed;
            } else { // Found Wall Right
                w.dir = 6; // Switch to Moving Up
                w.turnCount = 0;
            }
        }
        else if (w.dir == 6) { // Moving Up (Wall Right)
            if (isWall(w.pos.x, w.pos.y - r - speed)) { // Wall Ahead -> Concave
                w.dir = 4; // Switch to Moving Left
            } else if (isWall(w.pos.x + r + 2, w.pos.y)) { // Wall Right -> Continue
                w.pos.y -= speed;
                if (isWall(w.pos.x + r - 1, w.pos.y)) w.pos.x -= 1; // Slope
            } else { // No Wall Right -> Convex
                w.dir = 7; // Switch to Corner Right
            }
        }
        else if (w.dir == 7) { // Corner Right (Moving Right to find wall Down)
            w.turnCount++;
            if(w.turnCount > 20) { jump(w); return; }

            if (!isWall(w.pos.x, w.pos.y + r + 2)) { // No Wall Down yet
                w.pos.x += speed;
            } else { // Found Wall Down
                w.dir = 0; // Switch to Moving Right
                w.turnCount = 0;
            }
        }
        
        // --- ANTI-BLOCAGE GLOBAL ---
        // Si la position n'a pas changé par rapport à la frame précédente (vibration ou blocage)
        if(w.pos.squareDistance(w.lastPos) < 0.1f) {
            w.stuckTimer++;
        } else {
            w.stuckTimer = 0;
        }
        w.lastPos = w.pos; // Mémorisation pour la prochaine frame

        // Si bloqué plus de 10 frames, on force un saut
        if(w.stuckTimer > 10) {
            jump(w);
            w.stuckTimer = 0;
        }

        // Anti-stuck safety: if inside wall, push out
        if(isWall(w.pos.x, w.pos.y)) {
             // Si on est coincé dans un mur, on saute immédiatement
             jump(w);
        }
    }
    
    // Screen Wrapping
    if(w.pos.x < 0) w.pos.x += simWidth;
    if(w.pos.x >= simWidth) w.pos.x -= simWidth;
}

//--------------------------------------------------------------
void WallWalkerLayer::draw() {
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CENTER);

    // Lambda pour dessiner une créature (permet de dessiner les fantômes facilement)
    auto drawOne = [&](const WallWalker& w, float ox, float oy) {
        ofPushMatrix();
        ofTranslate((w.pos + ofVec2f(ox, oy)) * scale);
        ofScale(scale, scale);
        
        // Rotation based on direction or velocity
        if(w.isJumping) {
            float angle = atan2(w.vel.y, w.vel.x) * RAD_TO_DEG;
            ofRotateDeg(angle);
        } else {
            // Map 8 states to rotation (0=0, 2=90, 4=180, 6=270)
            // Odd states are corners (45, 135, etc)
            ofRotateDeg(w.dir * 45);
        }
        
        // Draw Creature (Little Bug)
        ofSetColor(w.color);
        ofDrawRectangle(0, 0, w.size, w.size/2); // Body
        
        // Legs/Feelers
        ofSetColor(255);
        if(!w.isJumping) {
            float legOffset = sin(ofGetElapsedTimef() * 20.0f) * 3.0f;
            // Coordonnées relatives à la taille
            float lx = w.size * 0.33f;
            float ly = w.size * 0.33f;
            float footX = w.size * 0.5f;
            float footY = w.size * 0.66f;
            
            ofDrawLine(-lx, ly, -footX, footY + legOffset);
            ofDrawLine(lx, ly, footX, footY - legOffset);
        }
        
        ofPopMatrix();
    };
    
    for(auto& w : walkers) {
        // Dessin normal
        drawOne(w, 0, 0);
        
        // Dessin des fantômes pour le wrapping (Bouclage texture Gauche/Droite)
        if(w.pos.x < 50) drawOne(w, simWidth, 0);
        else if(w.pos.x > simWidth - 50) drawOne(w, -simWidth, 0);
    }
    ofPopStyle();
}
