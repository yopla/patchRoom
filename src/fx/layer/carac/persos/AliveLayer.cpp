#include "AliveLayer.h"

//--------------------------------------------------------------
// ALIVE CREATURE (Reproduction of alive.as logic)
//--------------------------------------------------------------
void AliveCreature::setup(float x, float y) {
    pos.set(x, y);
    vel.set(0, 0);
    angle = 0.0f;
    angularVel = 0.0f;
    
    color = ofColor(204, 204, 204); // Gris comme le style par défaut
    
    for(int i = 0; i < 3; i++) {
        motorAngles[i] = 0.0f;
        angleLimits[i] = PI * ofRandom(0.1f, 0.8f); // SetLimits aléatoire
        // Alternate directions : dir, -dir, dir
        // 0.05 rad/frame correspond très précisément à "3 rad/s" de QuickBox2D en 60fps.
        motorSpeeds[i] = (i % 2 == 0 ? 1.0f : -1.0f) * 0.05f;
    }
}

void AliveCreature::update(shared_ptr<ColliderLayer> collider, float simWidth, float simHeight) {
    // 1. Mise à jour des Moteurs (Similaire à Box2D SetMotorSpeed & repeatWalk)
    for(int i = 0; i < 3; i++) {
        motorAngles[i] += motorSpeeds[i];
        if (motorAngles[i] > angleLimits[i]) {
            motorAngles[i] = angleLimits[i];
            motorSpeeds[i] *= -1.0f; 
        } else if (motorAngles[i] < -angleLimits[i]) {
            motorAngles[i] = -angleLimits[i];
            motorSpeeds[i] *= -1.0f;
        }
    }
    
    // 2. Cinématique Inverse / Construction de la forme locale
    // Angles des 4 segments (LegA, LegB, LegC, LegD) par rapport à la base
    float a[4];
    a[0] = 0.0f;
    a[1] = a[0] + motorAngles[0];
    a[2] = a[1] + motorAngles[1];
    a[3] = a[2] + motorAngles[2];
    
    // Positions locales des centres des rectangles
    // Dans alive.as : dx = 0.7 = 21px. Le joint est donc décalé de 10.5px de chaque centre.
    localNodes[0].set(0, 0);
    localNodes[1] = localNodes[0] + ofVec2f(10.5f, 0).getRotatedRad(a[0]) + ofVec2f(10.5f, 0).getRotatedRad(a[1]);
    localNodes[2] = localNodes[1] + ofVec2f(10.5f, 0).getRotatedRad(a[1]) + ofVec2f(10.5f, 0).getRotatedRad(a[2]);
    localNodes[3] = localNodes[2] + ofVec2f(10.5f, 0).getRotatedRad(a[2]) + ofVec2f(10.5f, 0).getRotatedRad(a[3]);
    
    // Recentrement pour une rotation stable autour du vrai Centre de Gravité
    ofVec2f com(0, 0);
    for(int i = 0; i < 4; i++) com += localNodes[i];
    com /= 4.0f;
    for(int i = 0; i < 4; i++) localNodes[i] -= com;
        
    // 3. Physique Globale Rigide
    if (bIsDragged) {
        ofVec2f diff = dragTarget - pos;
        vel += diff * 0.1f; // Tiré vers la souris
        vel *= 0.8f;        // Amortissement fort pendant qu'il est attrapé
        angularVel *= 0.8f;
    } else {
        vel.y += 0.3f; // Gravité
    }
    
    vel *= 0.98f;  // Amortissement linéaire
    angularVel *= 0.95f; // Amortissement rotatif
    
    pos += vel;
    angle += angularVel;
    
    // Calcul des positions mondiales des segments pour la collision et le rendu
    for(int i = 0; i < 4; i++) {
        worldAngles[i] = angle + a[i];
        worldNodes[i] = pos + localNodes[i].getRotatedRad(angle);
    }
            
    // 4. Collisions Solides au Pixel
    if(collider) {
        for(int k = 0; k < 5; k++) { // 5 micro-steps pour une réaction très solide
            ofVec2f totalPush(0, 0);
            float totalTorque = 0.0f;
            int hitCount = 0;

            for(int i = 0; i < 4; i++) {
                ofVec2f dirX = ofVec2f(1.0f, 0.0f).getRotatedRad(worldAngles[i]);
                ofVec2f dirY = ofVec2f(0.0f, 1.0f).getRotatedRad(worldAngles[i]);
                
                float hx = 15.0f;  // Demi-longueur exacte (30px total)
                float hy = 2.25f;  // Demi-épaisseur exacte (4.5px total)
                
                // Échantillonnage de la surface du rectangle pour une collision parfaite
                for(float tx = -hx; tx <= hx; tx += 5.0f) {
                    for(float ty = -hy; ty <= hy; ty += 4.5f) { // Teste les deux bords extrêmes de l'épaisseur
                        ofVec2f pt = worldNodes[i] + dirX * tx + dirY * ty;
                        
                        if(collider->isWall(pt.x, pt.y)) {
                            // Déduction de la Normale de Collision via gradient
                            float eps = 2.0f;
                            float nx = (collider->isWall(pt.x - eps, pt.y) ? 1 : 0) - (collider->isWall(pt.x + eps, pt.y) ? 1 : 0);
                            float ny = (collider->isWall(pt.x, pt.y - eps) ? 1 : 0) - (collider->isWall(pt.x, pt.y + eps) ? 1 : 0);
                            ofVec2f normal(nx, ny);
                            
                            if(normal.lengthSquared() < 0.1f) normal.set(0, -1); // Force vers le haut si totalement coincé
                            else normal.normalize();
                            
                            totalPush += normal;
                            ofVec2f r = pt - pos;
                            totalTorque += (r.x * normal.y - r.y * normal.x);
                            hitCount++;
                        }
                    }
                }
            }
            
            if (hitCount > 0) {
                totalPush.normalize();
                
                // Résolution de la Pénétration globale et unifiée (1.5 px max par step, peu importe le nombre de points touchés)
                pos += totalPush * 1.5f; 
                angle += (totalTorque / hitCount) * 0.003f;
                
                // Résolution de la Vélocité (Annule la vitesse qui pousse vers le mur = pas de rebond/vibration = stabilité au sol)
                float velAlongNormal = vel.dot(totalPush);
                if(velAlongNormal < 0) {
                    vel -= totalPush * velAlongNormal; 
                }
                
                // Friction (Traction) sur le mur permettant aux moteurs de faire avancer l'objet de façon fluide
                vel *= 0.85f;
                angularVel *= 0.85f;

                // Re-calcule les noeuds si la physique l'a bougé
                for(int i = 0; i < 4; i++) {
                    worldAngles[i] = angle + a[i];
                    worldNodes[i] = pos + localNodes[i].getRotatedRad(angle);
                }
            } else {
                break; // Plus de collision détectée
            }
        }
    }
}

void AliveCreature::draw(float scale) {
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CENTER);
    
    if (bIsDragged) {
        ofPushStyle();
        ofSetColor(255, 0, 0, 150);
        ofSetLineWidth(2.0f);
        ofDrawLine(pos * scale, dragTarget * scale);
        ofPopStyle();
    }
    
    for(int i = 0; i < 4; i++) {
        ofVec2f p = worldNodes[i] * scale;
        
        ofPushMatrix();
        ofTranslate(p.x, p.y);
        ofRotateRad(worldAngles[i]);
        
        // Tailles exactes AS3: wd: 1 (30px), hd: 0.15 (4.5px)
        float w = 30.0f * scale;
        float h = 4.5f * scale;
        
        ofSetColor(0); // Fill (Noir)
        ofDrawRectangle(0, 0, w, h);
        ofNoFill();
        ofSetColor(204); // Line (Gris clair)
        ofSetLineWidth(1.0f);
        ofDrawRectangle(0, 0, w, h);
        ofFill();
        
        ofPopMatrix();
    }
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofPopStyle();
}

//--------------------------------------------------------------
// ALIVE LAYER
//--------------------------------------------------------------
void AliveLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w; simHeight = h; scale = s; collider = col;
    
    // Optionnel: Spawn de 10 objets par defaut au démarrage du layer comme dans l'AS3
    for(int i = 0; i < 10; i++) {
        addCreature(w * 0.5f + ofRandom(-100, 100), h * 0.3f + ofRandom(-100, 100));
    }
}

void AliveLayer::update(float mx, float my, float time) {
    for(auto& c : creatures) c->update(collider, simWidth, simHeight);
}

void AliveLayer::draw() {
    for(auto& c : creatures) c->draw(scale);
}

void AliveLayer::addCreature(float x, float y) {
    auto c = make_shared<AliveCreature>();
    c->setup(x, y);
    creatures.push_back(c);
}

void AliveLayer::mousePressed(float x, float y, int button) {
    float bestDist = 80.0f; // Rayon de préhension large
    draggedCreature = nullptr;
    for(auto& c : creatures) {
        float d = c->pos.distance(ofVec2f(x, y));
        if(d < bestDist) {
            bestDist = d;
            draggedCreature = c;
        }
    }
    if(draggedCreature) {
        draggedCreature->bIsDragged = true;
        draggedCreature->dragTarget.set(x, y);
    }
}

void AliveLayer::mouseDragged(float x, float y, int button) {
    if(draggedCreature) {
        draggedCreature->dragTarget.set(x, y);
    }
}

void AliveLayer::mouseReleased(float x, float y, int button) {
    if(draggedCreature) {
        draggedCreature->bIsDragged = false;
        draggedCreature = nullptr;
    }
}