#include "AutoPongLayer.h"
#include <algorithm>

void AutoPongLayer::setup(float w, float h, shared_ptr<ColliderLayer> colliders) {
    simWidth = w;
    simHeight = h;
    colliderLayer = colliders;
    wbar = 160.0f; // Scale x2
    hbar = 20.0f;  // Scale x2
    xbar = w * 0.5f;
    ybar = h - 100.0f;
    vbar = 30.0f; // Vitesse de la barre (Scale x2)

    for(int i = 0; i < 10; i++) appendBall();
}

void AutoPongLayer::appendBall() {
    AutoPongBall b;
    float vr = ofRandom(10.0f, 20.0f); // Scale x2
    float vtheta = ofRandom(-PI * 0.45f, PI * 0.45f);
    if(ofRandom(1.0f) > 0.5f) vtheta += PI;
    b.pos.set(ofRandom(simWidth), ofRandom(100.0f));
    b.vel.set(vr * sin(vtheta), vr * cos(vtheta));
    balls.push_back(b);
}

AutoPongRet AutoPongLayer::solvePath(float cur, float maxv, vector<AutoPongRange>& ranges) {
    if (ranges.empty()) return {cur, cur};

    float mostclose = FLT_MAX;
    for (auto& r : ranges) {
        if (r.t < mostclose) mostclose = r.t;
    }

    float rangemin = cur - maxv * mostclose;
    float rangemax = cur + maxv * mostclose;
    vector<AutoPongEvent> updowns;

    for (auto& r : ranges) {
        float pos = r.maxP + maxv * (r.t - mostclose);
        if (pos >= rangemin && pos <= rangemax) updowns.push_back({pos, -r.w});
        pos = r.minP - maxv * (r.t - mostclose);
        if (pos >= rangemin && pos <= rangemax) updowns.push_back({pos, r.w});
    }

    if (updowns.empty()) return {cur, cur};

    std::sort(updowns.begin(), updowns.end(), [](const AutoPongEvent& a, const AutoPongEvent& b) {
        if (abs(a.pos - b.pos) < 0.0001f) return a.w > b.w; // w décroissant
        return a.pos < b.pos; // pos croissant
    });

    float maxp = 0;
    AutoPongRet maxrange = {rangemin, updowns[0].pos};
    float score = 0;

    for (size_t i = 0; i < updowns.size(); i++) {
        score += updowns[i].w;
        if (maxp < score) {
            maxp = score;
            float nextpos = (i + 1 < updowns.size()) ? updowns[i + 1].pos : rangemax;
            maxrange = {updowns[i].pos, nextpos};
        }
    }

    return maxrange;
}

float AutoPongLayer::algo() {
    vector<AutoPongRange> ranges;
    for (auto& b : balls) {
        if (b.vel.y > 0) {
            float t = ((ybar - hbar / 2.0f) - b.pos.y) / b.vel.y;
            if (t > 0 && t < 500.0f) {
                float px = b.pos.x + t * b.vel.x;
                
                // Wrapping infini au lieu de rebond
                px = fmod(px, simWidth);
                if (px < 0) px += simWidth;
                
                // On duplique les plages pour permettre à la barre de considérer un mouvement à travers les bords (wrap)
                ranges.push_back({t, px - wbar * 0.4f, px + wbar * 0.4f, 1.0f});
                ranges.push_back({t, px - wbar * 0.4f - simWidth, px + wbar * 0.4f - simWidth, 1.0f});
                ranges.push_back({t, px - wbar * 0.4f + simWidth, px + wbar * 0.4f + simWidth, 1.0f});
            }
        }
    }

    AutoPongRet ret = solvePath(xbar, vbar, ranges);
    if (ret.maxP < xbar) return -vbar;
    if (ret.minP > xbar) return vbar;
    return 0;
}

void AutoPongLayer::update(float time) {
    if (frameCount++ == 100) {
        appendBall();
        frameCount = 0;
    }

    xbar += algo();
    
    // Wrapping de la barre
    if (xbar < 0) xbar += simWidth;
    if (xbar > simWidth) xbar -= simWidth;

    for (int i = balls.size() - 1; i >= 0; i--) {
        auto& b = balls[i];
        ofVec2f oldPos = b.pos;
        b.pos += b.vel;

        // Rebond sur les colliders
        if (colliderLayer) {
            float scale = colliderLayer->scale;
            float cx = b.pos.x / scale;
            float cy = b.pos.y / scale;
            
            if (colliderLayer->isWall(cx, cy)) {
                float oldCx = oldPos.x / scale;
                float oldCy = oldPos.y / scale;
                
                bool hitX = colliderLayer->isWall(cx, oldCy);
                bool hitY = colliderLayer->isWall(oldCx, cy);
                
                if (hitX) {
                    b.vel.x = -b.vel.x;
                    b.pos.x = oldPos.x;
                }
                if (hitY) {
                    b.vel.y = -b.vel.y;
                    b.pos.y = oldPos.y;
                }
                if (!hitX && !hitY) { // Rebond parfait sur un coin isolé
                    b.vel.x = -b.vel.x;
                    b.vel.y = -b.vel.y;
                    b.pos = oldPos;
                }
            }
        }

        // Wrapping des balles
        if (b.pos.x < 0) b.pos.x += simWidth;
        if (b.pos.x > simWidth) b.pos.x -= simWidth;
        if (b.pos.y < 0) {
            b.vel.y = -b.vel.y;
            b.pos.y = -b.pos.y;
        }
        if (b.pos.y > simHeight + 100.0f) {
            balls.erase(balls.begin() + i);
            continue;
        }

        // Collision avec la barre (en prenant en compte la distance la plus courte via le wrap)
        float dx = b.pos.x - xbar;
        if (dx > simWidth * 0.5f) dx -= simWidth;
        else if (dx < -simWidth * 0.5f) dx += simWidth;

        if (b.pos.y >= ybar - hbar / 2.0f && b.pos.y - b.vel.y < ybar - hbar / 2.0f && abs(dx) <= wbar / 2.0f) {
            b.vel.y = -b.vel.y;
            b.pos.y = (ybar - hbar / 2.0f) * 2.0f - b.pos.y;
        }
    }
}

void AutoPongLayer::draw() {
    ofPushStyle();
    vector<ofVec2f> offsets = {ofVec2f(0, 0), ofVec2f(simWidth, 0), ofVec2f(-simWidth, 0)};
    for (auto& off : offsets) {
        ofPushMatrix();
        ofTranslate(off);
        ofSetColor(255);
        for (auto& b : balls) ofDrawRectangle(b.pos.x - 6, b.pos.y - 6, 12, 12);
        ofSetColor(0, 255, 0);
        ofDrawRectangle(xbar - wbar / 2.0f, ybar - hbar / 2.0f, wbar, hbar);
        ofPopMatrix();
    }
    ofPopStyle();
}