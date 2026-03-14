#include "WalkingSquareLayer.h"

//--------------------------------------------------------------
// WALKING SQUARE
//--------------------------------------------------------------
void WalkingSquare::setup(float x, float y, float s) {
    pos.set(x, y);
    size = s;
    phase = ofRandom(TWO_PI);
    speed = ofRandom(4.0f, 6.0f); // Vitesse du cycle de marche
    
    // Couleur mi-transparente avec opacité de 160
    color = ofColor::fromHsb(ofRandom(255), 180, 255, 160);
}

void WalkingSquare::update(float dt) {
    phase += speed * dt;
    
    // Vitesse de déplacement mathématiquement calculée pour annuler
    // EXACTEMENT le recul du pied au sol (Aucun glissement)
    float stride = size * 0.45f;
    float bodySpeed = (2.0f * stride * speed) / PI;
    pos.x += bodySpeed * dt; 
}

void WalkingSquare::draw() {
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);

    // --- ANIMATION PARAMÉTRIQUE (CYCLE DE MARCHE) ---
    float p1 = phase;             // Coin inférieur gauche (Pied 1)
    float p2 = phase + PI;        // Coin inférieur droit (Pied 2)

    float stride = size * 0.45f;      // Longueur de la foulée vers l'avant/arrière
    float stepHeight = size * 0.4f;   // Hauteur de la levée du pied

    // Fonction lambda pour ancrer parfaitement les pieds au sol
    auto getFootPos = [&](float p, float baselineX, float& fx, float& fy) {
        float p_norm = fmod(p, TWO_PI);
        if(p_norm < 0) p_norm += TWO_PI;
        
        float t = p_norm / TWO_PI; // Progression dans le cycle (0.0 à 1.0)
        float localX = 0;

        if(t < 0.5f) {
            // EN L'AIR (Phase 0.0 à 0.5)
            // Le pied avance de -stride à +stride
            float u = t * 2.0f; // 0.0 à 1.0
            float easeU = u * u * (3.0f - 2.0f * u); // Smoothstep pour un levé/posé en douceur
            localX = ofLerp(-stride, stride, easeU);
            
            // Cloche pour la hauteur
            fy = size / 2.0f - sin(u * PI) * stepHeight;
        } else {
            // AU SOL (Phase 0.5 à 1.0)
            // Le pied recule de +stride à -stride
            // Mouvement strictement linéaire pour contrer exactement la vitesse du corps
            float u = (t - 0.5f) * 2.0f; // 0.0 à 1.0
            localX = ofLerp(stride, -stride, u);
            fy = size / 2.0f; // Parfaitement plat sur le sol
        }
        
        fx = baselineX + localX;
    };

    // Calcul des "Pieds" (Corners inférieurs)
    float blX, blY;
    getFootPos(p1, -size * 0.4f, blX, blY);

    float brX, brY;
    getFootPos(p2, size * 0.4f, brX, brY);

    // Le "Corps" (Corners supérieurs)
    // Rebond vertical (Squash & Stretch léger) 2 fois par cycle
    float bounce = abs(sin(phase)) * size * 0.15f;
    
    // Inclinaison (Lean) : Le carré se penche légèrement en avant et oscille
    float leanX = cos(phase * 2.0f) * size * 0.05f + size * 0.1f; 

    float tlX = -size / 2 + leanX;
    float tlY = -size / 2 + bounce;

    float trX =  size / 2 + leanX;
    float trY = -size / 2 + bounce;

    // Création de la géométrie déformée
    ofPolyline shape;
    shape.addVertex(tlX, tlY);
    shape.addVertex(trX, trY);
    shape.addVertex(brX, brY);
    shape.addVertex(blX, blY);
    shape.close();

    // 1. Dessin de l'intérieur (Couleur mi-transparente)
    ofFill();
    ofSetColor(color);
    ofBeginShape();
    for(auto& p : shape.getVertices()) ofVertex(p.x, p.y, 0);
    ofEndShape();

    // 2. Dessin du contour (Blanc épais)
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(8); 
    shape.draw();

    ofPopMatrix();
}

//--------------------------------------------------------------
// LAYER MANAGER
//--------------------------------------------------------------
void WalkingSquareLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;

    // Ajout de 3 carrés avec des tailles différentes en bas de l'écran
    for(int i = 0; i < 3; i++) {
        WalkingSquare sq;
        sq.setup(ofRandom(0, simWidth * 0.5f), simHeight - 100, ofRandom(60, 110));
        squares.push_back(sq);
    }
}

void WalkingSquareLayer::update(float mx, float my, float time) {
    float dt = 1.0f / 60.0f; // Constante de temps pour l'intégration
    for(auto& sq : squares) {
        sq.update(dt);
        // Wrapping horizontal
        if(sq.pos.x - sq.size > simWidth) {
            sq.pos.x = -sq.size;
        }
    }
}

void WalkingSquareLayer::draw() {
    ofPushStyle();
    for(auto& sq : squares) {
        sq.draw();
    }
    ofPopStyle();
}