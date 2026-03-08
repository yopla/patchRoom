#include "GearLayer.h"

//--------------------------------------------------------------
void GearLayer::setup(float w, float h) {
    simWidth = w;
    simHeight = h;

    // Création d'une chaîne d'engrenages en bas de l'écran
    // On les place pour qu'ils soient partiellement visibles derrière le sol
    
    float startX = w * 0.3f;
    float groundY = h - 100; // Un peu au dessus du bas absolu

    // Engrenage 1 (Gros)
    Gear g1;
    g1.pos.set(startX, groundY - 150);
    g1.radius = 120;
    g1.teethCount = 24;
    g1.angle = 0;
    g1.angularVel = 0;
    g1.color = ofColor(100, 90, 80);
    gears.push_back(g1);

    // Engrenage 2 (Petit, connecté au 1)
    Gear g2;
    // On le place à droite, distance = r1 + r2 - overlap
    g2.radius = 60;
    // MODIFICATION: Augmentation de l'imbrication pour un meilleur effet visuel
    g2.pos.set(g1.pos.x + g1.radius + g2.radius - 20, groundY - 80); 
    g2.teethCount = 12;
    g2.angle = 0;
    g2.angularVel = 0;
    g2.color = ofColor(120, 110, 100);
    gears.push_back(g2);

    // Engrenage 3 (Moyen, connecté au 2)
    Gear g3;
    g3.radius = 90;
    // MODIFICATION: Augmentation de l'imbrication
    g3.pos.set(g2.pos.x + g2.radius + g3.radius - 20, groundY - 120);
    g3.teethCount = 18;
    g3.angle = 0;
    g3.angularVel = 0;
    g3.color = ofColor(90, 80, 70);
    gears.push_back(g3);
    
    // Engrenage 4 (Gros, connecté au 3)
    Gear g4;
    g4.radius = 140;
    // MODIFICATION: Augmentation de l'imbrication
    g4.pos.set(g3.pos.x + g3.radius + g4.radius - 25, groundY - 50);
    g4.teethCount = 28;
    g4.angle = 0;
    g4.angularVel = 0;
    g4.color = ofColor(80, 70, 60);
    gears.push_back(g4);
}

//--------------------------------------------------------------
void GearLayer::addSquare() {
    FallingSquare s;
    // Apparition aléatoire au dessus des engrenages
    float minX = gears[0].pos.x - 100;
    float maxX = gears.back().pos.x + 100;
    
    s.pos.set(ofRandom(minX, maxX), -100); // Commence en haut
    s.vel.set(ofRandom(-2, 2), ofRandom(5, 10));
    // MODIFICATION: Carrés plus petits
    s.size = ofRandom(8, 12);
    s.angle = ofRandom(360);
    s.rotSpeed = ofRandom(-5, 5);
    s.bDead = false;
    squares.push_back(s);
}

//--------------------------------------------------------------
void GearLayer::update(float mouseX, float mouseY, float time) {
    
    // 1. Spawn automatique de carrés
    if(ofRandom(1.0) < 0.015) { // 0.5% de chance par frame (Moins fréquent)
        addSquare();
    }

    // 2. Physique des Carrés
    for(auto& s : squares) {
        // Gravité
        s.vel.y += 0.4;
        s.pos += s.vel;
        s.angle += s.rotSpeed;
        
        // Collision avec les Engrenages
        for(auto& g : gears) {
            float dist = s.pos.distance(g.pos);
            float outerRadius = g.radius;
            float innerRadius = g.radius * 0.8f; // Doit correspondre à drawGear
            
            // Rayon de collision effectif (par défaut le moyeu central)
            float collisionRadius = innerRadius + s.size * 0.5f;
            bool checkTeeth = false;

            // Si on est dans la zone des dents (entre inner et outer)
            if (dist < outerRadius + s.size * 0.5f && dist > innerRadius - s.size * 0.5f) {
                // Calcul de l'angle relatif pour savoir si on touche une dent
                ofVec2f relPos = s.pos - g.pos;
                float angle = atan2(relPos.y, relPos.x) - g.angle;
                
                // Normalisation de l'angle autour de la dent la plus proche
                float sector = TWO_PI / g.teethCount;
                while(angle < 0) angle += TWO_PI;
                float angleInSector = fmod(angle, sector);
                if(angleInSector > sector * 0.5f) angleInSector -= sector;
                
                // Ratio épaisseur dent (0.25f pour correspondre au dessin plus fin)
                float toothHalfAngle = (sector * 0.25f) * 0.5f;
                // On ajoute la taille angulaire du carré pour la tolérance
                float squareHalfAngle = (s.size * 0.5f) / dist;

                if (abs(angleInSector) < (toothHalfAngle + squareHalfAngle)) {
                    // On touche une dent !
                    collisionRadius = outerRadius + s.size * 0.5f;
                }
            }
            
            if(dist < collisionRadius) {
                // --- IMPACT ---
                
                // 1. Vecteur de rebond
                ofVec2f normal = (s.pos - g.pos).getNormalized();
                
                // On repousse le carré pour ne pas qu'il colle
                s.pos = g.pos + normal * (collisionRadius + 1.0f);
                
                // Rebond élastique
                ofVec2f reflect = s.vel - 2 * (s.vel.dot(normal)) * normal;
                s.vel = reflect * 0.6f; // Perte d'énergie
                
                // MODIFICATION: Ajout d'une petite force tangentielle pour éviter que le carré ne se bloque
                ofVec2f tangent = normal.getPerpendicular(); // Vecteur perpendiculaire à la normale de collision
                s.vel += tangent * ofRandom(-2.5, 2.5);

                // Ajout d'un peu de chaos à la rotation du carré
                s.rotSpeed += ofRandom(-10, 10);

                // 2. Transfert d'énergie vers l'engrenage (Rotation)
                // Produit vectoriel 2D (Cross product) pour savoir dans quel sens tourner
                // Torque = r x F
                ofVec2f r = s.pos - g.pos;
                // Force approximative basée sur la vitesse d'impact
                ofVec2f impactForce = -reflect; 
                
                float torque = (r.x * impactForce.y - r.y * impactForce.x);
                
                // On ajoute à la vélocité angulaire (avec un facteur d'échelle)
                g.angularVel += torque * 0.0002f;
            }
        }
        
        // Mort du carré (si trop bas)
        if(s.pos.y > simHeight + 100) {
            s.bDead = true;
        }
    }
    
    // Nettoyage carrés morts
    squares.erase(std::remove_if(squares.begin(), squares.end(), 
        [](const FallingSquare& s){ return s.bDead; }), squares.end());

    // 3. Physique des Engrenages
    solveGearConstraints();
    
    for(auto& g : gears) {
        g.angle += g.angularVel;
        g.angularVel *= 0.85f; // Friction (ralentissement naturel)
    }
}

//--------------------------------------------------------------
void GearLayer::solveGearConstraints() {
    // Propagation du mouvement entre engrenages connectés
    // On suppose qu'ils sont en chaîne : 0 <-> 1 <-> 2 <-> 3
    
    float stiffness = 0.9f; // Rigidité de la transmission (0.1 = mou, 1.0 = rigide)

    for(size_t i = 0; i < gears.size() - 1; i++) {
        Gear& gA = gears[i];
        Gear& gB = gears[i+1];
        
        // Ratio de transmission (R1 * W1 = - R2 * W2)
        // Donc W2 idéal = - W1 * (R1 / R2)
        float ratio = gA.radius / gB.radius;
        
        // Vitesse cible pour B basée sur A
        float targetVelB = -gA.angularVel * ratio;
        // Vitesse cible pour A basée sur B
        float targetVelA = -gB.angularVel * (1.0f / ratio);
        
        // On mixe les vitesses pour simuler l'inertie partagée
        gB.angularVel += (targetVelB - gB.angularVel) * stiffness;
        gA.angularVel += (targetVelA - gA.angularVel) * stiffness;
    }
}

//--------------------------------------------------------------
void GearLayer::draw() {
    ofPushStyle();
    
    // Dessin des engrenages
    for(const auto& g : gears) {
        drawGear(g);
    }
    
    // Dessin des carrés
    ofSetRectMode(OF_RECTMODE_CENTER);
    for(const auto& s : squares) {
        ofPushMatrix();
        ofTranslate(s.pos);
        ofRotateDeg(s.angle);
        
        ofSetColor(200, 100, 100); // Carré rouge brique
        ofDrawRectangle(0, 0, s.size, s.size);
        
        ofNoFill();
        ofSetColor(50);
        ofDrawRectangle(0, 0, s.size, s.size);
        ofFill();
        
        ofPopMatrix();
    }
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    ofPopStyle();
}

//--------------------------------------------------------------
void GearLayer::drawGear(const Gear& g) {
    ofPushMatrix();
    ofTranslate(g.pos);
    ofRotateRad(g.angle);
    
    ofSetColor(g.color);
    
    // Style AS3 : Cercle central + Barres rectangulaires croisées
    
    // 1. Cercle central (Racine des dents)
    // On le fait un peu plus petit que le rayon total pour laisser dépasser les dents
    float innerRadius = g.radius * 0.8f;
    ofDrawCircle(0, 0, innerRadius);
    
    // 2. Dents (Barres)
    int numBars = g.teethCount / 2;
    if(numBars < 1) numBars = 1;
    
    float barLength = g.radius * 2.0f; // Diamètre total
    // Largeur de la barre (épaisseur de la dent)
    // MODIFICATION: Dents plus fines pour plus d'écart
    float barThickness = (innerRadius * TWO_PI) / g.teethCount * 0.25f; // était 0.4f
    
    // OPTIMISATION: Utilisation d'un Mesh pour les dents au lieu de multiples drawRectangle + Rotate
    // Cela réduit drastiquement les appels OpenGL et les changements de matrice.
    // Utilisation de static pour éviter la réallocation mémoire à chaque frame
    static ofMesh teethMesh;
    teethMesh.clear();
    teethMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    float halfW = barLength * 0.5f;
    float halfH = barThickness * 0.5f;

    for(int i=0; i<numBars; i++) {
        float angleRad = ofMap(i, 0, numBars, 0, PI); // 0 à 180 degrés en radians
        float c = cos(angleRad);
        float s = sin(angleRad);
        
        // Calcul manuel des 4 coins du rectangle après rotation
        // Coin 1 (-w, -h)
        teethMesh.addVertex(ofVec3f(-halfW * c - -halfH * s, -halfW * s + -halfH * c, 0));
        // Coin 2 (w, -h)
        teethMesh.addVertex(ofVec3f(halfW * c - -halfH * s, halfW * s + -halfH * c, 0));
        // Coin 3 (-w, h)
        teethMesh.addVertex(ofVec3f(-halfW * c - halfH * s, -halfW * s + halfH * c, 0));
        
        // Triangle 2
        teethMesh.addVertex(ofVec3f(halfW * c - -halfH * s, halfW * s + -halfH * c, 0)); // Coin 2
        teethMesh.addVertex(ofVec3f(halfW * c - halfH * s, halfW * s + halfH * c, 0));   // Coin 4 (w, h)
        teethMesh.addVertex(ofVec3f(-halfW * c - halfH * s, -halfW * s + halfH * c, 0)); // Coin 3
    }
    
    teethMesh.draw();
    
    // Trou central
    ofSetColor(50, 40, 30);
    ofDrawCircle(0, 0, g.radius * 0.2);
    
    // Rayon visuel pour voir la rotation
    ofSetColor(255, 50);
    ofDrawCircle(g.radius * 0.6f, 0, g.radius * 0.05f);
    
    ofPopMatrix();
}
