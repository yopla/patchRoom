#include "CousinCon.h"

//--------------------------------------------------------------
CousinCon::CousinCon(float startX, float startY, ofImage* img) {
    texture = img;
    headX = startX;
    headY = startY;
    time = 0;
    isDragging = false;
    
    initArrays();
    
    // Initialisation des poils (Moins nombreux que le Cousin original pour laisser voir la texture)
    numHairs = 15; 
    for(int i=0; i<numHairs; i++) {
        CousinHair hair;
        
        hair.attIndex = numSegments;//(int)ofRandom(numSegments); // Répartis sur le corps
        hair.attPct = ofRandom(1.0f);
        hair.attSide = (ofRandom(1.0f) > 0.5f);
        
        float len = ofRandom(250, 550); 
        int segs = 15 + (int)ofRandom(5);
        
        hair.setup(len, segs);
        
        // Teinte verte pour aller avec le concombre
        hair.color = ofColor(100, 255, 100, 180);
        
        hairs.push_back(hair);
    }
}

CousinCon::~CousinCon() {
    hairs.clear();
}

void CousinCon::initArrays() {
    // Variation aléatoire
    numSegments = (int)ofRandom(4, 8); // Un peu plus de segments pour que la texture se courbe bien
    speedFactor = ofRandom(0.8f, 1.5f);
    
    // Resize vectors
    posx.assign(numSegments + 1, 0);
    posy.assign(numSegments + 1, 0);
    angles.assign(numSegments + 1, -90);
    
    lx.assign(numSegments + 1, 0); ly.assign(numSegments + 1, 0);
    rx.assign(numSegments + 1, 0); ry.assign(numSegments + 1, 0);
    lmx.assign(numSegments + 1, 0); lmy.assign(numSegments + 1, 0);
    rmx.assign(numSegments + 1, 0); rmy.assign(numSegments + 1, 0);

    torsions.assign(numSegments + 1, 0);
    phases.assign(numSegments + 1, 0);
    
    thickness.resize(numSegments + 1);
    lengths.resize(numSegments + 1);
    amplitudes.resize(numSegments + 1);
    freqs.resize(numSegments + 1);

    // Tête (0) - Plus large pour un concombre
    thickness[0] = ofRandom(25, 35); 
    lengths[0] = 0; amplitudes[0] = 0; freqs[0] = 0;
    
    // Corps - Garde une épaisseur assez constante
    for(int i=1; i<numSegments; i++) {
        thickness[i] = ofRandom(25, 35);
        lengths[i] = ofRandom(40, 60);
        amplitudes[i] = ofRandom(10, 50); // Ondulation modérée
        freqs[i] = ofRandom(1.5f, 3.0f);
    }
    
    // Queue (Dernier) - Arrondi
    thickness[numSegments] = 15;
    lengths[numSegments] = ofRandom(20, 40);
    amplitudes[numSegments] = ofRandom(20, 40);
    freqs[numSegments] = ofRandom(5, 8);
}

void CousinCon::update(float mx, float my) {
    time += speedFactor;
    
    if(isDragging) {
        headX = mx + dragOffset.x;
        headY = my + dragOffset.y;
    }
    
    updateBody();
    updateHairs();
}

void CousinCon::updateBody() {
    float dtr = PI / 180.0f;
    
    // Segment 0 (Tête)
    posx[0] = headX;
    posy[0] = headY;
    
    // Cinématique
    int i = 1;
    angles[i] = angles[i-1];
    posx[i] = posx[i-1] + lengths[i] * cos(angles[i] * dtr);
    posy[i] = posy[i-1] + lengths[i] * sin(angles[i] * dtr);
    
    for(i = 2; i <= numSegments; i++) {
        float wave = amplitudes[i] * sin(freqs[i] * (phases[i] + time) * dtr);
        angles[i] = angles[i-1] + torsions[i] + wave;
        
        posx[i] = posx[i-1] + lengths[i] * cos(angles[i] * dtr);
        posy[i] = posy[i-1] + lengths[i] * sin(angles[i] * dtr);
    }
    
    // Calcul des points de contour (épaisseur)
    // On utilise la même logique que CousinCreature pour avoir les points lx/rx
    
    // Segment 0
    rx[0] = posx[0] - thickness[0] * sin(angles[1] * dtr);
    ry[0] = posy[0] + thickness[0] * cos(angles[1] * dtr);
    lx[0] = posx[0] + thickness[0] * sin(angles[1] * dtr);
    ly[0] = posy[0] - thickness[0] * cos(angles[1] * dtr);
    
    for(i = 1; i < numSegments; i++) {
        float avgAngle = 0.5f * (angles[i+1] + angles[i]) * dtr;
        float diffAngle = 0.5f * (angles[i+1] - angles[i]) * dtr;
        
        float cosAngle = cos(diffAngle);
        if(abs(cosAngle) < 0.01f) cosAngle = (cosAngle < 0 ? -0.01f : 0.01f);
        
        float dist = thickness[i] / cosAngle;
        
        rx[i] = posx[i] - dist * sin(avgAngle);
        ry[i] = posy[i] + dist * cos(avgAngle);
        lx[i] = posx[i] + dist * sin(avgAngle);
        ly[i] = posy[i] - dist * cos(avgAngle);
    }
    
    i = numSegments;
    rx[i] = posx[i] - thickness[i] * sin(angles[i] * dtr);
    ry[i] = posy[i] + thickness[i] * cos(angles[i] * dtr);
    lx[i] = posx[i] + thickness[i] * sin(angles[i] * dtr);
    ly[i] = posy[i] - thickness[i] * cos(angles[i] * dtr);
}

void CousinCon::updateHairs() {
    for(auto& hair : hairs) {
        int i = hair.attIndex;
        if(i >= numSegments) i = numSegments - 1;
        float t = hair.attPct;
        
        float tx = (hair.attSide) ? ofLerp(lx[i], lx[i+1], t) : ofLerp(rx[i], rx[i+1], t);
        float ty = (hair.attSide) ? ofLerp(ly[i], ly[i+1], t) : ofLerp(ry[i], ry[i+1], t);
        
        // Rapprochement vers le centre (colonne vertébrale) pour que les poils sortent de l'intérieur
        float cx = ofLerp(posx[i], posx[i+1], t);
        float cy = ofLerp(posy[i], posy[i+1], t);
        
        float inset = 0.39f; // 90% de la distance vers le bord (1.0 = sur la peau)
        tx = ofLerp(cx, tx, inset);
        ty = ofLerp(cy, ty, inset);

        hair.update(tx, ty, time);
    }
}

void CousinCon::draw() {
    // 1. Dessin des poils (Derrière)
    for(auto& hair : hairs) hair.draw();
    
    // 2. Dessin du corps texturé
    ofSetColor(255);
    if(texture && texture->isAllocated()) {
        texture->bind();
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        float texW = texture->getWidth();
        float texH = texture->getHeight();
        
        // Correction : Vérifier si la texture utilise des coordonnées normalisées (0..1) ou pixels (ARB)
        // Si RoomPreview a désactivé l'ARB, les textures sont en GL_TEXTURE_2D (0..1)
        if(texture->getTexture().getTextureData().textureTarget == GL_TEXTURE_2D) {
            texW = 1.0f;
            texH = 1.0f;
        }

        // Crop Horizontal : On ne garde que le centre de l'image (ex: 25% à 75%) pour isoler le concombre
        float uStart = texW * 0;//0.25f; 
        float uEnd   = texW * 1;//0.75f;

        // --- LISSAGE SPLINE ---
        ofPolyline lPoly, rPoly;
        
        // On ajoute les points de contrôle pour la spline (Catmull-Rom)
        if(numSegments > 0) {
            lPoly.curveTo(lx[0], ly[0]);
            rPoly.curveTo(rx[0], ry[0]);
            
            for(int i=0; i<=numSegments; i++) {
                lPoly.curveTo(lx[i], ly[i]);
                rPoly.curveTo(rx[i], ry[i]);
            }
            
            lPoly.curveTo(lx[numSegments], ly[numSegments]);
            rPoly.curveTo(rx[numSegments], ry[numSegments]);
        }
        
        // Rééchantillonnage pour avoir une courbe lisse
        // On lisse d'abord la courbe brute pour éviter les micro-sauts
        lPoly = lPoly.getSmoothed(3);
        rPoly = rPoly.getSmoothed(3);
        
        int smoothRes = 300; 
        lPoly = lPoly.getResampledByCount(smoothRes);
        rPoly = rPoly.getResampledByCount(smoothRes);
        
        int count = lPoly.size();
        if(rPoly.size() < count) count = rPoly.size();

        for(int i=0; i<count; i++) {
            // Mapping UV vertical le long du corps
            float v = ofMap(i, 0, count-1, 0, texH);
            
            // Coté Gauche
            const auto& pL = lPoly[i];
            mesh.addVertex(ofVec3f(pL.x, pL.y, 0));
            mesh.addTexCoord(ofVec2f(uStart, v));
            
            // Coté Droit
            const auto& pR = rPoly[i];
            mesh.addVertex(ofVec3f(pR.x, pR.y, 0));
            mesh.addTexCoord(ofVec2f(uEnd, v));
        }
        mesh.draw();
        texture->unbind();
    }
}

// Interactions
bool CousinCon::isInside(float mx, float my) { return (ofDist(mx, my, headX, headY) < 50); }
void CousinCon::onPress(float mx, float my) { if(isInside(mx, my)) { isDragging = true; dragOffset.set(headX - mx, headY - my); } }
void CousinCon::onRelease(float mx, float my) { isDragging = false; }