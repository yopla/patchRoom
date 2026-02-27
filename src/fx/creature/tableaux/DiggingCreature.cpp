#include "DiggingCreature.h"

//--------------------------------------------------------------
void DiggingCreature::setup(float x, float y, ofImage& img) {
    pos.set(x, y);
    
    // 1. Init Map from Image (The dirt to dig)
    if(img.isAllocated()) {
        ofImage temp = img;
        temp.resize(mWidth, mHeight);
        temp.setImageType(OF_IMAGE_COLOR_ALPHA);
        mapPix = temp.getPixels();
    } else {
        // Fallback if image not found
        mapPix.allocate(mWidth, mHeight, OF_IMAGE_COLOR_ALPHA);
        mapPix.setColor(ofColor(20, 10, 0, 220));
    }

    // Clear center (Start area) to ensure agents are not stuck
    int cx = mWidth / 2;
    int cy = mHeight / 2;
    int rad = 4;
    for(int y = cy - rad; y < cy + rad; y++) {
        for(int x = cx - rad; x < cx + rad; x++) {
            if(x >= 0 && x < mWidth && y >= 0 && y < mHeight) {
                mapPix.setColor(x, y, ofColor(0, 0, 0, 0)); // Transparent hole
            }
        }
    }
    
    mapTex.allocate(mapPix);
    mapTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST); // Pixelated style
    
    // 2. Init Agents
    int numAgents = 40;
    for(int i=0; i<numAgents; i++) {
        Agent a;
        a.cx = 0; 
        a.cy = 0;
        a.ang = ofRandom(TWO_PI);
        a.vx = cos(a.ang) * 2.0f;
        a.vy = sin(a.ang) * 2.0f;
        a.cargo = 0;
        agents.push_back(a);
    }
    
    // 3. Generate Particle Texture (Similar to Ripple/Fluids)
    int pSize = 32;
    ofFbo fbo;
    fbo.allocate(pSize, pSize, GL_RGBA);
    fbo.begin();
    ofClear(0, 0, 0, 0);
    // Soft glowing circle
    ofSetColor(255, 255, 255, 100); 
    ofDrawCircle(pSize/2, pSize/2, pSize/2);
    ofSetColor(255, 255, 255, 255);
    ofDrawCircle(pSize/2, pSize/2, pSize/4);
    fbo.end();
    
    fbo.readToPixels(particleImg.getPixels());
    particleImg.update();
}

//--------------------------------------------------------------
bool DiggingCreature::isWall(float wx, float wy) {
    // Convert local coords to grid coords
    float gridX = (wx / cw) + (mWidth / 2.0f);
    float gridY = (wy / ch) + (mHeight / 2.0f);
    
    int tx = floor(gridX);
    int ty = floor(gridY);
    
    // Boundaries are walls
    if (tx < 0 || tx >= mWidth) return true; 
    if (ty < 0 || ty >= mHeight) return true;
    
    // If alpha is high, it's a wall
    return (mapPix.getColor(tx, ty).a > 10);
}

//--------------------------------------------------------------
void DiggingCreature::crackMap(float wx, float wy) {
    float gridX = (wx / cw) + (mWidth / 2.0f);
    float gridY = (wy / ch) + (mHeight / 2.0f);
    
    int tx = floor(gridX);
    int ty = floor(gridY);
    
    if (tx < 0 || tx >= mWidth) return;
    if (ty < 0 || ty >= mHeight) return;
    
    // Dig: Reduce alpha (Create Alpha)
    ofColor c = mapPix.getColor(tx, ty);
    if(c.a > 0) {
        int digSpeed = 60; 
        if(c.a > digSpeed) c.a -= digSpeed;
        else c.a = 0;
        
        mapPix.setColor(tx, ty, c);
        mapTex.loadData(mapPix); // Update GPU texture
    }
}

//--------------------------------------------------------------
void DiggingCreature::update(float mx, float my) {
    if(!bEnabled) return;
    
    // Interaction: Accelerate if clicking on it
    float d = ofDist(mx, my, pos.x, pos.y);
    bool isClicking = (d < (mWidth * cw * 0.5f) && ofGetMousePressed(0));
    
    int iterations = (bAccelerate || isClicking) ? 4 : 1;
    
    for(int k=0; k<iterations; k++) {
        for(auto& a : agents) {
            if (a.cargo > 0) {
                // Return to center (AS3 logic)
                float dx = -a.cx;
                float dy = -a.cy;
                a.ang = atan2(dy, dx);
                
                if (abs(dx) < 8 && abs(dy) < 8) {
                    a.cargo = 0; 
                    a.vx = 0; a.vy = 0; 
                    a.ang = ofRandom(TWO_PI);
                }
            } 
            
            // Move
            a.vx = cos(a.ang) * 3.0f;
            a.vy = sin(a.ang) * 3.0f;
            
            // Collision & Digging
            bool hit = false;
            if (isWall(a.cx + a.vx, a.cy + a.vy)) {
                hit = true;
                crackMap(a.cx + a.vx, a.cy + a.vy);
            }
            
            if (hit) {
                a.vx = 0; a.vy = 0;
                a.cargo = 1; // Hit wall -> Dig & Return
                a.ang += PI; // Turn around
            } else {
                a.cx += a.vx;
                a.cy += a.vy;
                if(ofRandom(1.0) < 0.05) a.ang += ofRandom(-0.5, 0.5);
            }
        }
    }
}

//--------------------------------------------------------------
void DiggingCreature::draw() {
    if(!bEnabled) return;
    
    ofPushMatrix();
    ofTranslate(pos);
    
    // Draw Map (The dirt)
    ofSetColor(255);
    float drawW = mWidth * cw;
    float drawH = mHeight * ch;
    mapTex.draw(-drawW/2, -drawH/2, drawW, drawH);
    
    // Draw Agents
    for(auto& a : agents) {
        ofSetColor(255, 200, 150);
        particleImg.draw(a.cx - 8, a.cy - 8, 16, 16);
    }
    
    ofPopMatrix();
}
