#include "SwingLayer.h"
#include "ofApp.h" // Pour APP_FPS

void SwingLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    scale = s;
    collider = col;
    
    swingers.clear();
    int numSwingers = 5;
    for(int i=0; i<numSwingers; i++) {
        Swinger sw;
        // On les fait apparaître en bas de la scène, près du "sol"
        sw.pos.set(ofRandom(simWidth), simHeight - ofRandom(20, 100));
        sw.oldPos = sw.pos;
        sw.isSwinging = false;
        sw.color = ofColor::fromHsb(ofRandom(255), 200, 255);
        sw.shootCooldown = ofRandom(1.0f); // Décale les premiers tirs
        sw.swingTimer = 0.0f;
        swingers.push_back(sw);
    }
}

void SwingLayer::update(float mouseX, float mouseY, float time) {
    if (!collider) return;

    float dt = 1.0f / (float)APP_FPS;
    if (APP_FPS == 0) dt = 1.0/60.0; // Sécurité

    for(auto& s : swingers) {
        updateSwinger(s, dt);
    }
}

void SwingLayer::updateSwinger(Swinger& s, float dt) {
    // 1. Intégration Verlet
    ofVec2f vel = s.pos - s.oldPos;
    s.oldPos = s.pos;
    
    vel *= friction;
    vel.y += gravity;
    
    s.pos += vel;

    // GRAPPIN : On raccourcit le fil pour monter (Treuil) - Une fois par frame seulement
    if (s.isSwinging) {
        s.ropeLength -= 2.0f; // Vitesse de montée plus modérée
        if(s.ropeLength < 30.0f) s.ropeLength = 30.0f;
    }

    // 2. Contraintes
    for(int i=0; i<constraintIterations; i++) {
        // A. Contrainte du fil
        if (s.isSwinging) {
            ofVec2f toAnchor = s.pos - s.ropeAnchor;
            float dist = toAnchor.length();
            if (dist > s.ropeLength) {
                s.pos = s.ropeAnchor + toAnchor.getNormalized() * s.ropeLength;
            }
        }

        // B. Collision avec les murs
        if (collider->isWall(s.pos.x, s.pos.y)) {
            // Si on est dans un mur, on revient à la position précédente et on arrête tout.
            s.pos = s.oldPos;
            s.oldPos = s.pos; // Annule la vélocité
            if (s.isSwinging) {
                s.isSwinging = false;
                s.shootCooldown = 1.0f; // Temps de pause avant de relancer
            }
        }
    }

    // 3. Logique d'état (Lancer / Relâcher)
    s.shootCooldown -= dt;
    
    // Condition pour relâcher le fil
    if (s.isSwinging) {
        s.swingTimer += dt;
        
        ofVec2f vel = s.pos - s.oldPos;
        // On relâche si on est arrivé en haut (fil court) ou si on est bloqué trop longtemps
        if (s.ropeLength <= 30.0f || s.swingTimer > 2.0f) {
            s.isSwinging = false;
            s.shootCooldown = ofRandom(0.5f, 1.5f); // Pause plus longue pour tomber/planer un peu
            
            // Petit boost de saut au relâchement pour la fluidité
            s.oldPos = s.pos - (vel * 1.2f); 
        }
    }
    
    // Condition pour tirer un fil
    if (!s.isSwinging && s.shootCooldown <= 0) {
        shootRope(s);
    }
    
    // Limites du monde (sécurité)
    // Wrapping Horizontal (Cycle gauche <-> droite)
    if (s.pos.x < 0) { 
        s.pos.x += simWidth; 
        s.oldPos.x += simWidth; 
        s.isSwinging = false; // On coupe le fil si on traverse
    }
    if (s.pos.x > simWidth) { 
        s.pos.x -= simWidth; 
        s.oldPos.x -= simWidth; 
        s.isSwinging = false; 
    }
    
    if (s.pos.y > simHeight) { s.pos.y = simHeight; s.oldPos.y = s.pos.y; }
    if (s.pos.y < 0) { s.pos.y = 0; s.oldPos.y = s.pos.y; }
}

void SwingLayer::shootRope(Swinger& s) {
    // Tir directionnel basé sur le mouvement (Ballet aérien)
    ofVec2f vel = s.pos - s.oldPos;
    ofVec2f shootDir;
    
    if(vel.lengthSquared() > 1.0f) {
        shootDir = vel.getNormalized();
        shootDir.y -= 1.2f; // Biais vers le haut
    } else {
        shootDir.set(ofRandom(-0.5, 0.5), -1.0f); // Tir vers le haut par défaut
    }
    shootDir.normalize();
    // Petit cône aléatoire
    shootDir.rotate(ofRandom(-20, 20));

    ofVec2f rayPos = s.pos;
    float step = 5.0f;
    int maxSteps = 150; // ~750px de portée
    
    for(int i=0; i<maxSteps; i++) { 
        rayPos += shootDir * step;
        
        if (rayPos.x < 0 || rayPos.x >= simWidth || rayPos.y < 0 || rayPos.y >= simHeight) break;

        if (collider->isWall(rayPos.x, rayPos.y)) {
            s.isSwinging = true;
            s.swingTimer = 0.0f;
            s.ropeAnchor = rayPos;
            s.ropeLength = s.pos.distance(s.ropeAnchor);
            if (s.ropeLength < 20) s.isSwinging = false; 
            return;
        }
    }
    s.shootCooldown = 0.1f; // Pas de mur trouvé, on réessaie très vite
}

void SwingLayer::draw() {
    ofPushStyle();
    ofPushMatrix();
    ofScale(scale, scale); // On applique l'échelle globale pour le dessin

    for(auto& s : swingers) {
        if (s.isSwinging) {
            ofSetColor(255, 255, 255, 150);
            ofSetLineWidth(2);
            ofDrawLine(s.pos, s.ropeAnchor);
            ofDrawCircle(s.ropeAnchor, 4);
        }
        ofSetColor(s.color);
        ofDrawRectangle(s.pos.x - s.size/2, s.pos.y - s.size/2, s.size, s.size);
    }

    ofPopMatrix();
    ofPopStyle();
}