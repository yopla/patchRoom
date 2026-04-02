#pragma once
#include "ofMain.h"
#include <vector>
#include <memory>
#include <algorithm>

struct JpUnit {
    int hp;
    int team;
    int spec; // 1 = base, 0 = unité
    int spawnPriority; // Priorité absolue de génération
    ofVec3f pos;
    ofVec3f vel;
    long long reload;
    long long dmgTime;
    long long spawnTimer;
    ofColor color;
    std::weak_ptr<JpUnit> targ;
};

class JupyterBox {
public:
    void setup(float w, float h, float d);
    void reset();
    void update();
    void draw();

    float width, height, depth;
    long long gt; // Timer global
    std::vector<std::shared_ptr<JpUnit>> units;
    bool bPaused = false;
};