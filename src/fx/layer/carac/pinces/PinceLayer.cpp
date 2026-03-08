// /Users/ludo/Desktop/360MECA/_ofap/_of_v0.12.1_osx_release/_apps/myApps/patch-186/src/fx/layer/carac/PinceLayer.cpp
#include "PinceLayer.h"

//--------------------------------------------------------------
void PinceLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;
    
    // On ancre la pince au centre de l'écran
    basePos.set(w * 0.5f, h * 0.5f); 
    
    angles.assign(NARM, 0.0f);
}

//--------------------------------------------------------------
void PinceLayer::update(float mouseX, float mouseY, float time) {
    ofVec2f target(mouseX, mouseY);
    
    // Résolution IK : On itère plusieurs fois pour converger vers la cible
    // La ref AS3 utilise 10 itérations par frame
    ofVec2f e;
    for(int i=0; i<10; i++) {
        e = pursuit(0, basePos, target);
    }
    
    // Animation de la pince (ouverture/fermeture)
    // Basé sur la distance entre l'effecteur (e) et la cible
    float distSq = e.squareDistance(target);
    if(distSq < 50 * 50) {
        ph += 0.1f;
    }
}

//--------------------------------------------------------------
ofVec2f PinceLayer::pursuit(int bi, ofVec2f o, ofVec2f target) {
    ofVec2f e;
    
    // Calcul récursif de la position de l'extrémité
    if (bi < NARM - 1) {
        ofVec2f o2(o.x + cos(angles[bi]) * segLength, 
                   o.y + sin(angles[bi]) * segLength);
        e = pursuit(bi + 1, o2, target);
    } else {
        // Dernier segment : un peu plus long virtuellement pour l'IK (comme dans la ref)
        e.set(o.x + cos(angles[bi]) * (segLength + 20), 
              o.y + sin(angles[bi]) * (segLength + 20));
    }
    
    // Si l'effecteur est déjà sur la cible ou la cible sur l'origine, on arrête
    if ((e == o) || (target == o)) return e;
    
    // Calcul de l'angle différentiel pour orienter ce segment vers la cible
    float angleE = atan2(e.y - o.y, e.x - o.x);
    float angleP = atan2(target.y - o.y, target.x - o.x);
    float angleD = angleP - angleE;
    
    // Normalisation de l'angle (-PI, PI)
    while (angleD <= -PI) angleD += TWO_PI;
    while (angleD > PI) angleD -= TWO_PI;
    
    // Limitation de la vitesse angulaire (Amortissement / Rigidité)
    float maxSpeed = 0.05f; // Un peu plus réactif que la ref (0.025)
    if (angleD > maxSpeed) angleD = maxSpeed;
    else if (angleD < -maxSpeed) angleD = -maxSpeed;
    
    angles[bi] += angleD;
    
    return e;
}

//--------------------------------------------------------------
void PinceLayer::draw() {
    ofPushStyle();
    ofSetLineWidth(2);
    ofSetColor(255); // Blanc pour être visible sur fond noir
    ofNoFill();
    
    float x = basePos.x;
    float y = basePos.y;
    
    // Dessin des segments du bras
    for(int i=0; i<NARM; i++) {
        float nextX = x + cos(angles[i]) * segLength;
        float nextY = y + sin(angles[i]) * segLength;
        
        ofPushMatrix();
        ofTranslate(x, y);
        ofRotateRad(angles[i]);
        
        // Corps du segment (Rectangle arrondi)
        ofDrawRectRounded(-8, -8, segLength + 16, 16, 8);
        
        // Articulations
        ofDrawCircle(0, 0, 3);
        ofDrawCircle(0, 0, 8);
        
        ofPopMatrix();
        
        x = nextX;
        y = nextY;
    }
    
    // Dessin de l'effecteur (Pince)
    // Il est attaché au dernier segment
    float lastAngle = angles[NARM - 1];
    
    // Position visuelle de la pince :
    // Dans la ref, l'effecteur est à (e - 25). L'IK tip (e) est à Length+20.
    // Donc visuellement à (Length + 20 - 25) = Length - 5 du dernier joint.
    // 'x' et 'y' sont actuellement au bout du segment visuel (Length).
    // On recule donc de 5 pixels.
    float effX = x - cos(lastAngle) * 5.0f;
    float effY = y - sin(lastAngle) * 5.0f;
    
    // Angle d'ouverture oscillant
    float angleOpen = ofDegToRad(25 + 30 * pow(sin(ph), 2));
    
    ofPushMatrix();
    ofTranslate(effX, effY); 
    ofRotateRad(lastAngle);  // Orientation du bras
    
    // Mâchoire Supérieure
    ofPushMatrix();
    ofRotateRad(angleOpen);
    ofDrawRectangle(-5, -5, 30, 10);
    ofDrawRectangle(20, -10, 5, 15); // Crochet
    ofDrawCircle(0, 0, 8);
    ofPopMatrix();
    
    // Mâchoire Inférieure
    ofPushMatrix();
    ofRotateRad(-angleOpen);
    ofDrawRectangle(-5, -5, 30, 10);
    ofDrawRectangle(20, -5, 5, 15); // Crochet
    ofDrawCircle(0, 0, 3);
    ofPopMatrix();
    
    ofPopMatrix();
    
    ofPopStyle();
}
