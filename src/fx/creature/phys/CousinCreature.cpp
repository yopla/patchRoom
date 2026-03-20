#include "CousinCreature.h"

//--------------------------------------------------------------
// COUSIN HAIR (IK Logic)
//--------------------------------------------------------------
void CousinHair::setup(float len, int numSegs) {
    segments.resize(numSegs);
    float segLen = len / (float)numSegs;
    
    for(int i=0; i<numSegs; i++) {
        segments[i].x = 0;
        segments[i].y = 0;
        segments[i].angle = 0;
        segments[i].length = segLen;
    }
    
    // Comportements variés (Friction, Gravité)
    friction = ofRandom(0.8f, 0.98f); 
    gravity = 0; // ofRandom(0.5f, 0.9f); 
    color = ofColor(255, 255, 255, 200);
    offsetX = 0; offsetY = 0;
    uniqueOffset = ofRandom(1000.0f);
}

void CousinHair::update(float targetX, float targetY, float time, float scale) {
    currentScale = scale;
    // Drag du premier segment vers la cible
    float tx = targetX + offsetX;
    float ty = targetY + offsetY;
    
    for(int i=0; i<segments.size(); i++) {
        HairSegment& seg = segments[i];
        
        // Physique simple sur le segment (simule vx/vy de Flash)
        // Ici simplifié en IK pur avec gravité simulée sur l'angle
        
        float dx = tx - seg.x;
        float dy = ty - seg.y;
        
        // Ajout gravité
        dy += gravity; 
        
        // Ajout de turbulence pour écarter les poils (Noise)
        // Le bruit dépend du temps, de l'index du segment et de l'identité du poil
        float noise = ofSignedNoise(time * 0.01f, i * 0.2f, uniqueOffset * 0.01f);
        
        float angle = atan2(dy, dx) + noise * 0.15f; // Deviation réduite
        seg.angle = angle;
        
        // On place le segment pour que sa "tête" touche la cible (tx, ty)
        // Dans Flash: x = target - cos(angle)*len
        float currentSegLen = seg.length * currentScale;
        seg.x = tx - cos(angle) * currentSegLen;
        seg.y = ty - sin(angle) * currentSegLen;
        
        // La cible du prochain segment est la "queue" de celui-ci
        tx = seg.x;
        ty = seg.y;
    }
}

void CousinHair::draw() {
    // Dessin avec dégradé alpha (Mesh)
    // Utilisation de ofPolyline pour lisser (Spline)
    ofSetLineWidth(3.0f);
    ofPolyline poly;

    if(segments.size() > 0) {
        float currentSegLen = segments[0].length * currentScale;
        float startX = segments[0].x + cos(segments[0].angle) * currentSegLen;
        float startY = segments[0].y + sin(segments[0].angle) * currentSegLen;
        
        poly.addVertex(startX, startY);
        
        for(int i=0; i<segments.size(); i++) {
            poly.addVertex(segments[i].x, segments[i].y);
        }
    }
    
    // Lissage de la courbe
    ofPolyline smoothed = poly.getSmoothed(4); 
    
    // Correction : Forcer le premier point (racine) à rester fixe sur l'ancrage malgré le lissage
    if(smoothed.size() > 0 && poly.size() > 0) {
        smoothed[0] = poly[0];
    }
    
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    vector<glm::vec3> verts = smoothed.getVertices();
    for(int i=0; i<verts.size(); i++) {
        mesh.addVertex(verts[i]);
        float alpha = ofMap(i, 0, verts.size()-1, 255, 0);
        mesh.addColor(ofColor(color.r, color.g, color.b, alpha));
    }
    mesh.draw();
}

//--------------------------------------------------------------
// COUSIN CREATURE (Body Logic)
//--------------------------------------------------------------
CousinCreature::CousinCreature(float startX, float startY) {
    headX = startX;
    headY = startY;
    time = 0;
    isDragging = false;
    bHovered = false;
    hairScale = 1.0f;
    
    initArrays();
    
    // Initialisation des poils (TailLine.as)
    numHairs = 30; 
    for(int i=0; i<numHairs; i++) {
        CousinHair hair;
        
        hair.attIndex = numSegments;// (int)ofRandom(numSegments - 1); // Plus haut sur le corps
        hair.attPct = ofRandom(1.0f);
        hair.attSide = (ofRandom(1.0f) > 0.5f);
        
        // Longueur variable
        float len = ofRandom(200, 450); // Plus long à la base comme demandé
        int segs = 10 + (int)ofRandom(5);
        
        hair.setup(len, segs);
        hairs.push_back(hair);
    }
}

CousinCreature::~CousinCreature() {
    hairs.clear();
}

void CousinCreature::initArrays() {
    // Variation aléatoire pour chaque créature
    numSegments = (int)ofRandom(3, 7); // Entre 3 et 6 segments
    speedFactor = ofRandom(0.8f, 1.5f); // Vitesse variable
    
    // Resize vectors
    posx.assign(numSegments + 1, 0);
    posy.assign(numSegments + 1, 0);
    angles.assign(numSegments + 1, -90); // angle1
    
    lx.assign(numSegments + 1, 0); ly.assign(numSegments + 1, 0);
    rx.assign(numSegments + 1, 0); ry.assign(numSegments + 1, 0);
    lmx.assign(numSegments + 1, 0); lmy.assign(numSegments + 1, 0);
    rmx.assign(numSegments + 1, 0); rmy.assign(numSegments + 1, 0);

    torsions.assign(numSegments + 1, 0);
    phases.assign(numSegments + 1, 0);
    
    // Génération procédurale des paramètres
    thickness.resize(numSegments + 1);
    lengths.resize(numSegments + 1);
    amplitudes.resize(numSegments + 1);
    freqs.resize(numSegments + 1);

    // Tête (0)
    thickness[0] = ofRandom(9, 12); lengths[0] = 0; amplitudes[0] = 0; freqs[0] = 0;
    
    // Corps
    for(int i=1; i<numSegments; i++) {
        thickness[i] = ofRandom(6, 11);
        lengths[i] = ofRandom(30, 70);
        amplitudes[i] = ofRandom(20, 100);
        freqs[i] = ofRandom(1.5f, 4.0f);
    }
    
    // Queue (Dernier)
    thickness[numSegments] = 1;
    lengths[numSegments] = ofRandom(15, 30);
    amplitudes[numSegments] = ofRandom(40, 60);
    freqs[numSegments] = ofRandom(8, 12);
}

void CousinCreature::update(float mx, float my) {
    time += speedFactor; // glob.T++ avec vitesse variable
    
    // Gestion souris / Position Tête
    if(isDragging) {
        headX = mx + dragOffset.x;
        headY = my + dragOffset.y;
    } else {
        // Reste fixe (ne suit plus la souris)
        // headX/headY restent à leur position initiale
    }
    
    // Dynamique de rétractation des poils sur 35 frames et 55 frames (ragrandit)
    float minScale = 0.15f;
    if (bHovered) {
        hairScale = max(minScale, hairScale - (1.0f - minScale) / 35.0f);
    } else {
        hairScale = std::min(1.0f, hairScale + (1.0f - minScale) / 55.0f);
    }

    updateBody();
    updateHairs();
}

void CousinCreature::updateBody() {
    float dtr = PI / 180.0f;
    
    // Segment 0 (Tête)
    posx[0] = headX;
    posy[0] = headY;
    
    // Calcul de la chaîne cinématique (corp.as)
    int i = 1;
    
    // Segment 1
    angles[i] = angles[i-1];
    posx[i] = posx[i-1] + lengths[i] * cos(angles[i] * dtr);
    posy[i] = posy[i-1] + lengths[i] * sin(angles[i] * dtr);
    
    // Segments suivants avec ondulation
    for(i = 2; i <= numSegments; i++) {
        // a[i] = a[i-1] + glob.tors[i] + da[i]*Math.sin(frec[i]*(glob.depart[i]+t)*dtr);
        float wave = amplitudes[i] * sin(freqs[i] * (phases[i] + time) * dtr);
        angles[i] = angles[i-1] + torsions[i] + wave;
        
        posx[i] = posx[i-1] + lengths[i] * cos(angles[i] * dtr);
        posy[i] = posy[i-1] + lengths[i] * sin(angles[i] * dtr);
    }
    
    // Calcul des points de contour (épaisseur)
    // Segment 0
    rx[0] = posx[0] - thickness[0] * sin(angles[1] * dtr);
    ry[0] = posy[0] + thickness[0] * cos(angles[1] * dtr);
    lx[0] = posx[0] + thickness[0] * sin(angles[1] * dtr);
    ly[0] = posy[0] - thickness[0] * cos(angles[1] * dtr);
    
    // Segments intermédiaires
    for(i = 1; i < numSegments; i++) {
        float avgAngle = 0.5f * (angles[i+1] + angles[i]) * dtr;
        float diffAngle = 0.5f * (angles[i+1] - angles[i]) * dtr;
        float dist = thickness[i] / cos(diffAngle);
        
        rx[i] = posx[i] - dist * sin(avgAngle);
        ry[i] = posy[i] + dist * cos(avgAngle);
        lx[i] = posx[i] + dist * sin(avgAngle);
        ly[i] = posy[i] - dist * cos(avgAngle);
        
        // Midpoints pour Bezier quadratique
        rmx[i] = 0.5f * (rx[i] + rx[i-1]);
        rmy[i] = 0.5f * (ry[i] + ry[i-1]);
        lmx[i] = 0.5f * (lx[i] + lx[i-1]);
        lmy[i] = 0.5f * (ly[i] + ly[i-1]);
    }
    
    // Dernier segment
    i = numSegments;
    rx[i] = posx[i] - thickness[i] * sin(angles[i] * dtr);
    ry[i] = posy[i] + thickness[i] * cos(angles[i] * dtr);
    lx[i] = posx[i] + thickness[i] * sin(angles[i] * dtr);
    ly[i] = posy[i] - thickness[i] * cos(angles[i] * dtr);
    
    // Point d'attache des poils (transfertpoil)
    hairTargetX = (rx[numSegments] + lx[numSegments]) / 2.0f;
    hairTargetY = (ry[numSegments] + ly[numSegments]) / 2.0f;
}

void CousinCreature::updateHairs() {
    for(auto& hair : hairs) {
        int i = hair.attIndex;
        if(i >= numSegments) i = numSegments - 1;
        float t = hair.attPct;
        
        float tx, ty;
        if(hair.attSide) {
            tx = ofLerp(lx[i], lx[i+1], t);
            ty = ofLerp(ly[i], ly[i+1], t);
        } else {
            tx = ofLerp(rx[i], rx[i+1], t);
            ty = ofLerp(ry[i], ry[i+1], t);
        }
        hair.update(tx, ty, time, hairScale);
    }
}

void CousinCreature::draw() {
    // 1. Dessin des poils (Derrière le corps)
    for(auto& hair : hairs) {
        hair.draw();
    }
    
    // 2. Dessin du corps
    // Construction de la liste des points pour le contour
    vector<ofVec2f> points;
    // Top head
    points.push_back(ofVec2f(lx[0], ly[0]));
    points.push_back(ofVec2f(rx[0], ry[0]));
    // Right side down
    for(int i = 1; i <= numSegments; i++) points.push_back(ofVec2f(rx[i], ry[i]));
    // Left side up
    for(int i = numSegments; i >= 1; i--) points.push_back(ofVec2f(lx[i], ly[i]));

    auto drawSplineShape = [&](const vector<ofVec2f>& pts) {
        ofBeginShape();
        if(pts.size() > 3) {
            ofCurveVertex(pts.back().x, pts.back().y); // Control point
            for(const auto& p : pts) ofCurveVertex(p.x, p.y);
            ofCurveVertex(pts[0].x, pts[0].y); // Close
            ofCurveVertex(pts[1].x, pts[1].y); // Control point
        }
        ofEndShape(true);
    };

    ofNoFill();
    ofSetColor(255); // Contour blanc
    ofSetLineWidth(2);
    drawSplineShape(points); // Contour

    // Debug points (optionnel)
    /*
    ofSetColor(255, 0, 0);
    for(int i=0; i<=numSegments; i++) ofDrawCircle(posx[i], posy[i], 2);
    */
}

//--------------------------------------------------------------
// INTERACTIONS
//--------------------------------------------------------------
bool CousinCreature::isInside(float mx, float my) {
    // Zone simple autour de la tête
    return (ofDist(mx, my, headX, headY) < 50);
}

void CousinCreature::onPress(float mx, float my) {
    if(isInside(mx, my)) {
        isDragging = true;
        dragOffset.set(headX - mx, headY - my);
    }
}

void CousinCreature::onRelease(float mx, float my) {
    isDragging = false;
}