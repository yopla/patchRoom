#include "FluidRing.h"

//--------------------------------------------------------------
void FluidRing::setup(float r, float topH, float botExt) {
    radius = r;
    height = topH;
    bottomExt = botExt;

    // 1. Configuration de la simulation fluide
    // On utilise une résolution suffisante pour faire le tour
    // Chargement de la texture spécifique demandée
    fluid.bgImage.load("iconeRZEF.png");
    fluid.bUseImage = true;

    // Largeur texture = 2048 (pour boucler proprement), Hauteur = 1024
    fluid.setup(2048, 1024, 512, 256);
    
    // On s'assure que la texture boucle horizontalement
    fluid.fluidImage.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    fluid.fluidImage.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);

    // On force le mode image pour avoir une base colorée si besoin, 
    // ou on laisse le pattern par défaut.
    // fluid.toggleBackground(); 

    // 2. Création du Maillage (Cylindre / Anneau)
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES); // Passage en TRIANGLES pour gérer les rangées du dégradé

    int resolution = 100; // Nombre de segments pour le cercle
    float totalHeight = height + bottomExt; // Hauteur totale de la texture
    
    // --- PARAMÈTRES DE COURBURE ---
    float curveHeight = 480.0f;   // Taille de la zone courbée (depuis le haut/bas)
    float curveScale = 0.55f;     // Intensité (1.0 = droit, < 1.0 = vers l'intérieur)

    if(curveHeight * 2 > totalHeight) curveHeight = totalHeight * 0.4f; // Sécurité

    // On définit 4 niveaux verticaux pour le dégradé :
    // 0: Haut absolu (Transparent)
    // 1: Haut visible (Opaque)
    // 2: Bas visible (Opaque)
    // 3: Bas absolu (Transparent)
    struct Level { float y; float v; float alpha; float rScale; };
    vector<Level> levels;
    // On ajoute un facteur d'échelle (rScale) pour courber le haut et le bas vers l'intérieur (ex: 0.85)
    levels.push_back({height, 0.0f, 0.0f, curveScale});
    levels.push_back({height - curveHeight, curveHeight / totalHeight, 255.0f, 1.0f});
    levels.push_back({-bottomExt + curveHeight, (totalHeight - curveHeight) / totalHeight, 255.0f, 1.0f});
    levels.push_back({-bottomExt, 1.0f, 0.0f, curveScale});

    // Génération des sommets
    for(int j = 0; j < levels.size(); j++) {
        for(int i = 0; i <= resolution; i++) {
            float pct = (float)i / (float)resolution;
            float angle = pct * TWO_PI;
            
            float currentRadius = radius * levels[j].rScale;
            float x = cos(angle) * currentRadius;
            float z = sin(angle) * currentRadius;
            
            mesh.addVertex(ofVec3f(x, levels[j].y, z));
            mesh.addTexCoord(ofVec2f(pct, levels[j].v));
            mesh.addColor(ofColor(255, 255, 255, levels[j].alpha));
        }
    }

    // Génération des indices (Triangles)
    int stride = resolution + 1;
    for(int j = 0; j < levels.size() - 1; j++) {
        for(int i = 0; i < resolution; i++) {
            int idx1 = j * stride + i;       // Haut Gauche
            int idx2 = idx1 + 1;             // Haut Droite
            int idx3 = idx1 + stride;        // Bas Gauche
            int idx4 = idx3 + 1;             // Bas Droite
            
            // Triangle 1
            mesh.addIndex(idx1); mesh.addIndex(idx2); mesh.addIndex(idx3);
            // Triangle 2
            mesh.addIndex(idx2); mesh.addIndex(idx4); mesh.addIndex(idx3);
        }
    }
}

//--------------------------------------------------------------
void FluidRing::update() {
    // Animation automatique supprimée pour laisser le contrôle exclusif à la souris
    // La mise à jour du fluide (fluid.update) est désormais gérée par RoomApp
    // avec les coordonnées précises du Raycast.
}

//--------------------------------------------------------------
void FluidRing::draw() {
    ofEnableAlphaBlending(); // Important pour que le dégradé fonctionne
    ofSetColor(255);
    
    // On bind la texture générée par le FluidFloorLayer
    fluid.fluidImage.bind();
    
    // On dessine le maillage
    mesh.draw();
    
    fluid.fluidImage.unbind();
}
