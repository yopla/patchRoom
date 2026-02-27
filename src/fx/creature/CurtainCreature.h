#pragma once
#include "ofMain.h"

struct CurtainPoint {
    glm::vec2 pos;
    glm::vec2 oldPos;
    glm::vec2 pinPos;
    bool bPinned;
    glm::vec2 texCoord;

    CurtainPoint(float x, float y, float u, float v) {
        pos = glm::vec2(x, y);
        oldPos = pos;
        pinPos = pos;
        bPinned = false;
        texCoord = glm::vec2(u, v);
    }

    void pin(float px, float py) {
        pinPos = glm::vec2(px, py);
        bPinned = true;
        pos = pinPos;
        oldPos = pinPos;
    }
};

struct CurtainConstraint {
    CurtainPoint* p1;
    CurtainPoint* p2;
    float restLength;

    CurtainConstraint(CurtainPoint* a, CurtainPoint* b) {
        p1 = a;
        p2 = b;
        restLength = glm::distance(p1->pos, p2->pos);
    }

    void resolve();
};

class CurtainCreature {
public:
    void setup(float x, float y, float w, float h, string imgPath);
    void update(float mx, float my);
    void draw();
    
    void mousePressed(float mx, float my);
    void mouseReleased(float mx, float my);
    
    // Simulation params
    int cols = 40;
    int rows = 30;
    float gravity = 0.6f;
    float friction = 0.98f;
    float mouseInfluenceSize = 80.0f;
    
    bool isDragging = false;
    glm::vec2 lastMouse;
    
    ofImage texture;
    vector<CurtainPoint> points;
    vector<CurtainConstraint> constraints;
    ofMesh mesh;
    
    float startX, startY, width, height;
};
