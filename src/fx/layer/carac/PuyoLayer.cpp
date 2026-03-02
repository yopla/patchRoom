#include "PuyoLayer.h"

//--------------------------------------------------------------
// PUYO OBJECT
//--------------------------------------------------------------
void Puyo::setup(float x, float y, float r, bool gravity) {
    radius = r;
    bGravity = gravity;
    center.set(x, y);
    noiseOffset = ofRandom(1000);
    foldedTimer = 0.0f;
    
    // Couleur aléatoire avec opacité gérée au draw
    color = ofColor::fromHsb(ofRandom(255), 180, 255);

    int numNodes = 16; // Moins de noeuds pour un rendu plus "spline" et arrondi (moins de bruit HF)
    float angleStep = TWO_PI / numNodes;
    
    // 1. Création des noeuds (Anneau + Centre)
    nodes.reserve(numNodes + 1); // Réservation mémoire pour éviter les réallocations
    for(int i=0; i<numNodes; i++) {
        float angle = i * angleStep;
        PuyoNode n;
        n.pos.set(x + cos(angle) * r, y + sin(angle) * r);
        n.oldPos = n.pos;
        nodes.push_back(n);
    }
    
    // Noeud central pour la stabilité du volume
    PuyoNode c;
    c.pos.set(x, y);
    c.oldPos = c.pos;
    nodes.push_back(c);
    int centerIdx = numNodes;

    // 2. Création des ressorts (Links)
    // Contour
    links.reserve(numNodes * 2); // Réservation pour contour + rayons
    for(int i=0; i<numNodes; i++) {
        PuyoLink l;
        l.n1 = i;
        l.n2 = (i + 1) % numNodes;
        l.length = nodes[l.n1].pos.distance(nodes[l.n2].pos);
        l.stiffness = 0.95f; // Contour très rigide
        links.push_back(l);
    }
    
    // Rayons (Vers le centre)
    for(int i=0; i<numNodes; i++) {
        PuyoLink l;
        l.n1 = i;
        l.n2 = centerIdx;
        l.length = radius;
        l.stiffness = 0.6f; // Ressorts internes renforcés pour la structure
        links.push_back(l);
    }
}

void Puyo::update(float w, float h, shared_ptr<ColliderLayer> collider) {
    // 1. Verlet Integration & Forces
    center.set(0,0);
    int count = 0;
    
    // Calcul du bruit global pour tout le Puyo (évite la déformation interne / tortillements)
    float time = ofGetFrameNum() * 0.005f;
    float nx = ofSignedNoise(time, noiseOffset);
    float ny = ofSignedNoise(noiseOffset, time);
    
    for(auto& n : nodes) {
        ofVec2f vel = (n.pos - n.oldPos) * 0.98f; // Friction
        n.oldPos = n.pos;
        
        // Gravité ou Flottement
        if(bGravity) {
            vel.y += 0.15f; // Gravité légère
        } else {
            // Balade (Perlin Noise)
            vel += ofVec2f(nx, ny) * 0.1f; // Mouvement uniforme
        }
        
        n.pos += vel;
        
        // Calcul du centre en temps réel
        if(count < nodes.size()-1) center += n.pos; // On exclut le noeud central du calcul moyen pour éviter biais
        count++;
    }
    center /= (nodes.size()-1);
    
    // FIX: On force le noeud central à suivre le centre géométrique
    // Cela évite que le poids du noeud central ne déforme la bulle (effet goutte d'eau)
    nodes.back().pos += (center - nodes.back().pos) * 0.5f;
    
    // 1b. PRESSION INTERNE (Comme des bulles gonflées)
    // On pousse les noeuds vers l'extérieur pour maintenir le rayon
    for(int i=0; i<nodes.size()-1; i++) {
        auto& n = nodes[i];
        ofVec2f dir = n.pos - center;
        float d = dir.length();
        if(d > 0.01f) {
            // FORME : On ramène les points vers le cercle idéal (Se refont)
            // Cela gère la compression ET l'extension pour éviter les formes "longues"
            ofVec2f target = center + dir.getNormalized() * radius;
            n.pos += (target - n.pos) * 0.35f; // Regonflement plus tonique
        } else {
            // Sécurité si le point est pile au centre (évite le blocage)
            n.pos += ofVec2f(ofRandom(-1,1), ofRandom(-1,1)) * 0.1f;
        }
    }
    
    // 2. Résolution des contraintes (Forme)
    for(int k=0; k<8; k++) solveConstraints(); // Optimisation : 8 itérations suffisent généralement pour cette rigidité
    
    // 3. Collisions avec le Collider (Murs)
    if(collider) {
        for(auto& n : nodes) {
            // Boucle de sécurité pour forcer la sortie du mur (plus d'influence)
            int safe = 0;
            while(collider->isWall(n.pos.x, n.pos.y) && safe < 8) {
                ofVec2f dir = (center - n.pos).getNormalized();
                if(dir.lengthSquared() == 0) dir.set(0, -1);
                
                n.pos += dir * 4.0f; // Poussée plus forte
                
                // Friction forte contre les murs pour éviter l'enfoncement
                ofVec2f vel = n.pos - n.oldPos;
                n.oldPos = n.pos - vel * 0.2f; 
                safe++;
            }
        }
    }
    
    // 4. Wrapping (Boucle Gauche -> Droite)
    // On utilise le centre pour décider
    if(center.x > w) {
        for(auto& n : nodes) { n.pos.x -= w; n.oldPos.x -= w; }
        center.x -= w;
    } else if(center.x < 0) {
        for(auto& n : nodes) { n.pos.x += w; n.oldPos.x += w; }
        center.x += w;
    }
    
    // Sol et Plafond de sécurité
    for(auto& n : nodes) {
        if(n.pos.y > h) { n.pos.y = h; n.oldPos.y = n.pos.y; }
        if(n.pos.y < 0) { n.pos.y = 0; n.oldPos.y = n.pos.y; }
    }
    
    // 5. Regonflement automatique après 3 secondes de pliage
    if(isFolded()) {
        foldedTimer += ofGetLastFrameTime();
        if(foldedTimer > 3.0f) {
            // Réinitialisation de la forme (Cercle parfait comme à l'origine)
            int ringSize = nodes.size() - 1;
            float angleStep = TWO_PI / ringSize;
            
            for(int i=0; i<ringSize; i++) {
                float angle = i * angleStep;
                nodes[i].pos.set(center.x + cos(angle) * radius, center.y + sin(angle) * radius);
                nodes[i].oldPos = nodes[i].pos; // Stop net du mouvement
            }
            nodes.back().pos = center;
            nodes.back().oldPos = center;
            
            foldedTimer = 0.0f;
        }
    } else {
        foldedTimer = 0.0f;
    }
}

void Puyo::solveConstraints() {
    for(auto& l : links) {
        PuyoNode& p1 = nodes[l.n1];
        PuyoNode& p2 = nodes[l.n2];
        ofVec2f delta = p1.pos - p2.pos;
        float dist = delta.length();
        if(dist == 0) continue;
        
        float diff = (dist - l.length) / dist;
        ofVec2f offset = delta * 0.5f * diff * l.stiffness;
        
        p1.pos -= offset;
        p2.pos += offset;
    }
}

bool Puyo::isFolded() {
    // Détection de forme "Ligne" via PCA (Analyse des valeurs propres de la covariance)
    // Cela permet de savoir si le Puyo est écrasé (quasi ligne) de manière précise.
    int N = nodes.size() - 1; 
    if (N < 3) return false;

    // 1. Centre de gravité
    ofVec2f mean(0,0);
    for(int i=0; i<N; i++) mean += nodes[i].pos;
    mean /= (float)N;

    // 2. Matrice de Covariance (Distribution des points)
    float cxx = 0, cyy = 0, cxy = 0;
    for(int i=0; i<N; i++) {
        ofVec2f d = nodes[i].pos - mean;
        cxx += d.x * d.x;
        cyy += d.y * d.y;
        cxy += d.x * d.y;
    }
    
    // 3. Valeurs propres (Eigenvalues) pour trouver petit axe et grand axe
    float trace = cxx + cyy;
    float det = cxx*cyy - cxy*cxy;
    float disc = sqrt(max(0.0f, trace*trace - 4*det));
    
    float lambdaMax = (trace + disc) * 0.5f; // Grand axe (longueur)
    float lambdaMin = (trace - disc) * 0.5f; // Petit axe (largeur)
    
    if(lambdaMax < 0.001f) return false; // Trop petit pour être jugé
    
    // Ratio d'aspect : Si < 0.1, la largeur est négligeable devant la longueur -> Ligne
    return (lambdaMin / lambdaMax < 0.1f);
}

void Puyo::draw(float scale) {
    ofPushMatrix();
    ofScale(scale, scale);
    
    // Fond couleur plus opaque pour l'aspect "épais"
    ofSetColor(color.r, color.g, color.b, 220);
    
    // OPTIMISATION: Construction de la forme une seule fois via ofPolyline
    // Utilisation de static pour réutiliser la mémoire du vecteur interne
    static ofPolyline shape;
    shape.clear();
    int ringSize = nodes.size() - 1;
    
    // On ajoute les points pour fermer la courbe proprement (wrapping)
    for(int i=0; i<ringSize + 3; i++) {
        int idx = (i + ringSize - 1) % ringSize;
        shape.curveTo(glm::vec3(nodes[idx].pos.x, nodes[idx].pos.y, 0));
    }
    
    // 1. Remplissage
    ofFill();
    ofBeginShape();
    const auto& vertices = shape.getVertices();
    for(const auto& v : vertices) ofVertex(v);
    ofEndShape();
    
    // Contour Filaire Blanc
    ofSetColor(255);
    ofNoFill();
    ofSetLineWidth(12); // Trait très épais (x4)
    shape.draw(); // ofPolyline gère le dessin du contour efficacement
    
    ofPopMatrix();
}

//--------------------------------------------------------------
// PUYO LAYER MANAGER
//--------------------------------------------------------------
void PuyoLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    scale = s;
    collider = col;
    
    // Ajout de quelques Puyos au départ
    for(int i=0; i<15; i++) { // Moins de puyos car ils sont plus gros
        addPuyo(ofRandom(simWidth), ofRandom(simHeight * 0.5));
    }
}

void PuyoLayer::addPuyo(float x, float y) {
    auto p = make_shared<Puyo>();
    static int count = 0;
    // La moitié est sujet à la gravité
    bool grav = (count % 2 == 0); 
    count++;
    
    // Taille variable
    float r = ofRandom(5, 15); // Vraiment épais pour bien voir la déformation
    p->setup(x, y, r, grav);
    puyos.push_back(p);
}

void PuyoLayer::getStats(int& folded, int& complete) {
    folded = 0;
    complete = 0;
    for(auto& p : puyos) {
        if(p->isFolded()) folded++;
        else complete++;
    }
}

void PuyoLayer::update(float mx, float my) {
    // Conversion souris Monde -> Sim
    float simMx = mx / scale;
    float simMy = my / scale; // Attention offset Y géré dans Scene2DLayerManager

    for(auto& p : puyos) {
        p->update(simWidth, simHeight, collider);
        
        // Interaction Souris (Répulsion simple)
        if(ofGetMousePressed()) {
            float d = p->center.distance(ofVec2f(simMx, simMy));
            if(d < 100) {
                ofVec2f dir = (p->center - ofVec2f(simMx, simMy)).getNormalized();
                for(auto& n : p->nodes) n.pos += dir * 2.0f;
            }
        }
    }
    
    // Collision Puyo <-> Puyo (Adaptation de forme)
    for(int i=0; i<puyos.size(); i++) {
        for(int j=i+1; j<puyos.size(); j++) {
            auto& p1 = puyos[i];
            auto& p2 = puyos[j];
            
            float dist = p1->center.distance(p2->center);
            float minDist = p1->radius + p2->radius;
            
            // Si les bounding circles se touchent
            if(dist < minDist) {
                ofVec2f dir = (p1->center - p2->center).getNormalized();
                float overlap = minDist - dist;
                
                // 1. Séparation douce des centres (pour éviter l'interpénétration totale)
                ofVec2f force = dir * overlap * 0.4f; // Séparation plus franche pour éviter l'écrasement
                
                for(auto& n : p1->nodes) n.pos += force;
                for(auto& n : p2->nodes) n.pos -= force;
                
            }
        }
    }
}

void PuyoLayer::draw() {
    for(auto& p : puyos) {
        p->draw(scale);
        
        // Gestion du wrapping visuel (Fantômes)
        if(p->center.x < p->radius) {
            ofPushMatrix();
            ofTranslate(simWidth * scale, 0);
            p->draw(scale);
            ofPopMatrix();
        } else if(p->center.x > simWidth - p->radius) {
            ofPushMatrix();
            ofTranslate(-simWidth * scale, 0);
            p->draw(scale);
            ofPopMatrix();
        }
    }
}
