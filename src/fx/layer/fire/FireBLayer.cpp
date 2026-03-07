#include "FireBLayer.h"

// --- Burst implementation ---
FireBLayer::Burst::Burst(float x, float y) {
    position.set(x, y);
    fbo.allocate(200, 200, GL_RGBA);
    fbo.begin();
    ofClear(0, 0, 0, 0);
    fbo.end();

    createParticles();
}

void FireBLayer::Burst::createParticles() {
    for (int i = 0; i < 50; i++) {
        float x = fbo.getWidth() / 2;
        float y = fbo.getHeight() / 2;
        float v = ofRandom(5);
        float angle = ofRandom(360);
        particles.emplace_back(x, y, v, angle, 1.0f);
    }
}

void FireBLayer::Burst::update() {
    for (auto& p : particles) {
        p.move();
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) {
            return p.energy < 0.01;
        }), particles.end());
}

void FireBLayer::Burst::draw() {
    fbo.begin();
    ofPushStyle();
    ofSetColor(0, 0, 0, 32);
    ofFill();
    ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
    ofPopStyle();

    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofPushStyle();
    ofColor particleColor = ofColor::fromHex(0xFF8022);

    for (const auto& p : particles) {
        ofSetColor(particleColor, 128 * p.energy);
        ofDrawCircle(p.pos.x, p.pos.y, 10);
    }
    ofPopStyle();
    ofDisableBlendMode();
    fbo.end();

    fbo.draw(position.x - fbo.getWidth()/2, position.y - fbo.getHeight()/2);
}

// --- FireBLayer implementation ---

void FireBLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
}

void FireBLayer::update(float mouseX, float mouseY, float time) {
    for (auto& b : bursts) {
        b->update();
    }
    bursts.remove_if([](const shared_ptr<Burst>& b) {
        return b->isDead();
    });
}

void FireBLayer::draw() {
    for (auto& b : bursts) {
        b->draw();
    }
}

void FireBLayer::mousePressed(float x, float y, int button) {
    bursts.push_back(make_shared<Burst>(x, y));
}