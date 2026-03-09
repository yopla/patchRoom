#include "FireALayer.h"

//--------------------------------------------------------------
void FireALayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // FBO pour l'effet de traînée (Trail)
    fbo.allocate(w, h, GL_RGBA);
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
    createFlameTextures();
}

//--------------------------------------------------------------
void FireALayer::createFlameTextures() {
    int size = 40;
    int numImages = 20;
    
    for(int i=0; i<numImages; i++) {
        ofFbo temp;
        temp.allocate(size, size, GL_RGBA);
        temp.begin();
        ofClear(0,0,0,0);
        
        // 1. Gradient Radial (Blanc -> Jaune -> Orange -> Rouge -> Transparent)
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        mesh.addVertex(ofVec3f(size/2, size/2, 0));
        mesh.addColor(ofColor(255, 255, 200, 255)); // Centre clair
        
        int segs = 30;
        for(int k=0; k<=segs; k++) {
            float ang = k * TWO_PI / segs;
            float rad = size * 0.45;
            mesh.addVertex(ofVec3f(size/2 + cos(ang)*rad, size/2 + sin(ang)*rad, 0));
            mesh.addColor(ofColor(200, 50, 0, 0)); // Bord rouge transparent
        }
        mesh.draw();
        temp.end();
        
        // 2. Displacement Map (Simulation du filtre AS3)
        ofPixels pix;
        temp.readToPixels(pix);
        ofPixels newPix = pix;
        
        float offset = ofRandom(100);
        float scale = 0.15;
        
        for(int y=0; y<size; y++) {
            for(int x=0; x<size; x++) {
                // Bruit de Perlin pour déformer le cercle
                float n = ofNoise(x*scale, y*scale, offset);
                float dx = (n-0.5) * 12.0;
                float dy = (n-0.5) * 12.0;
                int sx = ofClamp(x+dx, 0, size-1);
                int sy = ofClamp(y+dy, 0, size-1);
                newPix.setColor(x,y, pix.getColor(sx, sy));
            }
        }
        
        ofTexture tex;
        tex.allocate(newPix);
        flameTextures.push_back(tex);
    }
}

//--------------------------------------------------------------
void FireALayer::update(float mx, float my) {
    // 1. Spawning
    if(isMousePressed) {
        pressTimer++;
        spawnParticles(mx, my);
    } else {
        pressTimer = -1;
    }
    
    // 2. Update Particles
    for(auto& p : particles) {
        p.life -= p.decay;
        p.pos += p.vel;
        
        // Comportement selon le type (inspiré des Tweens AS3)
        if(p.type == 0) { // SmallFlame
            p.scaleY *= 0.95; 
            p.color.a = p.life * 255;
        } else if(p.type == 1) { // LargeFlame
            p.scaleX += 0.05;
            p.scaleY += 0.1;
            p.color.a = p.life * 255;
            // Changement de couleur vers le rouge/sombre
            if(p.life < 0.5) p.color.setHue(10); 
        } else if(p.type == 2) { // Smoke
            p.pos.y -= 1.5; // Monte
            p.scaleX += 0.03;
            p.scaleY += 0.03;
            p.color.a = p.life * 100;
        } else if(p.type == 3) { // Explosion
            p.scaleX += 0.1;
            p.scaleY += 0.1;
            p.color.a = p.life * 255;
        }
    }
    
    // Suppression des particules mortes
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const FireParticle& p){ return p.life <= 0; }), particles.end());
        
    // 3. Dessin dans le FBO (Trail effect)
    fbo.begin();
    
    // Fade (BlurFilter simulé par transparence)
    ofSetColor(0, 0, 0, 60); // Alpha contrôle la longueur de la traînée
    ofDrawRectangle(0, 0, simWidth, simHeight);
    
    // Dessin des particules
    for(auto& p : particles) {
        if(p.imgIndex < flameTextures.size()) {
            ofSetColor(p.color);
            ofEnableBlendMode(p.blendMode);
            
            ofPushMatrix();
            ofTranslate(p.pos);
            ofRotateRad(p.angle);
            ofScale(p.scaleX, p.scaleY);
            
            flameTextures[p.imgIndex].draw(-20, -20); // Centre (40x40)
            
            ofPopMatrix();
        }
    }
    ofDisableBlendMode();
    fbo.end();
    
    lastMouse.set(mx, my);
}

//--------------------------------------------------------------
void FireALayer::spawnParticles(float x, float y) {
    // Direction aléatoire ou basée sur le mouvement
    float angle = ofRandom(TWO_PI);
    float speed = ofRandom(2, 6);
    
    FireParticle p;
    p.pos.set(x, y);
    p.life = 1.0;
    p.imgIndex = (int)ofRandom(flameTextures.size());
    p.angle = ofRandom(TWO_PI);
    
    if (pressTimer > 14) {
        // Large Smoke + Large Flame
        p.type = 2; // Smoke
        p.decay = 0.015;
        p.scaleX = 1.5; p.scaleY = 1.5;
        p.vel.set(cos(angle)*speed*0.5, sin(angle)*speed*0.5 - 2);
        p.color = ofColor(150, 150, 150);
        p.blendMode = OF_BLENDMODE_SCREEN;
        particles.push_back(p);
        
        p.type = 1; // Large Flame
        p.decay = 0.03;
        p.scaleX = 1.0; p.scaleY = 1.0;
        p.vel.set(cos(angle)*speed, sin(angle)*speed);
        p.color = ofColor(255, 200, 50);
        p.blendMode = OF_BLENDMODE_ADD;
        particles.push_back(p);
        
    } else {
        // Small Flame
        p.type = 0;
        p.decay = 0.05;
        p.scaleX = 1.0; p.scaleY = 1.0;
        p.vel.set(cos(angle)*speed*0.5, sin(angle)*speed*0.5);
        p.color = ofColor(255, 120, 0);
        p.blendMode = OF_BLENDMODE_ADD;
        particles.push_back(p);
    }
}

//--------------------------------------------------------------
void FireALayer::draw() {
    ofSetColor(255);
    fbo.draw(0,0);
}

void FireALayer::mousePressed(float x, float y) { isMousePressed = true; }
void FireALayer::mouseReleased(float x, float y) { isMousePressed = false; }