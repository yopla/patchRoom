#include "LightningLayer.h"

//--------------------------------------------------------------
void LightningLayer::setup(float w, float h) {
    width = w;
    height = h;
    
    // FBO pour le Glow : 1/4 de la taille pour un flou naturel performant
    glowFbo.allocate(width / 4, height / 4, GL_RGBA);
    
    // Nettoyage initial
    glowFbo.begin(); ofClear(0,0,0,0); glowFbo.end();
}

//--------------------------------------------------------------
void LightningLayer::trigger(ofVec2f p1, ofVec2f p2) {
    Bolt b;
    
    b.start = p1; // Le 1er clic
    b.end   = p2; // Le 2eme clic
    
    b.life = 1.0f;
    b.decay = ofRandom(0.03, 0.06); 
    
    // L'épaisseur dépend un peu de la distance (plus c'est loin, plus c'est gros)
    float dist = p1.distance(p2);
    b.thickness = ofMap(dist, 0, 1000, 2.0, 5.0, true);
    
    // Couleurs : Cyan ou Violet
    if(ofRandom(1.0) > 0.5) b.color = ofColor(100, 200, 255); 
    else b.color = ofColor(200, 100, 255); 
    
    bolts.push_back(b);
}

//--------------------------------------------------------------
// Algorithme récursif "Midpoint Displacement"
void LightningLayer::createBoltPoints(ofVec2f p1, ofVec2f p2, float displacement, vector<ofVec2f>& pts) {
    if (displacement < 4.0f) { // Résolution du détail
        pts.push_back(p1);
    } else {
        ofVec2f mid = (p1 + p2) * 0.5f;
        // Décalage perpendiculaire
        ofVec2f perp = (p2 - p1).getNormalized().getRotated(90);
        mid += perp * ofRandom(-displacement, displacement);
        
        createBoltPoints(p1, mid, displacement * 0.55f, pts); // 0.55 pour réduire le chaos
        createBoltPoints(mid, p2, displacement * 0.55f, pts);
    }
}

//--------------------------------------------------------------
void LightningLayer::update(float mouseX, float mouseY, float time) {
    // Mise à jour des éclairs
    for(int i = bolts.size() - 1; i >= 0; i--) {
        bolts[i].life -= bolts[i].decay;
        
        if(bolts[i].life <= 0) {
            bolts.erase(bolts.begin() + i);
        } else {
            // "Vibration" : On régénère la géométrie à chaque frame pour l'effet crépitant
            bolts[i].points.clear();
            // L'amplitude du zigzag dépend de la vie (plus fort au début)
            float displ = 150.0f * bolts[i].life; 
            createBoltPoints(bolts[i].start, bolts[i].end, displ, bolts[i].points);
            bolts[i].points.push_back(bolts[i].end);
        }
    }
}

//--------------------------------------------------------------
void LightningLayer::draw() {
    if(bolts.empty()) return;

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD); // INDISPENSABLE pour l'effet lumineux

    // 1. DESSINER LE GLOW (dans le petit FBO)
    glowFbo.begin();
    ofClear(0, 0, 0, 0);
    
    for(auto& b : bolts) {
        ofSetLineWidth(b.thickness * 4.0); // Trait très large
        // Couleur avec Alpha lié à la vie
        ofSetColor(b.color, 150 * b.life); 
        
        ofMesh m;
        m.setMode(OF_PRIMITIVE_LINE_STRIP);
        for(auto& p : b.points) m.addVertex(ofVec3f(p.x, p.y, 0));
        m.draw();
    }
    glowFbo.end();

    // 2. RENDU FINAL A L'ECRAN
    ofPushMatrix();
    ofScale(scale, scale); // Si besoin de scaler

    // A. Afficher le Glow (étiré -> flou)
    ofSetColor(255); 
    glowFbo.draw(0, 0, width, height);

    // B. Afficher le Coeur Blanc (Net et précis)
    for(auto& b : bolts) {
        ofSetLineWidth(b.thickness);
        ofSetColor(255, 255, 255, 255 * b.life); // Blanc pur
        
        ofMesh m;
        m.setMode(OF_PRIMITIVE_LINE_STRIP);
        for(auto& p : b.points) m.addVertex(ofVec3f(p.x, p.y, 0));
        m.draw();
        
        // Petit cercle à l'impact
        ofDrawCircle(b.end.x, b.end.y, 10 * b.life);
    }
    
    ofPopMatrix();
    ofDisableBlendMode();
    ofPopStyle();
}