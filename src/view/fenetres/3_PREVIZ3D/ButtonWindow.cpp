#include "ButtonWindow.h"
#include <map>

// Stockage global des états OSC pour les boutons
static std::map<int, bool> g_oscButtonStates;

void setButtonOscState(int id, bool state) {
    g_oscButtonStates[id] = state;
}

//--------------------------------------------------------------
void ButtonWindow::setup(float w, float h) {
    
    // Chargement de l'image de fond
    // On gère le cas de l'extension .jog (typo probable pour .jpg)
    bool bLoaded = false;
    if(!bgImg.load("GAB1/SOL.jpg")) {
        if(!bgImg.load("GAB1/SOL.png")) {
        } else bLoaded = true;
    } else bLoaded = true;

    if(bLoaded) {
        winW = bgImg.getWidth();
        winH = bgImg.getHeight();
    } else {
        winW = w;
        winH = h;
    }

    buttons.clear();
    
    // Configuration: Variable pour définir le nombre de boutons par côté (8 au lieu de 12)
    int numButtonsPerSide = 12;
    int numCenterButtons = 3; // <--- NOUVELLE VARIABLE : Changez cette valeur (ex: 1, 3, 5...)
    float bigBtnSize = 360.0f; // Taille indépendante pour les gros boutons du milieu
    float bigBtnSpacing = 0.45f; // <--- NOUVELLE VARIABLE : Écartement des gros boutons (multiplicateur de leur taille)

    float margin = 20.0f;
    float overlap = 8.0f;
    float sideLen = std::min(winW, winH) - 2 * margin;
    float btnSize = (sideLen + (numButtonsPerSide - 1) * overlap) / (float)numButtonsPerSide;
    
    auto addButton = [&](int id, float x, float y, float size) {
        Button b;
        b.rect.set(x, y, size, size);
        b.id = id;
        b.currentAlpha = 15; // Alpha léger de base
        b.targetAlpha = 15;
        // Couleur unique basée sur l'ID
        b.color.setHsb((id * 15) % 255, 200, 220);
        buttons.push_back(b);
    };

    float step = btnSize - overlap;

    // 1. Côté GAUCHE : Montant (Bas vers Haut)
    float xLeft = margin;
    float yBottom = winH - margin - btnSize;
    
    for(int i=0; i<numButtonsPerSide; i++) {
        float y = yBottom - i * step;
        addButton(i + 1, xLeft, y, btnSize);
    }

    // 2. Côté HAUT : Vers la Droite
    float yTop = margin;
    for(int i=0; i<numButtonsPerSide - 1; i++) {
        float x = xLeft + (i + 1) * step;
        addButton(numButtonsPerSide + 1 + i, x, yTop, btnSize);
    }

    // 3. Côté DROIT : Descendant
    float xRight = winW - margin - btnSize;
    for(int i=0; i<numButtonsPerSide - 1; i++) {
        float y = yTop + (i + 1) * step;
        addButton(numButtonsPerSide * 2 + i, xRight, y, btnSize);
    }

    // 4. Côté BAS : Vers la Gauche
    for(int i=0; i<numButtonsPerSide - 2; i++) {
        float x = xRight - (i + 1) * step;
        addButton(numButtonsPerSide * 3 - 1 + i, x, yBottom, btnSize);
    }

    // 5. TROIS GROS BOUTONS AU MILIEU
    if (numCenterButtons <= 0) return; // S'il n'y a pas de bouton, on arrête ici.

    // Le rayon du cercle est nul si on a un seul bouton, pour le centrer parfaitement.
    float circleRadius = (numCenterButtons > 1) ? (bigBtnSize * bigBtnSpacing) : 0.0f;

    float centerX = winW / 2.0f;
    float centerY = winH / 2.0f;
    int startId = (4 * numButtonsPerSide) - 3; // On continue l'ID après le dernier du bord
    
    for(int i=0; i<numCenterButtons; i++) {
        // Répartition sur un cercle.
        // Angle de départ à -90 degrés (en haut) pour avoir un bouton au sommet.
        float angle = -PI/2.0f + i * (TWO_PI / (float)numCenterButtons);
        float x = centerX + circleRadius * cos(angle) - bigBtnSize / 2.0f;
        float y = centerY + circleRadius * sin(angle) - bigBtnSize / 2.0f;
        addButton(startId + i, x, y, bigBtnSize);
    }
}

//--------------------------------------------------------------
void ButtonWindow::setupWorms(int count) {

    worms.clear();
    for(int i=0; i<count; i++) {
        ButtonWorm w;
        w.headPos.set(ofRandom(winW), ofRandom(winH));
        w.angle = ofRandom(TWO_PI);
        w.speed = ofRandom(2.0f, 5.0f);
        w.vel.set(cos(w.angle)*w.speed, sin(w.angle)*w.speed);
        w.numSegments = (int)ofRandom(5, 12);
        w.segments.assign(w.numSegments, w.headPos);
        w.color.setHsb(ofRandom(255), 200, 255);
        worms.push_back(w);
    }
}

//--------------------------------------------------------------
void ButtonWindow::update(float mx, float my) {
    hoveredIndices.clear();
    
    // 1. Update Worms Logic
    if(bDrawWorms) {
        for(auto& w : worms) {
            // Wander
            w.angle += ofRandom(-0.2f, 0.2f);
            w.vel.set(cos(w.angle)*w.speed, sin(w.angle)*w.speed);
            w.headPos += w.vel;

            // Bounce
            if(w.headPos.x < 0) { w.headPos.x = 0; w.vel.x *= -1; w.angle = atan2(w.vel.y, w.vel.x); }
            if(w.headPos.x > winW) { w.headPos.x = winW; w.vel.x *= -1; w.angle = atan2(w.vel.y, w.vel.x); }
            if(w.headPos.y < 0) { w.headPos.y = 0; w.vel.y *= -1; w.angle = atan2(w.vel.y, w.vel.x); }
            if(w.headPos.y > winH) { w.headPos.y = winH; w.vel.y *= -1; w.angle = atan2(w.vel.y, w.vel.x); }

            // Segments follow
            ofVec2f target = w.headPos;
            for(int i=0; i<w.segments.size(); i++) {
                ofVec2f dir = w.segments[i] - target;
                float dist = dir.length();
                float spacing = 15.0f;
                if(dist > spacing) {
                    w.segments[i] = target + dir.getNormalized() * spacing;
                }
                target = w.segments[i];
            }
        }
    }

    for(int i=0; i<buttons.size(); i++) {
        auto& b = buttons[i];
        bool isHovered = false;
        
        // Mouse Check
        if(b.rect.inside(mx, my)) isHovered = true;

        // External 3D Hover Check (ex: from RoomPreview)
        if(bHasExternalHover && b.rect.inside(externalHoverPos.x, externalHoverPos.y)) isHovered = true;

        // OSC Check
        if(g_oscButtonStates[b.id]) isHovered = true;
        
        // Toggled Check (Clic)
        if(b.isToggled) isHovered = true;

        // Worms Check
        if(bDrawWorms && !isHovered) {
            for(auto& w : worms) {
                if(b.rect.inside(w.headPos)) {
                    isHovered = true;
                    break;
                }
            }
        }

        if(isHovered) {
            b.targetAlpha = 200;
            hoveredIndices.push_back(i);
        } else {
            b.targetAlpha = 55;
        }
        
        // Animation fluide de l'alpha
        b.currentAlpha += (b.targetAlpha - b.currentAlpha) * 0.15f;
    }
}

//--------------------------------------------------------------
vector<ofVec3f> ButtonWindow::get3DPosForActiveButtons(float roomW, float roomSolD, float roomDepth) {
    vector<ofVec3f> positions;
    if(hoveredIndices.empty()) return positions;
    
    for(int idx : hoveredIndices) {
        ofRectangle r = buttons[idx].rect;
        ofVec2f center = r.getCenter();
        
        float pctX = center.x / winW;
        float pctY = center.y / winH;
        
        float d2 = roomDepth / 2.0f;
        float zCenterSol = -d2 + roomSolD / 2.0f;
        
        float x3d = -roomW/2.0f + pctX * roomW;
        float z3d = (zCenterSol - roomSolD/2.0f) + pctY * roomSolD;
        
        positions.push_back(ofVec3f(x3d, 2.0f, z3d));
    }
    return positions;
}

//--------------------------------------------------------------
void ButtonWindow::draw() {
    ofPushStyle();
    
    // Dessin du fond (Image ou Gris)
    // Alpha 20% environ (51/255)
    ofSetColor(255, 255, 255, 51);
    if(bgImg.isAllocated()) {
        bgImg.draw(0, 0, winW, winH);
    } else {
        ofSetColor(30, 51);
        ofDrawRectangle(0, 0, winW, winH);
    }
    
    // Dessin des carrés
    for(auto& b : buttons) {
        ofSetColor(b.color.r, b.color.g, b.color.b, b.currentAlpha);
        ofFill();
        ofDrawRectangle(b.rect);
        
        // Contour et Numéro
        ofNoFill();
        ofSetColor(255, 200);
        ofDrawRectangle(b.rect);
        
        ofSetColor(0); // Texte noir
        ofDrawBitmapString(ofToString(b.id), b.rect.x + b.rect.width/2 - 5, b.rect.y + b.rect.height/2 + 5);
    }

    // Dessin des Worms
    if(bDrawWorms) {
        ofNoFill();
        ofSetLineWidth(2);
        for(auto& w : worms) {
            ofSetColor(w.color);
            ofPolyline line;
            line.addVertex(w.headPos.x, w.headPos.y);
            for(auto& s : w.segments) line.addVertex(s.x, s.y);
            line.getSmoothed(3).draw();
        }
    }
    
    ofPopStyle();
}

//--------------------------------------------------------------
void ButtonWindow::checkClick(float mx, float my) {
    for(auto& b : buttons) {
        if(b.rect.inside(mx, my)) {
            b.isToggled = !b.isToggled; // Bascule l'état (On/Off)
        }
    }
}

//--------------------------------------------------------------
void ButtonWindow::checkExternalClick() {
    if(bHasExternalHover) {
        checkClick(externalHoverPos.x, externalHoverPos.y);
    }
}

//--------------------------------------------------------------
void ButtonWindow::drawPreview(float x, float y, float z, float w, float h) {
    ofPushStyle();
    ofEnableAlphaBlending(); // On force l'activation de la transparence car RoomPreview l'a désactivée
    ofPushMatrix();
    ofTranslate(x, y, z);
    
    // Rotation pour mettre à plat sur le sol (Plan X-Z)
    // 90 sur X pour orienter le bouton 1 vers le fond (Back/Jar)
    ofRotateXDeg(90);
    
    // Centrage (pour que x,y,z soit le centre du rectangle au sol)
    ofTranslate(-w * 0.5f, -h * 0.5f);
    
    // Dessin Fond
    ofSetColor(255, 255, 255, 51);
    if(bgImg.isAllocated()) {
       // bgImg.draw(0, 0, w, h);
    } else {
        //ofSetColor(30, 51);
       // ofDrawRectangle(0, 0, w, h);
    }
    
    // Dessin Boutons (Mise à l'échelle pour la 3D)
    float scaleX = w / winW;
    float scaleY = h / winH;
    
    for(auto& b : buttons) {
        ofSetColor(b.color.r, b.color.g, b.color.b, b.currentAlpha);
        ofFill();
        
        // Recalcul de la position scalée
        ofRectangle r = b.rect;
        r.x *= scaleX;
        r.y *= scaleY;
        r.width *= scaleX;
        r.height *= scaleY; 
        
        ofDrawRectangle(r);
        
        // Numéro
        ofSetColor(0);
        ofDrawBitmapString(ofToString(b.id), r.x + r.width/2 - 5, r.y + r.height/2 + 5);
    }
    
    // Dessin des Worms (Ajouté pour la preview)
    if(bDrawWorms) {
        ofNoFill();
        ofSetLineWidth(2);
        for(auto& w : worms) {
            ofSetColor(w.color);
            ofPolyline line;
            line.addVertex(w.headPos.x * scaleX, w.headPos.y * scaleY);
            for(auto& s : w.segments) line.addVertex(s.x * scaleX, s.y * scaleY);
            line.getSmoothed(3).draw();
        }
    }

    ofPopMatrix();
    ofPopStyle();
}
