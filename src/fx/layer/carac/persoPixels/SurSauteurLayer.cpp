#include "SurSauteurLayer.h"

void SurSauteurLayer::setup(float realSceneWidth, float targetRealHeight, shared_ptr<ColliderLayer> colliders, shared_ptr<EatMapLayer> eatMap) {
    this->colliderLayer = colliders;
    this->eatMapLayer = eatMap;

    scale = realSceneWidth / simWidth;
    simHeight = targetRealHeight / scale;

    sauteurs.resize(numSauteurs);
    for(auto& s : sauteurs) {
        s.pos.x = ofRandom(simWidth);
        s.pos.y = ofRandom(simHeight * 0.5); 
        s.xrad = .75; 
        s.yrad = .75;
        s.ready = true;
        s.vel.set(0,0);
    }
}

void SurSauteurLayer::loadTexture(string path) {
    currentTextPath = path;
    ofImage img;
    if (img.load(path)) {
        float imgRatio = img.getWidth() / img.getHeight();
        bool isFullSize = (imgRatio < 4.0f);
        float offsetWorldY = isFullSize ? 912.0f : 0.0f;
        textSimOffsetY = offsetWorldY / scale;
        
        float targetHeight = simWidth / imgRatio;
        img.resize(simWidth, targetHeight);
        img.setImageType(OF_IMAGE_COLOR_ALPHA);
        
        originalTextPixels = img.getPixels();
        textSurSaut.setFromPixels(originalTextPixels);
        bHasText = true;
    }
}

void SurSauteurLayer::reset() {
    if (bHasText && originalTextPixels.isAllocated()) {
        textSurSaut.setFromPixels(originalTextPixels);
    }
    if (eatMapLayer) {
        eatMapLayer->reset();
    }
    particles.clear();
}

void SurSauteurLayer::update(float mouseX, float mouseY, float time) {
    if(!colliderLayer) return;

    for (int i = particles.size() - 1; i >= 0; i--) {
        particles[i].pos += particles[i].vel;
        particles[i].vel.y += 0.2f; 
        particles[i].life -= 1.0f;
        if (particles[i].life <= 0) {
            particles.erase(particles.begin() + i);
        }
    }

    for(auto& s : sauteurs) {
        s.vel.y += 0.2;

        if(s.vel.y > 5.0) s.vel.y = 5.0; 

        int bGround = 0;
        if (colliderLayer->isWall(s.pos.x, s.pos.y + s.yrad + 1) || (eatMapLayer && eatMapLayer->isWall(s.pos.x, s.pos.y + s.yrad + 1))) {
            bGround = 1;
        }

        if (bGround) {
            s.w += 1;
            s.vel.x *= 0.8; 
        }

        if (s.w >= 32) { 
            s.w = 0;
            s.vel.x = (ofRandom(1.0) - ofRandom(1.0)) * 3.0; 
            s.vel.y = -3.5 + (ofRandom(1.0) - ofRandom(1.0)) * -2.0; 
            
            bool wallRight = colliderLayer->isWall(s.pos.x + s.xrad + 1, s.pos.y) || (eatMapLayer && eatMapLayer->isWall(s.pos.x + s.xrad + 1, s.pos.y));
            bool wallLeft = colliderLayer->isWall(s.pos.x - s.xrad - 1, s.pos.y) || (eatMapLayer && eatMapLayer->isWall(s.pos.x - s.xrad - 1, s.pos.y));
            if (s.vel.x > 0 && wallRight) s.vel.x = -s.vel.x;
            else if (s.vel.x < 0 && wallLeft) s.vel.x = -s.vel.x;
        }

        bool wallDown = colliderLayer->isWall(s.pos.x, s.pos.y + s.yrad + s.vel.y);
        bool eatDown = eatMapLayer && eatMapLayer->isWall(s.pos.x, s.pos.y + s.yrad + s.vel.y);
        if (s.vel.y > 0 && (wallDown || eatDown)) {
            s.vel.y = 0;
            if (eatDown) explode(s.pos.x, s.pos.y + s.yrad, 10.0f);
        }
        
        bool wallUp = colliderLayer->isWall(s.pos.x, s.pos.y - s.yrad + s.vel.y);
        bool eatUp = eatMapLayer && eatMapLayer->isWall(s.pos.x, s.pos.y - s.yrad + s.vel.y);
        if (s.vel.y < 0 && (wallUp || eatUp)) {
            s.vel.y = 0;
            if (eatUp) explode(s.pos.x, s.pos.y - s.yrad, 10.0f);
        }
        
        bool wallRightV = colliderLayer->isWall(s.pos.x + s.xrad + s.vel.x, s.pos.y);
        bool eatRightV = eatMapLayer && eatMapLayer->isWall(s.pos.x + s.xrad + s.vel.x, s.pos.y);
        if (s.vel.x > 0 && (wallRightV || eatRightV)) {
            s.vel.x = -s.vel.x * 0.5;
            if (eatRightV) explode(s.pos.x + s.xrad, s.pos.y, 10.0f);
        }
        
        bool wallLeftV = colliderLayer->isWall(s.pos.x - s.xrad + s.vel.x, s.pos.y);
        bool eatLeftV = eatMapLayer && eatMapLayer->isWall(s.pos.x - s.xrad + s.vel.x, s.pos.y);
        if (s.vel.x < 0 && (wallLeftV || eatLeftV)) {
            s.vel.x = -s.vel.x * 0.5;
            if (eatLeftV) explode(s.pos.x - s.xrad, s.pos.y, 10.0f);
        }

        s.pos.x += s.vel.x;
        s.pos.y += s.vel.y;

        if (colliderLayer->isWall(s.pos.x, s.pos.y) || (eatMapLayer && eatMapLayer->isWall(s.pos.x, s.pos.y))) {
            s.pos.y -= 0.5;
        }

        if (s.pos.x < 0) s.pos.x += simWidth;
        if (s.pos.x > simWidth) s.pos.x -= simWidth;
        
        if (s.pos.y > simHeight) {
            s.pos.y = 0;
            s.vel.set(0,0);
        }
    }
    
    for(size_t i=0; i<sauteurs.size(); i++) {
        for(size_t j=i+1; j<sauteurs.size(); j++) {
            float dx = sauteurs[i].pos.x - sauteurs[j].pos.x;
            float dy = sauteurs[i].pos.y - sauteurs[j].pos.y;
            float distSq = dx*dx + dy*dy;
            float minDist = sauteurs[i].xrad + sauteurs[j].xrad + 2.0f; 
            if(distSq < minDist*minDist && distSq > 0) {
                float dist = sqrt(distSq);
                float overlap = minDist - dist;
                ofVec2f normal(dx/dist, dy/dist);
                
                sauteurs[i].pos += normal * overlap * 0.5f;
                sauteurs[j].pos -= normal * overlap * 0.5f;
                
                ofVec2f relVel = sauteurs[i].vel - sauteurs[j].vel;
                float velAlongNormal = relVel.dot(normal);
                if(velAlongNormal > 0) continue; 
                
                float restitution = 0.8f;
                float jImpulse = -(1 + restitution) * velAlongNormal;
                jImpulse /= 2.0f; 
                
                ofVec2f impulse = normal * jImpulse;
                sauteurs[i].vel += impulse;
                sauteurs[j].vel -= impulse;
            }
        }
    }
}

void SurSauteurLayer::explode(float x, float y, float radius) {
    if (eatMapLayer) {
        eatMapLayer->explode(x, y, radius);
    }
    
    if (bHasText) {
        int cx = (int)x;
        int cy = (int)(y + textSimOffsetY);
        int r = (int)radius;
        bool modified = false;
        
        ofPixels& pix = textSurSaut.getPixels();
        int w = pix.getWidth();
        int h = pix.getHeight();
        
        for (int iy = cy - r; iy <= cy + r; iy++) {
            for (int ix = cx - r; ix <= cx + r; ix++) {
                if (ix >= 0 && ix < w && iy >= 0 && iy < h) {
                    float distSq = (ix - cx) * (ix - cx) + (iy - cy) * (iy - cy);
                    if (distSq <= r * r) {
                        ofColor c = pix.getColor(ix, iy);
                        if (c.a > 0) {
                            if (ofRandom(1.0) < 0.2) { 
                                ExplodingPixel ep;
                                ep.pos.set(ix, iy - textSimOffsetY);
                                ep.vel.set(ofRandom(-2, 2), ofRandom(-4, 0));
                                ep.color = c;
                                ep.life = ofRandom(20, 60);
                                ep.maxLife = ep.life;
                                particles.push_back(ep);
                            }
                            pix.setColor(ix, iy, ofColor(0, 0, 0, 0));
                            modified = true;
                        }
                    }
                }
            }
        }
        if (modified) {
            textSurSaut.update();
        }
    }
}

void SurSauteurLayer::draw() {
    ofPushStyle();
    
    if (bHasText) {
        ofEnableAlphaBlending();
        textSurSaut.draw(0, -textSimOffsetY * scale, simWidth * scale, textSurSaut.getHeight() * scale);
    }

    for (auto& p : particles) {
        float alpha = (p.life / p.maxLife) * 255.0f;
        ofSetColor(p.color.r, p.color.g, p.color.b, alpha);
        ofDrawRectangle(p.pos.x * scale, p.pos.y * scale, 2 * scale, 2 * scale);
    }

    for(auto& s : sauteurs) {
        float rx = s.pos.x * scale;
        float ry = s.pos.y * scale;
        float rw = s.xrad * scale * 2; 
        float rh = s.yrad * scale * 2;

        ofFill();
        ofSetColor(255, 100, 50); 
        ofDrawRectangle(rx - rw/2, ry - rh/2, rw, rh);
    }
    ofPopStyle();
}