#include "BalletLayer.h"

//--------------------------------------------------------------
void BalletLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    createParticleTexture();
}

//--------------------------------------------------------------
void BalletLayer::createParticleTexture() {
    // Simulate Dot(2) + BlurFilter(8,8,3)
    // Radius 2 + Blur 8 ~= 10-12px radius visual
    int size = 32;
    float center = size * 0.5f;
    particleImg.allocate(size, size, OF_IMAGE_COLOR_ALPHA);
    
    for(int y=0; y<size; y++) {
        for(int x=0; x<size; x++) {
            float dist = ofDist(x, y, center, center);
            // Soft gaussian-like falloff
            float alpha = 0;
            if(dist < 12) {
                alpha = ofMap(dist, 0, 12, 255, 0, true);
                alpha = pow(alpha / 255.0f, 2.0f) * 255.0f; // Quadratic falloff for softness
            }
            particleImg.setColor(x, y, ofColor(255, 255, 255, alpha));
        }
    }
    particleImg.update();
}

//--------------------------------------------------------------
void BalletLayer::update(float time) {
    if(!bActive) return;

    // 1. Spawn Particles (Steady(2))
    for(int i=0; i<2; i++) {
        BalletParticle p;
        // Position(new RectangleZone(0, 50, WIDTH, HEIGHT-50))
        p.pos.set(ofRandom(0, simWidth), ofRandom(50, simHeight - 50));
        p.vel.set(0, 0);
        
        // Lifetime(10, 10)
        p.maxLife = 10.0f;
        p.life = p.maxLife;
        
        // ScaleAllInit(1, 2)
        p.startScale = ofRandom(1.0f, 2.0f);
        p.scale = p.startScale;
        
        // ColorInit(White)
        p.color = ofColor(255);
        
        particles.push_back(p);
    }

    // 2. Mutual Gravity (N^2 interaction)
    // MutualGravity(50, 100, 5) -> power, maxDist, epsilon
    float maxDistSq = maxDist * maxDist;
    float epsilonSq = epsilon * epsilon;

    for(size_t i=0; i<particles.size(); i++) {
        for(size_t j=i+1; j<particles.size(); j++) {
            ofVec2f dir = particles[j].pos - particles[i].pos;
            float distSq = dir.lengthSquared();
            
            if(distSq < maxDistSq && distSq > epsilonSq) {
                float dist = sqrt(distSq);
                // Force = power / dist^2
                // Applied as acceleration: vel += force * dirNormalized
                // Optimized: vel += dir * (power / (dist * distSq))
                
                float f = gravityPower / (distSq * dist);
                ofVec2f force = dir * f;
                
                particles[i].vel += force;
                particles[j].vel -= force;
            }
        }
    }

    // 3. Update Particles
    for(auto& p : particles) {
        // Move
        p.pos += p.vel;
        
        // Age (Quadratic easeOut for life?) 
        // In Flint Age updates energy. 
        // Let's just decrease life linearly for simplicity, visual difference is subtle for Age action alone.
        p.life -= 1.0f / 60.0f; // Assuming 60fps
        
        // Fade(0, 1) -> Start 0, End 1 (Fade In)
        // Alpha based on life progress
        float progress = 1.0f - (p.life / p.maxLife); // 0 to 1
        
        // Fade(0, 1) means alpha goes from 0 to 1 over lifetime
        float alphaVal = ofMap(progress, 0.0f, 1.0f, 0.0f, 255.0f, true);
        p.color.a = alphaVal;
    }

    // 4. Remove Dead
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const BalletParticle& p) { return p.life <= 0; }), particles.end());
}

//--------------------------------------------------------------
void BalletLayer::draw() {
    if(!bActive) return;
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD); // Additive blending for "glow" effect
    ofSetColor(255);
    
    for(auto& p : particles) {
        ofSetColor(p.color);
        float s = p.scale;
        // Draw centered
        particleImg.draw(p.pos.x - 16 * s, p.pos.y - 16 * s, 32 * s, 32 * s);
    }
    
    ofDisableBlendMode();
    ofPopStyle();
}
