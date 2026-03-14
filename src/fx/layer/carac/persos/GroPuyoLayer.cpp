#include "GroPuyoLayer.h"

//--------------------------------------------------------------
// GRO PUYO OBJECT
//--------------------------------------------------------------
void GroPuyo::setup(float x, float y) {
    center.set(x, y);
    phaseOffset = ofRandom(1000);
    color.set(ofRandom(200, 255), ofRandom(100, 200), ofRandom(200, 255), 200);

    int numNodes = 120; // Beaucoup de segments pour bien épouser les formes
    float initialRadius = 50.0f;
    float angleStep = TWO_PI / numNodes;

    nodes.reserve(numNodes);
    restLengths.reserve(numNodes);

    for(int i=0; i<numNodes; i++) {
        float angle = i * angleStep;
        GroPuyoNode n;
        n.pos.set(x + cos(angle) * initialRadius, y + sin(angle) * initialRadius);
        n.oldPos = n.pos;
        nodes.push_back(n);
    }

    // Calcul des longueurs de repos du périmètre (Skin)
    for(int i=0; i<numNodes; i++) {
        int next = (i + 1) % numNodes;
        float dist = nodes[i].pos.distance(nodes[next].pos);
        restLengths.push_back(dist);
    }
    
    currentRadius = initialRadius;
}

void GroPuyo::update(float w, float h, shared_ptr<ColliderLayer> collider, float time) {
    if (nodes.empty()) return;

    // 1. Respiration / Gonflement (Target Radius)
    // Oscille entre 50px et 200px (Très gros)
    float breath = sin(time * 2.0f + phaseOffset);
    targetRadius = ofMap(breath, -1.0f, 1.0f, 50.0f, 250.0f);

    // Lissage du rayon actuel
    currentRadius = ofLerp(currentRadius, targetRadius, 0.05f);

    // Recalcul du centre
    center.set(0,0);
    for(auto& n : nodes) center += n.pos;
    center /= (float)nodes.size();

    // Déplacement aléatoire (Wandering)
    float wx = ofSignedNoise(time * 0.3f, phaseOffset) * 0.2f;
    float wy = ofSignedNoise(phaseOffset, time * 0.3f) * 0.2f;

    // 2. Verlet Integration + Forces
    for(auto& n : nodes) {
        ofVec2f vel = (n.pos - n.oldPos) * 0.9f; // Friction assez forte (aspect lourd/visqueux)
        n.oldPos = n.pos;
        
        // Gravité légère
        vel.y += 0.1f;
        vel += ofVec2f(wx, wy); // Application du wandering
        n.pos += vel;
        
        // Force de Pression (Gonflement)
        // Pousse les nœuds vers l'extérieur depuis le centre
        ofVec2f dir = n.pos - center;
        float dist = dir.length();
        if(dist < currentRadius && dist > 0.1f) {
            ofVec2f push = dir.getNormalized();
            // Force proportionnelle à la distance manquante (Ressort interne)
            // Faible rigidité (0.05) pour permettre l'écrasement contre les murs
            n.pos += push * (currentRadius - dist) * 0.05f; 
        }
    }

    // 3. Contraintes de périmètre (La peau ne s'étire pas trop)
    for(int k=0; k<15; k++) solveConstraints();

    // 4. Collisions Murs (ColliderLayer)
    if(collider) {
        for(auto& n : nodes) {
            // Si le point est dans un mur
            if(collider->isWall(n.pos.x, n.pos.y)) {
                // On le repousse vers le centre de la bulle (qui est supposé être dans le vide)
                // C'est ce qui donne l'effet "s'écrase sur le contour"
                ofVec2f escapeDir = (center - n.pos).getNormalized();
                
                // On pousse jusqu'à sortir (itération simple)
                int safety = 0;
                while(collider->isWall(n.pos.x, n.pos.y) && safety < 10) {
                    n.pos += escapeDir * 2.0f;
                    safety++;
                }
                // Friction de contact
                n.oldPos = n.pos; 
            }
        }
    }
    
    // Sol de sécurité
    for(auto& n : nodes) {
        if(n.pos.y > h) n.pos.y = h;
    }

    // Wrapping Horizontal (Boucle infinie)
    if(center.x > w + currentRadius) {
        float shift = w;
        for(auto& n : nodes) { n.pos.x -= shift; n.oldPos.x -= shift; }
        center.x -= shift;
    } else if(center.x < -currentRadius) {
        float shift = w;
        for(auto& n : nodes) { n.pos.x += shift; n.oldPos.x += shift; }
        center.x += shift;
    }
}

void GroPuyo::solveConstraints() {
    for(size_t i=0; i<nodes.size(); i++) {
        int next = (i + 1) % nodes.size();
        GroPuyoNode& p1 = nodes[i];
        GroPuyoNode& p2 = nodes[next];
        
        ofVec2f delta = p1.pos - p2.pos;
        float dist = delta.length();
        if(dist == 0) continue;
        
        // On essaie de maintenir la distance de repos initiale (peau élastique mais ferme)
        // Mais on autorise un peu d'étirement car le rayon change beaucoup (50 -> 250)
        // Donc on recalcule dynamiquement une distance cible basée sur le rayon actuel vs initial
        // Approximation : périmètre = 2*PI*R
        float scaleFactor = currentRadius / 50.0f; // 50.0 est le rayon setup
        float targetLen = restLengths[i] * scaleFactor;

        float diff = (dist - targetLen) / dist;
        ofVec2f offset = delta * 0.5f * diff * 0.8f; // 0.8 stiffness
        
        p1.pos -= offset;
        p2.pos += offset;
    }
}

void GroPuyo::draw() {
    ofSetColor(color);
    ofFill();
    ofBeginShape();
    for(auto& n : nodes) ofVertex(n.pos);
    ofVertex(nodes[0].pos);
    ofEndShape();
    
    ofNoFill();
    ofSetColor(255, 255, 255, 200);
    ofSetLineWidth(3);
    ofBeginShape();
    for(auto& n : nodes) ofVertex(n.pos);
    ofVertex(nodes[0].pos);
    ofEndShape();
}

//--------------------------------------------------------------
// LAYER MANAGER
//--------------------------------------------------------------
void GroPuyoLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w; simHeight = h; scale = s; collider = col;
}
void GroPuyoLayer::addGroPuyo(float x, float y) {
    auto p = make_shared<GroPuyo>();
    p->setup(x, y);
    puyos.push_back(p);
}
void GroPuyoLayer::update(float mx, float my, float time) {
    for(auto& p : puyos) p->update(simWidth, simHeight, collider, time);
    
    // Collisions entre GroPuyos (Déformation mutuelle)
    for(size_t i=0; i<puyos.size(); i++) {
        for(size_t j=i+1; j<puyos.size(); j++) {
            auto& p1 = puyos[i];
            auto& p2 = puyos[j];
            
            ofVec2f diff = p1->center - p2->center;
            // Correction pour le wrapping (distance la plus courte)
            if(diff.x > simWidth * 0.5f) diff.x -= simWidth;
            else if(diff.x < -simWidth * 0.5f) diff.x += simWidth;
            
            float dist = diff.length();
            float minDist = p1->currentRadius + p2->currentRadius;
            
            if(dist < minDist) {
                ofVec2f dir = (dist > 0.1f) ? diff.getNormalized() : ofVec2f(1, 0);
                
                // 1. Répulsion globale des centres
                float overlap = minDist - dist;
                ofVec2f force = dir * overlap * 0.02f; 
                
                for(auto& n : p1->nodes) n.pos += force;
                for(auto& n : p2->nodes) n.pos -= force;
                
                // 2. Déformation locale (Nodes repoussés par le volume de l'autre)
                for(auto& n : p1->nodes) {
                    float d = n.pos.distance(p2->center);
                    if(d < p2->currentRadius) {
                        n.pos += (n.pos - p2->center).getNormalized() * (p2->currentRadius - d) * 0.2f;
                    }
                }
                for(auto& n : p2->nodes) {
                    float d = n.pos.distance(p1->center);
                    if(d < p1->currentRadius) {
                        n.pos += (n.pos - p1->center).getNormalized() * (p1->currentRadius - d) * 0.2f;
                    }
                }
            }
        }
    }
}
void GroPuyoLayer::draw() {
    ofPushMatrix();
    ofScale(scale, scale);
    for(auto& p : puyos) {
        p->draw();
        
        // Dessin des fantômes pour le wrapping visuel
        if(p->center.x < p->currentRadius + 100) {
            ofPushMatrix();
            ofTranslate(simWidth, 0);
            p->draw();
            ofPopMatrix();
        } else if(p->center.x > simWidth - (p->currentRadius + 100)) {
            ofPushMatrix();
            ofTranslate(-simWidth, 0);
            p->draw();
            ofPopMatrix();
        }
    }
    ofPopMatrix();
}