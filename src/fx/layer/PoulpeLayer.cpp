#include "PoulpeLayer.h"

// --------------------------------------------------------
// LIMB (SEGMENT)
// --------------------------------------------------------
void Limb::setup(float startX, float startY, float s) {
    x = startX;
    y = startY;
    tx = startX;
    ty = startY;
    size = s;
    noiseOffset = ofRandom(1000);
}

// --------------------------------------------------------
// TENTACLE
// --------------------------------------------------------
void Tentacle::setup(float x, float y, float ang, float lengthScale) {
    angleOffset = ang;
    int numSegments = 50;
    
    segments.resize(numSegments);
    for(int i=0; i<numSegments; i++) {
        // La taille diminue vers le bout du tentacule
        float s = (numSegments - (i * 1.75f) + 25) * 1.25f; // 0.3f pour adapter l'échelle Processing à OF
        if(s < 1) s = 1;
        segments[i].setup(x, y, s);
    }
}

void Tentacle::update(float headX, float headY, float simWidth, float simHeight) {
    // Calcul de la position idéale de la base du tentacule (autour de la tête)
    // On ajoute du mouvement "organique" avec sin/cos
    float wave = ofGetFrameNum() * (2.0f / 60.0f);
    float baseX = headX + (stretchMult * cos(angleOffset + wave * 0.1));
    float baseY = headY + (stretchMult * sin(angleOffset + wave * 0.1));

    for(int i=0; i<segments.size(); i++) {
        float targetX, targetY;

        // Si c'est le premier segment, il suit la tête
        if(i == 0) {
            targetX = baseX;
            targetY = baseY;
        } else {
            // Sinon il suit le segment précédent
            targetX = segments[i-1].x;
            targetY = segments[i-1].y;
        }

        // Ajout du bruit de Perlin (comme dans le code original)
        // Note: adaptation des constantes pour OF
        float noiseX = (ofNoise((targetX + ofGetFrameNum()) * 0.008f, segments[i].noiseOffset) - 0.5f) * 20.0f;
        float noiseY = (ofNoise(segments[i].noiseOffset, (targetY + ofGetFrameNum()) * 0.008f) - 0.5f) * 20.0f;
        
        targetX += noiseX;
        targetY += noiseY;

        // Easing (Frottement)
        float easing = 0.3f; // Valeur du code original
        
        // --- LOGIQUE DE WRAPPING POUR LES SEGMENTS ---
        // Si le parent a "sauté" de l'autre côté de l'écran, le segment doit suivre intelligemment
        float dx = targetX - segments[i].x;
        
        // Si la distance est trop grande, c'est qu'il y a eu un wrap
        if(dx > simWidth * 0.5f) dx -= simWidth;
        else if(dx < -simWidth * 0.5f) dx += simWidth;

        segments[i].x += dx * easing;
        segments[i].y += (targetY - segments[i].y) * easing;

        // Wrapping strict de la position
        if(segments[i].x < 0) segments[i].x += simWidth;
        if(segments[i].x > simWidth) segments[i].x -= simWidth;
    }
}
// --------------------------------------------------------
// TENTACLE :: DRAW (CORRIGÉ)
// --------------------------------------------------------
void Tentacle::draw(float offsetX, float offsetY, ofColor col, float extraWidth, float simWidth) {
    
    for(int i=0; i<segments.size()-1; i++) {
        ofSetColor(col);

        // L'épaisseur = taille du segment + épaisseur extra
        ofSetLineWidth(segments[i].size + extraWidth);
        
        // --- LOGIQUE DE CORRECTION DU WRAP ---
        float x1 = segments[i].x;
        float y1 = segments[i].y;
        float x2 = segments[i+1].x;
        float y2 = segments[i+1].y;
        
        // Calcul de la distance brute en X
        float dist = x2 - x1;
        
        // Si la distance est supérieure à la moitié de la scène, c'est un wrap !
        // On rapproche virtuellement x2 de x1 pour le dessin
        if (abs(dist) > simWidth * 0.5f) {
            if (dist > 0) {
                x2 -= simWidth; // x2 était trop à droite, on le ramène à gauche
            } else {
                x2 += simWidth; // x2 était trop à gauche, on le ramène à droite
            }
        }
        
        // Dessin de la ligne
        ofDrawLine(x1 + offsetX, y1 + offsetY, 
                   x2 + offsetX, y2 + offsetY);
        
        // Dessin du joint (cercle)
        // Note : On utilise (segments[i].size + extraWidth) * 0.74 comme dans ton code précédent
        if(col.getHex() != 0x000000){ // Petite astuce pour éviter de redessiner le cercle noir si on veut
             ofDrawCircle(x1 + offsetX, y1 + offsetY, (segments[i].size + extraWidth) * 0.74f);
        } else {
             // Si c'est le contour noir, on dessine aussi pour boucher les trous
             ofDrawCircle(x1 + offsetX, y1 + offsetY, (segments[i].size + extraWidth) * 0.74f);
        }
    }
}

// --------------------------------------------------------
// POULPE LAYER (MAIN CLASS)
// --------------------------------------------------------

void PoulpeLayer::setup(float width, float height) {
    simWidth = width;
    simHeight = height;
    
    headPos.set(width/2, height/2);
    currentPos = headPos;
    targetX = width/2;
    targetY = height/2;

    // Création de 8 tentacules (comme l'original loop 8x)
    int numTentacles = 8;
    float angleStep = (2 * PI) / (float)numTentacles;
    
    for(int i=0; i<numTentacles; i++) {
        Tentacle t;
        // On passe l'angle pour qu'ils soient répartis autour de la tête
        t.setup(headPos.x, headPos.y, i * angleStep, 1.0f);
        tentacles.push_back(t);
    }
}

void PoulpeLayer::setTarget(float x, float y) {
    targetX = x;
    targetY = y;
}

// Utilitaire pour calculer la distance signée la plus courte sur un axe qui boucle
float PoulpeLayer::getShortestDist(float current, float target, float w) {
    float dx = target - current;
    if (dx > w * 0.5f) dx -= w;
    else if (dx < -w * 0.5f) dx += w;
    return dx;
}

void PoulpeLayer::update() {
    // 1. Déplacement de la Tête avec Shortest Path
    float dx = getShortestDist(currentPos.x, targetX, simWidth);
    float dy = targetY - currentPos.y; // Pas de wrap vertical en général, sinon idem

    // Ajout d'un bruit sur la tête aussi (comme original frameCount noise)
    float noiseHeadX = (-0.5 + ofNoise((targetX + ofGetFrameNum() - 50) * 0.008f, 0)) * 120.0f;
    float noiseHeadY = (-0.5 + ofNoise(0, (targetY + ofGetFrameNum() - 50) * 0.008f)) * 120.0f;

    // Application de la force vers la cible + bruit
    // Note: easing * 0.3 pour ralentir un peu comme dans l'original
    currentPos.x += (dx + noiseHeadX) * (easing * 0.5); 
    currentPos.y += (dy + noiseHeadY) * (easing * 0.5);

    // Wrapping de la position physique
    if(currentPos.x < 0) currentPos.x += simWidth;
    if(currentPos.x > simWidth) currentPos.x -= simWidth;
    
    if(currentPos.y < 0) currentPos.y = 0;
    if(currentPos.y > simHeight) currentPos.y = simHeight;

    headPos = currentPos;

    // 2. Mise à jour des tentacules
    for(auto& t : tentacles) {
        t.update(headPos.x, headPos.y, simWidth, simHeight);
    }
}
void PoulpeLayer::draw() {
    // Gestion du wrapping (dessin fantôme si proche des bords)
    vector<ofVec2f> offsets;
    offsets.push_back(ofVec2f(0,0)); 
    if(headPos.x < 300) offsets.push_back(ofVec2f(simWidth, 0));
    else if(headPos.x > simWidth - 300) offsets.push_back(ofVec2f(-simWidth, 0));

    for(auto& offset : offsets) {
        float ox = offset.x;
        float oy = offset.y;
        float drawX = headPos.x + ox;
        float drawY = headPos.y + oy;

        // --- COUCHE 1 : CONTOURS NOIRS ---
        for(auto& t : tentacles) {
            // AJOUT DU PARAMETRE simWidth à la fin
            t.draw(ox, oy, ofColor(0), 15.0f, simWidth); 
        }

        // --- COUCHE 2 : TÊTE (Contour Noir) ---
        ofSetColor(0);
        ofDrawCircle(drawX, drawY, 35 + 2); 

        // --- COUCHE 3 : CORPS VERT ---
        for(auto& t : tentacles) {
            // AJOUT DU PARAMETRE simWidth à la fin
            t.draw(ox, oy, ofColor(75, 200, 0), 0.0f, simWidth);
        }

        // --- COUCHE 4 : DETAILS TETE ---
        ofSetColor(75, 200, 0); 
        ofDrawCircle(drawX, drawY, 35); 

        // Blanc des yeux
        ofSetColor(255); 
        ofDrawEllipse(drawX + 10, drawY - 6, 8, 5);
        ofDrawEllipse(drawX - 10, drawY - 6, 8, 5);
        
        // Pupilles
        ofSetColor(0); 
        ofDrawEllipse(drawX + 10, drawY - 6, 1, 1);
        ofDrawEllipse(drawX - 10, drawY - 6, 1, 1);

        // Bouche
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(0);
        ofDrawCurve(drawX - 10, drawY + 6, 
                    drawX - 7, drawY + 12,
                    drawX + 7, drawY + 12,
                    drawX + 10, drawY + 6);
        ofFill();
        ofSetLineWidth(1);
    }
}