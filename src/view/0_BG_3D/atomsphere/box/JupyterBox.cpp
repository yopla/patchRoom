#include "JupyterBox.h"

void JupyterBox::setup(float w, float h, float d) {
    width = w; height = h; depth = d;
    reset();
}

void JupyterBox::reset() {
    gt = 0;
    units.clear();

    // Couleurs AS3 (Jaune, Rouge, Magenta, Vert, Cyan, Bleu) + Orange (7ème) + Blanc (8ème dispersée)
    ofColor colors[8] = {
        ofColor(255, 255, 0), ofColor(255, 0, 0), ofColor(255, 0, 255),
        ofColor(0, 255, 0), ofColor(0, 255, 255), ofColor(0, 0, 255),
        ofColor(255, 128, 0), ofColor(255, 255, 255)
    };

    auto addBase = [&](ofVec3f pos, int team, ofColor color) {
        auto u = std::make_shared<JpUnit>();
        u->hp = 50; u->team = team; u->spec = 1; // AS3 ratio adapté
        u->spawnPriority = 0;
        u->pos = pos; u->vel.set(0, 0, 0);
        u->reload = 0; u->dmgTime = 0; u->spawnTimer = 0;
        u->color = color;
        units.push_back(u);
    };

    // Volume AS3 : Environ 10 bases par équipe (80 bases au total)
    int nbBase = 10;
    float diskRadius = 400.0f; // Rayon de dispersion initial autour du centre du mur
    
    // Centres des deux pans de toits projetés sur le plafond plat de la box
    float peakX = 252.0f; // (1452 - 1200) issu de RoomWalls
    float centerTopCour = (peakX + width / 2.0f) / 2.0f;
    float centerTopJar = (-width / 2.0f + peakX) / 2.0f;

    for (int team = 0; team < 8; team++) {
        for (int i = 0; i < nbBase; i++) {
            ofVec3f pos;
            
            if (team < 7) {
                // Point aléatoire dans un disque 2D pour concentrer les bases
                float angle = ofRandom(TWO_PI);
                float r = sqrt(ofRandom(1.0f)) * diskRadius;
                float dx = cos(angle) * r;
                float dy = sin(angle) * r;
                
                if(team == 0) pos.set(dx, -height/2, dy);                     // Sol (Jaune = 0)
                else if(team == 1) pos.set(dx, dy, -depth/2);                 // Mur Back
                else if(team == 2) pos.set(width/2, dx, dy);                  // Mur Droit
                else if(team == 3) pos.set(-width/2, dx, dy);                 // Mur Gauche
                else if(team == 4) pos.set(centerTopCour + dx, height/2, dy); // Plafond 1 (Top Cour)
                else if(team == 5) pos.set(dx, dy, depth/2);                  // Mur Front (Bleu = 5)
                else if(team == 6) pos.set(centerTopJar + dx, height/2, dy);  // Plafond 2 (Top Jar)
            } else {
                // 8ème couleur : distribuée aléatoirement en évitant les autres
                bool valid = false;
                int attempts = 0;
                while (!valid && attempts < 100) {
                    attempts++;
                    int face = ofRandom(6);
                    float rx = ofRandom(-width/2.0f, width/2.0f);
                    float ry = ofRandom(-height/2.0f, height/2.0f);
                    float rz = ofRandom(-depth/2.0f, depth/2.0f);
                    
                    if (face == 0) pos.set(rx, ry, depth/2.0f);
                    else if (face == 1) pos.set(rx, ry, -depth/2.0f);
                    else if (face == 2) pos.set(width/2.0f, ry, rz);
                    else if (face == 3) pos.set(-width/2.0f, ry, rz);
                    else if (face == 4) pos.set(rx, height/2.0f, rz);
                    else if (face == 5) pos.set(rx, -height/2.0f, rz);
                    
                    valid = true;
                    // Vérifie si on est trop près des autres bases
                    for (auto& u : units) {
                        if (u->spec == 1 && u->pos.distance(pos) < 500.0f) {
                            valid = false;
                            break;
                        }
                    }
                }
            }
            
            addBase(pos, team, colors[team]);
        }
    }

    // Mélange les bases aléatoirement pour casser la priorité de lecture de l'array
    ofShuffle(units);
    
    // Assigne et mémorise la priorité de spawn pour l'affichage (0 à 79)
    int currentPriority = 0;
    for (auto& u : units) {
        if (u->spec == 1) u->spawnPriority = currentPriority++;
    }
}

void JupyterBox::update() {
    if (bPaused) return;
    
    gt++;
    float spd = 18.0f; // Plus rapide (AS3 spd = 2.5 pour petit écran)
    float range = 400.0f;
    float sight = 3000.0f;
    int maxUnits = 1024; // Limite exacte AS3 (maxi = 512)

    std::vector<std::shared_ptr<JpUnit>> newUnits;
    
    int aliveUnits = 0;
    for (auto& u : units) if (u->spec == 0 && u->hp > 0) aliveUnits++;

    for (auto& u : units) {
        if (u->hp <= 0) continue;

        if (u->spec == 1) { // Logique des Bases
            if (gt > u->spawnTimer && (aliveUnits + newUnits.size()) < maxUnits) {
                // Rythme très agressif AS3: 1 + rnd(2). On l'adapte pour le 60fps C++
                u->spawnTimer = gt + 10 + ofRandom(0, 10);
                
                auto spawn = std::make_shared<JpUnit>();
                spawn->hp = 8; spawn->team = u->team; spawn->spec = 0;
                spawn->pos = u->pos + ofVec3f(ofRandom(-20, 20), ofRandom(-20, 20), ofRandom(-20, 20));
                spawn->vel.set(0, 0, 0);
                spawn->reload = 0; spawn->dmgTime = 0; spawn->spawnTimer = 0;
                spawn->color = u->color;
                newUnits.push_back(spawn);
            }
        } else { // Logique des Unités Mobiles
            // Recherche de cible
            if ((gt % 8) == (u->team % 8) || u->targ.expired()) { // Optimisation AS3 % 8
                std::shared_ptr<JpUnit> closest = nullptr;
                float minDist = sight * sight;
                for (auto& other : units) {
                    if (other->hp <= 0 || other->team == u->team) continue;
                    float d2 = u->pos.squareDistance(other->pos);
                    if (d2 < minDist) {
                        minDist = d2; closest = other;
                    }
                }
                u->targ = closest;
            }

            auto target = u->targ.lock();
            if (target && target->hp > 0) {
                float dist = u->pos.distance(target->pos);
                if (dist < range + (u->team * 10)) { // Diversité de portée
                    u->vel.set(0, 0, 0); // Arrêt pour tirer
                    if (gt > u->reload) {
                        u->reload = gt + 10; // AS3 recharge = 4
                        u->dmgTime = gt + 5; // Flash visuel
                        target->hp -= 1; // AS3 dmg = 1
                        
                        if (target->hp <= 0 && target->spec == 1) {
                            // La base n'est pas détruite, elle est convertie
                            target->hp = 50; 
                            target->team = u->team;
                            target->color = u->color;
                        }
                    }
                } else {
                    // Mouvement à la surface
                    ofVec3f dir = (target->pos - u->pos).getNormalized();
                    
                    // Identification du mur actuel par projection
                    float nx = u->pos.x / (width / 2.0f);
                    float ny = u->pos.y / (height / 2.0f);
                    float nz = u->pos.z / (depth / 2.0f);
                    float m = std::max({std::abs(nx), std::abs(ny), std::abs(nz)});
                    ofVec3f N(0, 0, 0);
                    if (m == std::abs(nx)) N.x = (nx > 0 ? 1 : -1);
                    else if (m == std::abs(ny)) N.y = (ny > 0 ? 1 : -1);
                    else N.z = (nz > 0 ? 1 : -1);

                    // Vélocité rasante (projection sur le plan du mur)
                    ofVec3f surface_vel = dir - N * dir.dot(N);
                    if (surface_vel.lengthSquared() > 0.001f) u->vel = surface_vel.getNormalized() * spd;
                    else u->vel = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1)).getNormalized() * spd;
                }
            } else {
                if (gt % 60 == 0) u->vel = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1)).getNormalized() * spd;
            }

            u->pos += u->vel;

            // Accroche parfaite mathématique de la position sur le cube (Même quand l'unité dépasse l'arête)
            float nx2 = u->pos.x / (width / 2.0f);
            float ny2 = u->pos.y / (height / 2.0f);
            float nz2 = u->pos.z / (depth / 2.0f);
            float m2 = std::max({std::abs(nx2), std::abs(ny2), std::abs(nz2)});
            if (m2 > 0.001f) { u->pos.x /= m2; u->pos.y /= m2; u->pos.z /= m2; }
        }
    }

    // On ajoute toutes les nouvelles unités de façon sécurisée APRES la boucle
    units.insert(units.end(), newUnits.begin(), newUnits.end());

    // Suppression des morts
    units.erase(std::remove_if(units.begin(), units.end(), [](const std::shared_ptr<JpUnit>& u) { return u->hp <= 0; }), units.end());
}

void JupyterBox::draw() {
    ofPushStyle();
    ofEnableDepthTest();
    
    ofPushMatrix();
    ofTranslate(0, height / 2.0f - 100.0f, 0); // Position identique à BoxTexture
    ofScale(-1, 1, 1);
    
    ofSetColor(50, 50, 50, 50);
    ofNoFill();
    ofDrawBox(0, 0, 0, width, height, depth);
    
    ofFill();
    for (auto& u : units) {
        if (u->hp <= 0) continue;
        
        if (gt < u->dmgTime) ofSetColor(255); // Flash quand on tire
        else ofSetColor(u->color);
        
        if (u->spec == 1) {
            ofDrawBox(u->pos, 40); // Base
            ofSetColor(0); // Numéro en noir pour bien contraster avec les couleurs vives
            // On décale légèrement le numéro vers le centre de la pièce (0,0,0) pour le rendre visible de l'extérieur
            ofVec3f textPos = u->pos + (ofVec3f(0,0,0) - u->pos).getNormalized() * 30.0f;
            ofDrawBitmapString(ofToString(u->spawnPriority), textPos.x - 4, textPos.y + 4, textPos.z);
        } else {
            ofDrawSphere(u->pos, 10); // Unité
        }
        
        // Dessin des tirs (lasers internes)
        if (u->spec == 0 && gt < u->dmgTime) {
            auto target = u->targ.lock();
            if (target && target->hp > 0) {
                ofSetColor(u->color, 200);
                ofSetLineWidth(3);
                ofDrawLine(u->pos, target->pos);
                ofSetLineWidth(1);
            }
        }
    }
    
    ofPopMatrix();
    ofPopStyle();
}