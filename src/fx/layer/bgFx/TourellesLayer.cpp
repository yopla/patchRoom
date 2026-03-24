#include "TourellesLayer.h"
#include <algorithm>

void TourellesLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // Densité fidèle à l'AS3 : ~1 batterie tous les 150 pixels
    int numBatteries = simWidth / 150.0f; 
    
    for(int i = 0; i < numBatteries; i++) {
        TourelleBattery b;
        b.pos.set(75.0f + i * 150.0f, h - 40);
        b.theta = -HALF_PI; // Regarde vers le haut
        b.vBullet = 30.0f; // Scale x2 par rapport à l'original (15.0f)
        b.omega = 0.1f;
        b.algoType = (i % 2 == 0) ? 1 : 2; // Alterne les comportements (Destruction vs Rapidité)
        batteries.push_back(b);
    }
}

float TourellesLayer::getShortestDist(float current, float target, float w) {
    float dx = target - current;
    if (dx > w * 0.5f) dx -= w;
    else if (dx < -w * 0.5f) dx += w;
    return dx;
}

void TourellesLayer::shoot(const TourelleBattery& bat) {
    TourelleBullet bullet;
    float c = cos(bat.theta);
    float s = sin(bat.theta);
    bullet.pos = bat.pos + ofVec2f(c, s) * 40.0f;
    bullet.vel.set(c * bat.vBullet, s * bat.vBullet);
    bullet.radius = 10.0f;
    bullet.active = true;
    bullets.push_back(bullet);
}

struct ScoredMeteor {
    TourelleMeteor* m;
    float mx;
    float distSq;
};

// Algo 1 : "algoFastestDestruction" (Traduction directe des mathématiques de l354.as)
void TourellesLayer::algoFastestDestruction(TourelleBattery& bat) {
    int minu = 80;
    int mint = 999;
    int minsg = 0;
    TourelleMeteor* targ = nullptr;
    
    vector<ScoredMeteor> closeMeteors;
    for (auto& m : meteors) {
        if (m.locked || !m.active || m.pos.y > bat.pos.y) continue;
        float dx = getShortestDist(bat.pos.x, m.pos.x, simWidth);
        if (abs(dx) > 1200.0f) continue;
        float dy = m.pos.y - bat.pos.y;
        closeMeteors.push_back({&m, bat.pos.x + dx, dx*dx + dy*dy});
    }
    
    sort(closeMeteors.begin(), closeMeteors.end(), [](const ScoredMeteor& a, const ScoredMeteor& b) { return a.distSq < b.distSq; });
    
    int checkLimit = std::min(5, (int)closeMeteors.size());
    for (int i = 0; i < checkLimit; i++) {
        auto* m = closeMeteors[i].m;
        float mx = closeMeteors[i].mx;
        
        for (int sg = -1; sg <= 1; sg++) {
            float omega = bat.omega * sg;
            for (int t = 0; t <= minu; t++) {
                float c = cos(bat.theta + omega * t);
                float s = sin(bat.theta + omega * t);
                float Ax = -(-bat.vBullet * c + m->vel.x);
                float Ay = -(G * t - bat.vBullet * s + m->vel.y);
                float Bx = -bat.vBullet * c * t + bat.pos.x + 40.0f * c - mx;
                float By = G * t * t / 2.0f - bat.vBullet * s * t + bat.pos.y + 40.0f * s - m->pos.y;
                
                float ip = Ax * Bx + Ay * By;
                if (ip > 0) continue;
                
                float cr2 = m->radius * m->radius;
                float D = ip * ip - (Ax * Ax + Ay * Ay) * (Bx * Bx + By * By - cr2);
                if (D < 0) continue;
                
                int u = ceil((-ip + sqrt(D)) / (Ax * Ax + Ay * Ay));
                if (t <= u && u < minu) {
                    minu = u;
                    mint = t;
                    minsg = sg;
                    targ = m;
                }
            }
        }
    }
    
    if (mint <= 0 && targ) {
        targ->locked = true;
        shoot(bat);
    } else if (targ) {
        bat.theta += bat.omega * minsg;
    }
}

// Algo 2 : "algoFastestShoot"
void TourellesLayer::algoFastestShoot(TourelleBattery& bat) {
    int mint = 80;
    int minsg = 0;
    TourelleMeteor* targ = nullptr;
    
    vector<ScoredMeteor> closeMeteors;
    for (auto& m : meteors) {
        if (m.locked || !m.active || m.pos.y > bat.pos.y) continue;
        float dx = getShortestDist(bat.pos.x, m.pos.x, simWidth);
        if (abs(dx) > 1200.0f) continue;
        float dy = m.pos.y - bat.pos.y;
        closeMeteors.push_back({&m, bat.pos.x + dx, dx*dx + dy*dy});
    }
    
    sort(closeMeteors.begin(), closeMeteors.end(), [](const ScoredMeteor& a, const ScoredMeteor& b) { return a.distSq < b.distSq; });
    
    int checkLimit = std::min(5, (int)closeMeteors.size());
    for (int i = 0; i < checkLimit; i++) {
        auto* m = closeMeteors[i].m;
        float mx = closeMeteors[i].mx;
        
        for (int sg = -1; sg <= 1; sg++) {
            float omega = bat.omega * sg;
            for (int t = 0; t <= mint; t++) {
                float c = cos(bat.theta + omega * t);
                float s = sin(bat.theta + omega * t);
                float Ax = -(-bat.vBullet * c + m->vel.x);
                float Ay = -(G * t - bat.vBullet * s + m->vel.y);
                float Bx = -bat.vBullet * c * t + bat.pos.x + 40.0f * c - mx;
                float By = G * t * t / 2.0f - bat.vBullet * s * t + bat.pos.y + 40.0f * s - m->pos.y;
                
                float ip = Ax * Bx + Ay * By;
                if (ip > 0) continue;
                
                float cr2 = m->radius * m->radius;
                float D = ip * ip - (Ax * Ax + Ay * Ay) * (Bx * Bx + By * By - cr2);
                if (D < 0) continue;
                
                if (t < mint) {
                    mint = t;
                    minsg = sg;
                    targ = m;
                    break;
                }
            }
        }
    }
    
    if (mint <= 0 && targ) {
        targ->locked = true;
        shoot(bat);
    } else if (targ) {
        bat.theta += bat.omega * minsg;
    }
}

void TourellesLayer::update(float time) {
    frameCount++;
    
    // Apparition des météores (Très intense pour compenser la largeur immense)
    int numMeteors = ofRandom(1, 4); // 1 à 3 par frame !
    for(int i = 0; i < numMeteors; i++) {
        TourelleMeteor m;
        m.pos.set(ofRandom(simWidth), -50);
        m.vel.set(ofRandom(-3.0f, 3.0f), ofRandom(-2.0f, 2.0f));
        m.radius = 20.0f; // Scale x2
        m.rot = 0.0f;
        m.omega = ofRandom(-0.2f, 0.2f);
        m.active = true;
        m.locked = false;
        meteors.push_back(m);
    }
    
    // Processus de ciblage et rotation des batteries
    for (auto& bat : batteries) {
        if (bat.algoType == 1) algoFastestDestruction(bat);
        else if (bat.algoType == 2) algoFastestShoot(bat);
    }
    
    // Mise à jour Météores
    for (auto& m : meteors) {
        m.vel.y += G; // Gravité
        m.pos += m.vel;
        m.rot += m.omega;
        if (m.pos.x < 0) m.pos.x += simWidth;
        if (m.pos.x > simWidth) m.pos.x -= simWidth;
        if (m.pos.y > simHeight) { // Déclenche l'impact pile au niveau du sol
            m.active = false;
            GroundImpact imp;
            imp.pos = ofVec2f(m.pos.x, simHeight);
            imp.t = 0;
            imp.maxT = 60.0f; // Durée de l'effet
            for(int i = 0; i < 30; i++) { // Crée 30 débris explosifs
                imp.sparksVel.push_back(ofVec2f(ofRandom(-15, 15), ofRandom(-25, -5)));
            }
            impacts.push_back(imp);
        }
    }
    
    // Mise à jour Balles
    for (auto& b : bullets) {
        b.vel.y += G; // Gravité sur les balles aussi ! (Fidèle à Bullet.step() as3)
        b.pos += b.vel;
        if (b.pos.x < 0) b.pos.x += simWidth;
        if (b.pos.x > simWidth) b.pos.x -= simWidth;
        if (b.pos.y < -100 || b.pos.y > simHeight + 50) b.active = false;
        
        // Collisions
        for (auto& m : meteors) {
            if (!m.active || !b.active) continue;
            
            float dx = getShortestDist(b.pos.x, m.pos.x, simWidth);
            float dy = m.pos.y - b.pos.y;
            float distSq = dx*dx + dy*dy;
            float rSum = m.radius + b.radius;
            
            if (distSq < rSum * rSum) {
                m.active = false;
                b.active = false;
                TourelleExplosion e;
                e.pos = m.pos;
                e.t = 0;
                explosions.push_back(e);
                break;
            }
        }
    }
    
    // Mise à jour Explosions
    for (auto& e : explosions) {
        e.t++;
    }
    
    // Mise à jour Impacts au sol
    for (auto& imp : impacts) {
        imp.t++;
    }
    
    // Nettoyage
    meteors.erase(remove_if(meteors.begin(), meteors.end(), [](const TourelleMeteor& m) { return !m.active; }), meteors.end());
    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const TourelleBullet& b) { return !b.active; }), bullets.end());
    explosions.erase(remove_if(explosions.begin(), explosions.end(), [](const TourelleExplosion& e) { return e.t >= 30; }), explosions.end());
    impacts.erase(remove_if(impacts.begin(), impacts.end(), [](const GroundImpact& imp) { return imp.t >= imp.maxT; }), impacts.end());
}

void TourellesLayer::draw() {
    ofPushStyle();
    
    vector<ofVec2f> offsets = {ofVec2f(0, 0), ofVec2f(simWidth, 0), ofVec2f(-simWidth, 0)};
    
    for (auto& offset : offsets) {
        ofPushMatrix();
        ofTranslate(offset);
        
        // Dessin Batteries (0x999999)
        for (auto& bat : batteries) {
            ofSetColor(153, 153, 153); 
            ofFill();
            ofDrawCircle(bat.pos, 20); // x2 origin
            
            ofPushMatrix();
            ofTranslate(bat.pos);
            ofRotateRad(bat.theta);
            ofDrawRectangle(10, -4, 30, 8); // Tête de canon
            ofPopMatrix();
        }
        
        // Dessin Météores (0x993333)
        ofSetColor(153, 51, 51);
        for (auto& m : meteors) {
            ofFill();
            ofPushMatrix();
            ofTranslate(m.pos);
            ofRotateRad(m.rot);
            ofDrawCircle(0, 0, m.radius);
            ofPopMatrix();
        }
        
        // Dessin Balles (0xcccccc)
        ofSetColor(204, 204, 204);
        for (auto& b : bullets) {
            ofFill();
            ofPushMatrix();
            ofTranslate(b.pos);
            ofRotateRad(atan2(b.vel.y, b.vel.x));
            ofDrawRectangle(-10, -4, 20, 8);
            ofPopMatrix();
        }
        
        // Dessin Explosions (Red/White frame based)
        for (auto& e : explosions) {
            ofSetColor(255, 0, 0); // 0xff0000
            ofDrawCircle(e.pos, (e.t + 3) * 2.0f);
            ofSetColor(255, 255, 255); // 0xffffff
            ofDrawCircle(e.pos, (e.t + 1) * 2.0f);
        }
        
        // Dessin Impacts massifs au sol
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        for (auto& imp : impacts) {
            float progress = (float)imp.t / imp.maxT;
            float invProg = 1.0f - progress;
            float easeOut = 1.0f - pow(invProg, 3.0f); // Déploiement très rapide au début, puis ralentit
            
            // Onde de choc principale
            ofSetColor(255, 100, 50, 255 * invProg);
            ofSetLineWidth(15.0f * invProg);
            ofNoFill();
            ofDrawCircle(imp.pos, easeOut * 400.0f);
            
            // Onde de choc secondaire (plus claire et plus petite)
            ofSetColor(255, 200, 100, 150 * invProg);
            ofSetLineWidth(5.0f * invProg);
            ofDrawCircle(imp.pos, easeOut * 250.0f);
            
            // Flash lumineux central hyper brillant
            ofFill();
            ofSetColor(255, 255, 200, 255 * pow(invProg, 4.0f));
            ofDrawCircle(imp.pos, 150.0f * invProg);
            
            // Particules / Débris propulsés avec la gravité
            ofSetLineWidth(4.0f);
            ofSetColor(255, 200, 100, 255 * invProg);
            for (auto& vel : imp.sparksVel) {
                ofVec2f sPos = imp.pos + vel * imp.t;
                sPos.y += 0.5f * G * imp.t * imp.t; // Applique la même gravité (G) que les météores
                ofDrawLine(sPos, sPos - vel * 2.0f);
            }
        }
        ofSetLineWidth(1.0f);
        ofEnableAlphaBlending(); // Rétablit le blending standard
        
        ofPopMatrix();
    }
    
    ofPopStyle();
}