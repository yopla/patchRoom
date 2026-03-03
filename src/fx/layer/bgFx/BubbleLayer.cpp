#include "BubbleLayer.h"

//--------------------------------------------------------------
// BUBBLE OBJECT
//--------------------------------------------------------------
void Bubble::setup(float x, float y, float r) {
    radius = r;
    center.set(x, y);
    noiseOffset = ofRandom(1000);
    // Couleur blanche transparente
    color.set(255, 255, 255, 100); 

    int numNodes = 32; // Moins de noeuds pour plus de stabilité sur les petites tailles
    float angleStep = TWO_PI / numNodes;

    nodes.reserve(numNodes + 1);
    for(int i=0; i<numNodes; i++) {
        float angle = i * angleStep;
        BubbleNode n;
        n.pos.set(x + cos(angle) * r, y + sin(angle) * r);
        n.oldPos = n.pos;
        nodes.push_back(n);
    }
    // Noeud central pour maintenir le volume
    BubbleNode c;
    c.pos.set(x, y);
    c.oldPos = c.pos;
    nodes.push_back(c);
    int centerIdx = numNodes;

    // Links (Ressorts)
    // 1. Contour
    for(int i=0; i<numNodes; i++) {
        BubbleLink l;
        l.n1 = i;
        l.n2 = (i + 1) % numNodes;
        l.length = nodes[l.n1].pos.distance(nodes[l.n2].pos);
        l.stiffness = 0.9f; // Plus rigide pour éviter le repliement
        links.push_back(l);
    }
    // 2. Rayons (Structure interne)
    for(int i=0; i<numNodes; i++) {
        BubbleLink l;
        l.n1 = i;
        l.n2 = centerIdx;
        l.length = radius;
        l.stiffness = 0.4f; // Structure interne renforcée
        links.push_back(l);
    }
}

void Bubble::update(float w, float h, shared_ptr<ColliderLayer> collider, float yOffset_sim, float time) {
    if (nodes.size() < 2) return; // Sécurité critique (évite div/0 et crash accès mémoire)

    // Verlet Integration
    center.set(0,0);
    int count = 0;
    
    for(auto& n : nodes) {
        ofVec2f vel = (n.pos - n.oldPos) * 0.99f; // Friction
        n.oldPos = n.pos;
        
        // Gravité inversée (Ça monte !)
        vel.y -= 0.0f; 
        
        // Petit mouvement brownien / turbulence
        float nx = ofSignedNoise(n.pos.x * 0.005, time + noiseOffset);
        float ny = ofSignedNoise(time + noiseOffset, n.pos.y * 0.005);
        vel += ofVec2f(nx, ny) * 0.05f;

        n.pos += vel;
        
        if(count < nodes.size()-1) center += n.pos;
        count++;
    }
    center /= (nodes.size()-1);
    
    // Le noeud central suit le centre géométrique avec un peu de retard (inertie)
    nodes.back().pos += (center - nodes.back().pos) * 0.2f;

    // Pression Interne (Regonflement)
    for(int i=0; i<nodes.size()-1; i++) {
        auto& n = nodes[i];
        ofVec2f dir = n.pos - center;
        float d = dir.length();
        if(d > 0.1f) {
            // On pousse doucement vers le rayon idéal
            ofVec2f target = center + dir.getNormalized() * radius;
            n.pos += (target - n.pos) * 0.1f; // Pression plus forte pour maintenir la forme
        }
    }

    // Résolution des contraintes (Ressorts)
    for(int k=0; k<8; k++) solveConstraints(); // Plus d'itérations pour la stabilité

    // Collisions avec le décor (ColliderLayer)
    if(collider) {
        for(auto& n : nodes) {
            // On traduit la coordonnée Y de la bulle dans l'espace du collider
            float colliderY = n.pos.y - yOffset_sim;
            // On ne vérifie la collision que si la bulle est dans la zone verticale du collider
            if (colliderY >= 0 && colliderY < collider->simHeight) {
                if(collider->isWall(n.pos.x, colliderY)) {
                    ofVec2f dir = (center - n.pos).getNormalized();
                    if(dir.lengthSquared() == 0) dir.set(0, -1);
                    
                    // Réponse collision
                    n.pos += dir * 2.0f;
                    
                    // Friction contre le mur
                    ofVec2f vel = n.pos - n.oldPos;
                    n.oldPos = n.pos - vel * 0.5f;
                }
            }
        }
    }

    // Wrapping Horizontal (Boucle gauche/droite)
    if(center.x > w + radius) {
        float shift = w;
        for(auto& n : nodes) { n.pos.x -= shift; n.oldPos.x -= shift; }
        center.x -= shift;
    } else if(center.x < -radius) {
        float shift = w;
        for(auto& n : nodes) { n.pos.x += shift; n.oldPos.x += shift; }
        center.x += shift;
    }
    
    // Plafond (Bloque en haut)
    for(auto& n : nodes) {
        if(n.pos.y < 0) { 
            n.pos.y = 0; 
            // Rebond léger
            float vy = n.pos.y - n.oldPos.y;
            n.oldPos.y = n.pos.y + vy * 0.5f; 
        }
    }
}

void Bubble::solveConstraints() {
    for(auto& l : links) {
        BubbleNode& p1 = nodes[l.n1];
        BubbleNode& p2 = nodes[l.n2];
        ofVec2f delta = p1.pos - p2.pos;
        float dist = delta.length();
        if(dist == 0) continue;
        
        float diff = (dist - l.length) / dist;
        ofVec2f offset = delta * 0.5f * diff * l.stiffness;
        
        p1.pos -= offset;
        p2.pos += offset;
    }
}

void Bubble::draw() {
    if (nodes.size() < 2) return;
    // Remplissage
    ofSetColor(color);
    ofFill();
    ofBeginShape();
    // On utilise une courbe pour lisser les noeuds
    for(int i=0; i < (int)nodes.size()-1; i++) {
        // Astuce pour lisser : curveVertex. 
        // Pour bien fermer, il faut répéter les points début/fin ou utiliser une boucle
        // Ici on fait simple avec vertex pour la performance, ou curveVertex si besoin
        ofVertex(nodes[i].pos);
    }
    ofVertex(nodes[0].pos); // Fermeture
    ofEndShape();
    
    // Contour
    ofSetColor(255, 200);
    ofNoFill();
    ofSetLineWidth(5);
    ofBeginShape();
    for(int i=0; i < (int)nodes.size()-1; i++) {
        ofVertex(nodes[i].pos);
    }
    ofVertex(nodes[0].pos);
    ofEndShape();
    
    // Reflet (Shine)
    ofFill();
    ofSetColor(255, 180);
    ofDrawCircle(center.x - radius*0.3, center.y - radius*0.3, radius*0.25);
}

//--------------------------------------------------------------
// BUBBLE LAYER MANAGER
//--------------------------------------------------------------
void BubbleLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col, float yOff) {
    simWidth = w;
    simHeight = h;
    scale = s;
    collider = col;
    colliderYOffset_sim = yOff / scale;
    
    // On remplit l'écran dès le départ (comme les poissons)
    for(int i=0; i<50; i++) {
        addBubble(ofRandom(w), ofRandom(h)); 
    }
}

void BubbleLayer::addBubble(float x, float y) {
    auto b = make_shared<Bubble>();
    // Taille plus petite (8 à 20px)
    b->setup(x, y, ofRandom(4, 12));
    bubbles.push_back(b);
}

void BubbleLayer::update(float mx, float my, float time) {
    // Génération continue pour maintenir la densité
    if(bubbles.size() < 100 && ofRandom(1.0) < 0.2) {
        addBubble(ofRandom(simWidth), simHeight - 20);
    }

    for(auto& b : bubbles) {
        b->update(simWidth, simHeight, collider, colliderYOffset_sim, time);
        
        // Interaction Souris (Repousse les bulles)
        float d = b->center.distance(ofVec2f(mx/scale, my/scale)); // Conversion souris
        if(d < 100) {
             ofVec2f dir = (b->center - ofVec2f(mx/scale, my/scale)).getNormalized();
             for(auto& n : b->nodes) n.pos += dir * 2.0f;
        }

        // Respawn si elles sortent trop haut (recyclage)
        // On recycle en bas
        if(b->center.y < -50) {
            b->setup(ofRandom(simWidth), simHeight + 20, ofRandom(4, 12));
        }
    }
    
    // Collision Bulle <-> Bulle (Softbody interaction)
    for(int i=0; i<bubbles.size(); i++) {
        for(int j=i+1; j<bubbles.size(); j++) {
            auto& b1 = bubbles[i];
            auto& b2 = bubbles[j];
            
            float dist = b1->center.distance(b2->center);
            float minDist = b1->radius + b2->radius;
            
            // Si elles se touchent
            if(dist < minDist) {
                ofVec2f dir = (b1->center - b2->center).getNormalized();
                float overlap = minDist - dist;
                
                // Force de répulsion douce appliquée aux noeuds
                ofVec2f force = dir * overlap * 0.15f;
                
                for(auto& n : b1->nodes) n.pos += force;
                for(auto& n : b2->nodes) n.pos -= force;
            }
        }
    }
}

void BubbleLayer::draw() {
    ofPushMatrix();
    ofScale(scale, scale); // Mise à l'échelle globale
    
    for(auto& b : bubbles) {
        b->draw();
        
        // Wrapping Visuel (Fantômes aux bords)
        if(b->center.x < b->radius + 50) {
            ofPushMatrix(); ofTranslate(simWidth, 0); b->draw(); ofPopMatrix();
        }
        if(b->center.x > simWidth - (b->radius + 50)) {
            ofPushMatrix(); ofTranslate(-simWidth, 0); b->draw(); ofPopMatrix();
        }
    }
    ofPopMatrix();
}
