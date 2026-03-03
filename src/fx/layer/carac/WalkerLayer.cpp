#include "WalkerLayer.h"

//--------------------------------------------------------------
void WalkerLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h; // Devrait être 830 selon ta demande
    
    pos.set(w * 0.5f, h - 50); // Départ en bas
    angleCycle = 0;
    
    // Création des 4 jambes (comme dans le script AS3)
    int legNum = 4;
    legs.resize(legNum);
    
    for(int i = 0; i < legNum; i++) {
        legs[i].rOffset = TWO_PI * (i * 1.5f) / (float)legNum;
        legs[i].length = 100.0f; // Longueur de base (sera scalée au draw)
        legs[i].rMul = ofRandom(0.2f, 1.2f);
        legs[i].currentH = 0;
    }
}

//--------------------------------------------------------------
float WalkerLayer::getShortestDist(float current, float target, float w) {
    float dx = target - current;
    if (dx > w * 0.5f) dx -= w;
    else if (dx < -w * 0.5f) dx += w;
    return dx;
}

//--------------------------------------------------------------
void WalkerLayer::update(float mouseX, float mouseY, float time) {
    // 1. Calcul de la distance vers la souris (avec Wrapping)
    // Le "0.1" est le easing factor du script original
    float dx = getShortestDist(pos.x, mouseX, simWidth) * 0.05f; 
    
    // 2. Déplacement horizontal
    pos.x += dx;
    
    // Wrapping de la position
    if (pos.x < 0) pos.x += simWidth;
    if (pos.x > simWidth) pos.x -= simWidth;

    // 3. Mise à jour du cycle de marche ('r' dans le script AS3)
    // Plus on va vite, plus l'angle tourne
    angleCycle = fmod(angleCycle + dx * 0.02f, TWO_PI);

    // 4. Calcul de la cinématique des jambes pour trouver la hauteur du corps
    // Le script AS3 ajuste la hauteur du corps pour que le pied le plus bas touche le sol "virtuel"
    // Ici, le sol est à Y = 0 (relatif au walker)
    
    float maxLegY = -9999.0f; // On cherche le Y le plus grand (le plus bas à l'écran)

    for(int i = 0; i < legs.size(); i++) {
        // Logique mathématique portée du script AS3 (Point.polar logic)
        // var pt:Point = Point.polar(leg.length/2, Math.abs(Math.sin(r + leg.r)) * (i %2 ==0 ? -1 : 1) *1.0 + Math.PI / 2);
        
        float dir = (i % 2 == 0) ? -1.0f : 1.0f;
        float theta = abs(sin(angleCycle + legs[i].rOffset)) * dir * 1.0f + HALF_PI;
        
        // Point.polar en C++ : x = r * cos(theta), y = r * sin(theta)
        // Attention Flash et OF ont Y inversé pareil, mais Point.polar utilise le cercle trigo standard.
        // Dans Flash, Angle 0 est à droite, PI/2 en bas.
        
        float polarR = legs[i].length * 0.5f;
        float pY = polarR * sin(theta); 
        
        // Dans le script AS3: height is pt.y * 2
        float footY = pY * 2.0f;
        
        legs[i].currentH = footY; // On stocke pour le draw
        
        if (footY > maxLegY) {
            maxLegY = footY;
        }
    }
    
    // AS3: legs.y = 300 - heights[0];
    // Ici on positionne le "pivot" des jambes pour que le pied le plus bas touche le sol (simHeight)
    // pos.y est la position du pivot des hanches
    
    // On veut que (pos.y + maxLegY) = simHeight
    float targetY = simHeight - maxLegY * globalScale; // * globalScale car les jambes sont dessinées scalées
    
    // Lissage vertical (optionnel, mais rend le mouvement plus fluide)
    pos.y += (targetY - pos.y) * 0.1f;
}

//--------------------------------------------------------------
void WalkerLayer::draw() {
    // Gestion du Wrapping (Fantômes)
    drawWalker(0, 0);
    
    if(pos.x < 300) drawWalker(simWidth, 0);
    else if(pos.x > simWidth - 300) drawWalker(-simWidth, 0);
}

//--------------------------------------------------------------
void WalkerLayer::drawWalker(float offsetX, float offsetY) {
    ofPushMatrix();
    ofTranslate(pos.x + offsetX, pos.y + offsetY);
    ofScale(globalScale, globalScale); // Mise à l'échelle globale
    
    // Le script AS3 dessine le corps par rapport aux jambes
    // body.y = legs.y - body.height * 0.75;
    float localBodyY = - bodyH * 0.75f;
    
    // 1. Dessin des Jambes
    ofSetColor(0); // Noir
    ofSetLineWidth(16); // Épaisseur comme dans AS3
    ofSetCircleResolution(20);

    for(int i = 0; i < legs.size(); i++) {
        // AS3 Logic pour X:
        // var lx:Number = (Math.ceil(i/2)) * 60 / legs.numChildren * ((i % 2 == 0) ? 1 : -1) ...
        float dir = (i % 2 == 0) ? 1.0f : -1.0f;
        float lx = ceil(i / 2.0f) * 60.0f / legs.size() * dir;
        if (legs.size() % 2 == 0) lx += (30.0f / legs.size());
        
        // Recalcul du point de genou (pt) pour le dessin
        // On refait le calcul fait dans update pour avoir les coords exactes
        float dirTheta = (i % 2 == 0) ? -1.0f : 1.0f;
        float theta = abs(sin(angleCycle + legs[i].rOffset)) * dirTheta * 1.0f + HALF_PI;
        
        float polarR = legs[i].length * 0.5f;
        float ptX = polarR * cos(theta);
        float ptY = polarR * sin(theta);
        
        // Dessin "Pli de la jambe" (Triangle strip style lineTo dans AS3)
        // moveTo(lx, 0) -> lineTo(lx + pt.x, pt.y) -> lineTo(lx, pt.y*2)
        
        ofDrawLine(lx, 0, lx + ptX, ptY);      // Hanche -> Genou
        ofDrawLine(lx + ptX, ptY, lx, ptY * 2); // Genou -> Pied
        
        // Optionnel : petits cercles aux jointures pour faire "propre" (Round Join simulation)
        ofDrawCircle(lx, 0, 7);
        ofDrawCircle(lx + ptX, ptY, 7);
        ofDrawCircle(lx, ptY*2, 7);
    }
    
    // 2. Dessin du Corps
    ofPushMatrix();
    ofTranslate(0, localBodyY);
    
    // Corps Noir (Rounded Rect)
    // AS3: drawRoundRect(-40, 0, 80, 60, 60, 60); -> C'est une ellipse presque parfaite ou capsule
    ofSetColor(0);
    ofDrawRectRounded(-40, 0, 80, 60, 30); // 30 = rayon des coins (moitié de hauteur 60)
    
    // Yeux (Blancs)
    ofSetColor(255);
    // AS3: (-20, 20) et (20, 20)
    ofDrawCircle(-20, 20, 4);
    ofDrawCircle(20, 20, 4);
    
    ofPopMatrix(); // Fin Corps
    
    ofPopMatrix(); // Fin Global
}