#pragma once
#include "ofMain.h"

class FireBLayer {
private:
    struct Particle {
        ofVec2f pos;
        ofVec2f vel;
        float energy;

        Particle(float x, float y, float v, float angle, float en) {
            pos.set(x, y);
            vel.set(cos(ofDegToRad(angle)) * v, sin(ofDegToRad(angle)) * v);
            energy = en;
        }

        void move() {
            pos += vel;
            energy *= 0.9f;
        }
    };

    class Burst {
    public:
        Burst(float x, float y);
        void update();
        void draw();
        bool isDead() const { return particles.empty(); }

    private:
        void createParticles();

        ofFbo fbo;
        ofPoint position; // position of the burst on screen
        vector<Particle> particles;
    };

public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time);
    void draw();
    void mousePressed(float x, float y, int button);

private:
    float simWidth, simHeight;
    std::list<shared_ptr<Burst>> bursts;
};