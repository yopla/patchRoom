#include "WhaaLayer.h"

//--------------------------------------------------------------
void WhaaLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    balls.clear();
    
    int numBalls = 10;
    ofVec2f center(w * 0.5f, h * 0.5f);
    
    // Création des balles
    for(int i=0; i<numBalls; i++) {
        auto b = make_shared<WhaaBall>();
        // Positionnement en cercle pour commencer proprement
        float angle = ofMap(i, 0, numBalls, 0, TWO_PI);
        b->pos = center + ofVec2f(cos(angle), sin(angle)) * 150.0f;
        b->vel.set(ofRandom(-2, 2), ofRandom(-2, 2));
        
        // Variation de taille et couleur comme dans la ref visuelle implicite
        b->radius = ofRandom(20, 40);
        b->mass = b->radius * 0.1f; 
        b->color = ofColor::fromHsb(ofRandom(0, 50), 200, 255); // Tons chauds/rouges
        
        balls.push_back(b);
    }
    
    // Chaînage : i suit i-1. Le premier suit le dernier (Boucle fermée)
    for(int i=0; i<numBalls; i++) {
        if (i == 0) balls[i]->target = balls.back().get();
        else balls[i]->target = balls[i-1].get();
    }
}

//--------------------------------------------------------------
void WhaaLayer::update(float mouseX, float mouseY) {
    
    // Gestion du Drag souris
    if(draggedBall) {
        draggedBall->pos.set(mouseX, mouseY);
        draggedBall->vel.set(0,0);
    }

    for(auto& b : balls) {
        if(b.get() == draggedBall) continue;

        // 1. Force de ressort vers la cible (Target)
        if(b->target) {
            ofVec2f dir = b->target->pos - b->pos;
            float dist = dir.length();
            
            // Distance de repos = somme des rayons (pour qu'elles se touchent juste)
            float restLength = b->radius + b->target->radius;
            
            // Loi de Hooke : F = k * (dist - rest)
            // On ne tire que si on est plus loin que la distance de repos (comme un élastique mou)
            if(dist > restLength) {
                ofVec2f force = dir.getNormalized() * (dist - restLength) * springK;
                b->vel += force / b->mass;
                // Action-Réaction (optionnel, mais stabilise la boucle)
                if(b->target != draggedBall) {
                    b->target->vel -= force / b->target->mass;
                }
            }
        }
        
        // 2. Répulsion entre toutes les balles (Collisions simples)
        for(auto& other : balls) {
            if(b == other) continue;
            ofVec2f dir = b->pos - other->pos;
            float dist = dir.length();
            float minDist = b->radius + other->radius;
            
            if(dist < minDist && dist > 0) {
                // Force de répulsion exponentielle ou simple correction de position
                ofVec2f push = dir.getNormalized() * (minDist - dist) * 0.2f; // 0.2 = rigidité collision
                b->vel += push;
            }
        }
        
        // 3. Gravité légère (optionnel, pour faire tomber la chaîne)
        // b->vel.y += 0.1f; 

        // 4. Intégration
        b->pos += b->vel;
        b->vel *= friction;
        
        // 5. Murs
        if(b->pos.x < b->radius) { b->pos.x = b->radius; b->vel.x *= -0.8; }
        if(b->pos.x > simWidth - b->radius) { b->pos.x = simWidth - b->radius; b->vel.x *= -0.8; }
        if(b->pos.y < b->radius) { b->pos.y = b->radius; b->vel.y *= -0.8; }
        if(b->pos.y > simHeight - b->radius) { b->pos.y = simHeight - b->radius; b->vel.y *= -0.8; }
    }
}

//--------------------------------------------------------------
void WhaaLayer::draw() {
    ofPushStyle();
    ofSetLineWidth(3);
    
    // Dessin des liens
    ofSetColor(255, 100);
    for(auto& b : balls) {
        if(b->target) ofDrawLine(b->pos, b->target->pos);
    }
    
    // Dessin des balles
    for(auto& b : balls) {
        ofSetColor(b->color);
        ofDrawCircle(b->pos, b->radius);
        
        // Petit reflet pour le volume
        ofSetColor(255, 150);
        ofDrawCircle(b->pos.x - b->radius*0.3, b->pos.y - b->radius*0.3, b->radius * 0.25f);
        
        // Debug ID
        // ofSetColor(0); ofDrawBitmapString(ofToString(b->radius), b->pos);
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void WhaaLayer::mousePressed(float x, float y) {
    float bestDist = 10000.0f;
    draggedBall = nullptr;
    
    for(auto& b : balls) {
        float d = ofDist(x, y, b->pos.x, b->pos.y);
        if(d < b->radius * 1.5f && d < bestDist) {
            bestDist = d;
            draggedBall = b.get();
        }
    }
}

void WhaaLayer::mouseReleased(float x, float y) {
    draggedBall = nullptr;
}