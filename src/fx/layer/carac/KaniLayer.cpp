#include "KaniLayer.h"

//--------------------------------------------------------------
// KANI (Creature)
//--------------------------------------------------------------
void Kani::setup(float x, float y) {
    pos.set(x, y);
    vel.set(0, 0);
    
    // Création des 6 pattes (3 en haut, 3 en bas comme la ref)
    // Offsets X : -80, 0, 80
    float offsets[] = {-80, 0, 80};
    
    for(int i=0; i<3; i++) {
        // Patte Bas (Sol)
        KaniLeg l1;
        l1.offsetX = offsets[i];
        l1.dirY = 1.0f;
        l1.pos = ofVec2f(x + l1.offsetX, y + 100);
        l1.target = l1.pos;
        l1.isMoving = false;
        l1.moveSpeed = 0.15f;
        l1.grounded = false;
        l1.thresholdOffset = ofRandom(-20.0f, 20.0f); // Asymétrie
        legs.push_back(l1);
        
        // Patte Haut (Plafond)
        KaniLeg l2;
        l2.offsetX = offsets[i];
        l2.dirY = -1.0f;
        l2.pos = ofVec2f(x + l2.offsetX, y - 100);
        l2.target = l2.pos;
        l2.isMoving = false;
        l2.moveSpeed = 0.15f;
        l2.grounded = false;
        l2.thresholdOffset = ofRandom(-20.0f, 20.0f); // Asymétrie
        legs.push_back(l2);
    }
}

bool Kani::castRay(float x, float startY, float dir, shared_ptr<ColliderLayer> collider, float& outY) {
    if(!collider) return false;
    
    // Raycast simple pixel par pixel (ou par pas de 4 pour perf)
    float checkY = startY;
    float limit = legReachY; 
    float step = 4.0f * dir;
    
    for(float d = 0; d < limit; d += 4.0f) {
        checkY += step;
        // Vérif limites monde (en coordonnées monde)
        float worldHeight = collider->simHeight * collider->scale;
        if(checkY < 0 || checkY >= worldHeight) return false;
        
        // Conversion des coordonnées monde vers les coordonnées de simulation du collider
        float simX = x / collider->scale;
        float simCheckY = checkY / collider->scale;

        if(collider->isWall(simX, simCheckY)) {
            // Affinement binaire pour coller parfaitement à la surface
            // On sait que checkY est dans le mur, et checkY - step était vide
            float solidY = checkY;
            float emptyY = checkY - step;
            
            for(int k=0; k<4; k++) {
                float midY = (solidY + emptyY) * 0.5f;
                if(collider->isWall(simX, midY / collider->scale)) {
                    solidY = midY;
                } else {
                    emptyY = midY;
                }
            }
            outY = solidY;
            return true; // Surface trouvée
        }
    }
    return false;
}

bool Kani::findBestSurface(float startX, float startY, float dir, shared_ptr<ColliderLayer> collider, float worldWidth, ofVec2f& outPoint) {
    // Recherche centrale d'abord
    float hitY;
    if(castRay(startX, startY, dir, collider, hitY)) {
        outPoint.set(startX, hitY);
        return true;
    }
    
    // Recherche latérale étendue (Scan gauche/droite)
    int steps = 10; // Nombre de tests de chaque côté
    float stepSize = 15.0f; // Pas de recherche
    
    for(int i=1; i<=steps; i++) {
        float offsets[] = {i * stepSize, -i * stepSize};
        for(int j=0; j<2; j++) {
            float testX = startX + offsets[j];
            
            // Wrapping du testX
            if(worldWidth > 0) {
                if(testX < 0) testX += worldWidth;
                else if(testX >= worldWidth) testX -= worldWidth;
            }
            
            if(castRay(testX, startY, dir, collider, hitY)) {
                outPoint.set(testX, hitY);
                return true;
            }
        }
    }
    return false;
}

void Kani::update(float targetX, float targetY, shared_ptr<ColliderLayer> collider, float time, float worldWidth) {
    // 1. Mouvement du corps (Suit la souris avec easing)
    ofVec2f target(targetX, targetY);
    ofVec2f diff = target - pos;
    
    // Wrapping distance check (si la souris est de l'autre côté de l'écran)
    if(worldWidth > 0) {
        if(diff.x > worldWidth * 0.5f) diff.x -= worldWidth;
        else if(diff.x < -worldWidth * 0.5f) diff.x += worldWidth;
    }
    
    vel = diff * 0.15f; // Easing plus rapide pour mieux suivre la souris
    pos += vel;
    
    // Wrapping position corps
    if(worldWidth > 0) {
        if(pos.x < 0) pos.x += worldWidth;
        if(pos.x > worldWidth) pos.x -= worldWidth;
    }

    // 2. Gestion des pattes (Scheduler)
    struct LegCandidate {
        int index;
        float priority;
    };
    vector<LegCandidate> candidates;

    for(int i=0; i<legs.size(); i++) {
        auto& leg = legs[i];
        // Position idéale X relative au corps
        float idealX = pos.x + leg.offsetX;
        
        // Gestion du wrapping pour le calcul de distance
        float distX = idealX - leg.pos.x;
        if(worldWidth > 0) {
            if(distX > worldWidth * 0.5f) distX -= worldWidth;
            else if(distX < -worldWidth * 0.5f) distX += worldWidth;
        }

        // Identification des candidats au mouvement
        if(!leg.isMoving) {
            float priority = 0.0f;
            
            // 1. Vérification distance
            if(abs(distX) > (stepThreshold + leg.thresholdOffset)) priority = abs(distX);
            
            // 2. Vérification perte de sol (si on était posé)
            if(leg.grounded && collider) {
                // On regarde un peu "sous" la patte pour voir si le sol est toujours là
                float simLegX = leg.pos.x / collider->scale;
                float simLegY = (leg.pos.y + leg.dirY * 10.0f) / collider->scale;
                if(!collider->isWall(simLegX, simLegY)) {
                    priority = 10000.0f; // Urgence absolue
                }
            }
            
            // 3. Opportunisme : Si on vole (pas grounded), on essaie d'attraper un mur si on passe près
            if(!leg.grounded) {
                float testY;
                // On teste à la position idéale X
                float checkX = idealX;
                if(worldWidth > 0) { // Wrap checkX
                    if(checkX < 0) checkX += worldWidth;
                    else if(checkX >= worldWidth) checkX -= worldWidth;
                }
                if(castRay(checkX, pos.y, leg.dirY, collider, testY)) priority = 5000.0f; // Priorité haute
            }
            
            if(priority > 0.0f) {
                candidates.push_back({i, priority});
            }
        }
    }

    // Tri des candidats par priorité
    sort(candidates.begin(), candidates.end(), [](const LegCandidate& a, const LegCandidate& b){
        return a.priority > b.priority;
    });

    // Déclenchement des mouvements par groupes
    if(!candidates.empty() && (time - lastMoveTime > moveDelay)) {
        int count = 0;
        for(auto& cand : candidates) {
            if(count >= moveGroupSize) break;
            
            auto& leg = legs[cand.index];
            
            // Logique de déclenchement du pas
                leg.isMoving = true;
                leg.moveProgress = 0.0f;
                leg.startPos = leg.pos;
                
                // Recalcul de idealX pour la cible
                float idealX = pos.x + leg.offsetX;
                
                // On vise un peu en avant du mouvement (prédiction)
                float predictX = idealX + vel.x * 20.0f;
                
                // Wrapping predictX pour le raycast (doit être dans [0, W])
                float checkX = predictX;
                if(worldWidth > 0) {
                    if(checkX < 0) checkX += worldWidth;
                    else if(checkX >= worldWidth) checkX -= worldWidth;
                }
                
                // On cherche le sol/plafond à cet endroit
                ofVec2f surfacePoint;
                if(findBestSurface(checkX, pos.y, leg.dirY, collider, worldWidth, surfacePoint)) {
                    // Cible trouvée à checkX (wrapped)
                    // On calcule la cible d'interpolation la plus proche de startPos (unwrapped)
                    float tX = surfacePoint.x;
                    if (worldWidth > 0) {
                        float d = tX - leg.startPos.x;
                        if (d > worldWidth * 0.5f) tX -= worldWidth;
                        else if (d < -worldWidth * 0.5f) tX += worldWidth;
                    }
                    leg.target.set(tX, surfacePoint.y);
                    leg.grounded = true;
                } else {
                    // Pas de sol : on replie la patte en position d'attente
                    // On part de idealX, on le wrap pour être sûr, puis on le rapproche de startPos
                    float tX = idealX;
                    if (worldWidth > 0) {
                        // Wrap [0, W]
                        if (tX < 0) tX += worldWidth;
                        else if (tX >= worldWidth) tX -= worldWidth;
                        
                        // Shortest path from startPos
                        float d = tX - leg.startPos.x;
                        if (d > worldWidth * 0.5f) tX -= worldWidth;
                        else if (d < -worldWidth * 0.5f) tX += worldWidth;
                    }
                    leg.target.set(tX, pos.y + leg.dirY * 60.0f);
                    leg.grounded = false;
                }
            
            count++;
        }
        if(count > 0) lastMoveTime = time;
    }

    // Animation des pattes
    for(auto& leg : legs) {
        // Animation du pas
        if(leg.isMoving) {
            leg.moveProgress += leg.moveSpeed;
            if(leg.moveProgress >= 1.0f) {
                leg.moveProgress = 1.0f;
                leg.isMoving = false;
                
                // Wrap final position
                ofVec2f finalPos = leg.target;
                if (worldWidth > 0) {
                    if (finalPos.x < 0) finalPos.x += worldWidth;
                    else if (finalPos.x >= worldWidth) finalPos.x -= worldWidth;
                }
                leg.pos = finalPos;
            } else {
                // Interpolation linéaire pour X,Y
                ofVec2f currentPos = leg.startPos.getInterpolated(leg.target, leg.moveProgress);
                
                // Arc de cercle pour lever la patte (sinus)
                // On lève vers le corps (inverse de dirY)
                float lift = sin(leg.moveProgress * PI) * 40.0f;
                currentPos.y -= lift * leg.dirY;
                
                // Wrapping du résultat pour le stockage dans leg.pos (affichage)
                if (worldWidth > 0) {
                    if (currentPos.x < 0) currentPos.x += worldWidth;
                    else if (currentPos.x >= worldWidth) currentPos.x -= worldWidth;
                }
                
                leg.pos = currentPos;
            }
        }
    }
}

void Kani::draw(float offsetX, float offsetY, float worldWidth) {
    // Style "Bete" (Mauve/Rose)
    ofColor colLeg(255, 128, 255);
    ofColor colBody(192, 96, 192);
    
    // Pattes
    ofSetLineWidth(4);
    for(auto& leg : legs) {
        ofSetColor(colLeg);

        ofVec2f p_body = ofVec2f(pos.x + leg.offsetX * 0.2f, pos.y + 10.0f * leg.dirY);
        ofVec2f p_knee = leg.pos;
        p_knee.y -= 40.0f * leg.dirY;
        ofVec2f p_foot = leg.pos;

        // On calcule la position de dessin la plus courte pour le genou par rapport au corps
        float dx_knee = p_knee.x - p_body.x;
        if (worldWidth > 0) {
            if (dx_knee > worldWidth * 0.5f) p_knee.x -= worldWidth;
            else if (dx_knee < -worldWidth * 0.5f) p_knee.x += worldWidth;
        }

        // On fait de même pour le pied par rapport au genou
        float dx_foot = p_foot.x - p_knee.x;
        if (worldWidth > 0) {
            if (dx_foot > worldWidth * 0.5f) p_foot.x -= worldWidth;
            else if (dx_foot < -worldWidth * 0.5f) p_foot.x += worldWidth;
        }

        ofDrawLine(p_body.x + offsetX, p_body.y + offsetY, p_knee.x + offsetX, p_knee.y + offsetY);
        ofDrawLine(p_knee.x + offsetX, p_knee.y + offsetY, p_foot.x + offsetX, p_foot.y + offsetY);
        ofDrawCircle(p_foot.x + offsetX, p_foot.y + offsetY, 4);
    }
    
    // Corps
    ofSetColor(colBody);
    ofFill();
    ofDrawRectangle(pos.x + offsetX - 15, pos.y + offsetY - 15, 30, 30);
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(colLeg);
    ofDrawRectangle(pos.x + offsetX - 15, pos.y + offsetY - 15, 30, 30);
}

//--------------------------------------------------------------
// KANI LAYER
//--------------------------------------------------------------
void KaniLayer::setup(float w, float h, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    collider = col;
    kani.setup(w/2, h/2);
}

void KaniLayer::update(float mx, float my, float time) {
    kani.update(mx, my, collider, time, simWidth);
}

void KaniLayer::draw() {
    // Gestion du wrapping visuel, comme le poulpe
    float wrapThreshold = 300.0f;

    vector<ofVec2f> offsets;
    offsets.push_back(ofVec2f(0,0));
    // Si la créature est près du bord gauche, on prépare un fantôme à droite
    if (kani.pos.x < wrapThreshold) {
        offsets.push_back(ofVec2f(simWidth, 0));
    }
    // Si la créature est près du bord droit, on prépare un fantôme à gauche
    if (kani.pos.x > simWidth - wrapThreshold) {
        offsets.push_back(ofVec2f(-simWidth, 0));
    }

    // On dessine la créature pour chaque offset nécessaire
    for (const auto& offset : offsets) {
        kani.draw(offset.x, offset.y, simWidth);
    }
}