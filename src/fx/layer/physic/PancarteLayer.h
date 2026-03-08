#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

// Vertex pour la simulation physique
struct PVtx {
    ofVec2f pos;
    ofVec2f vel;
    float mass; // 0 = fixe, >0 = mobile (sert aussi de facteur de mobilité)
    
    PVtx(float x, float y, float m) : pos(x,y), vel(0,0), mass(m) {}
    
    void update() {
        if (mass > 0) {
            pos += vel;
            vel.y += 0.5f; // Gravité
            vel *= 0.98f;  // Friction de l'air
        }
    }
};

// Ressort / Contrainte
struct PSpr {
    int i1, i2;
    float restLength;
    bool rigid;
    bool visible;
    
    PSpr(int a, int b, float len, bool r, bool v) : i1(a), i2(b), restLength(len), rigid(r), visible(v) {}
};

class PancarteLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY, float time) override;
    void draw() override;
    
    void mousePressed(float x, float y, int button) override;
    void mouseReleased(float x, float y, int button) override;

private:
    float simWidth, simHeight;
    
    vector<shared_ptr<PVtx>> vertices;
    vector<PSpr> springs;
    
    // Contrôle de l'ancrage
    ofVec2f anchorPos;
    bool isDragging = false;
    
    void solveSprings();
};