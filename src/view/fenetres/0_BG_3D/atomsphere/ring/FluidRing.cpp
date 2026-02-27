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
    fluid.setup(2048, 1024, 256, 128);
    
    // On s'assure que la texture boucle horizontalement
    fluid.fluidImage.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);

    // On force le mode image pour avoir une base colorée si besoin, 
    // ou on laisse le pattern par défaut.
    // fluid.toggleBackground(); 

    // 2. Création du Maillage (Cylindre / Anneau)
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    int resolution = 100; // Nombre de segments pour le cercle
    float Total_Height = height + bottomExt; // Hauteur totale de la texture

    for(int i = 0; i <= resolution; i++) {
        float pct = (float)i / (float)resolution;
        float angle = pct * TWO_PI;
        
        // Coordonnées X, Z sur le cercle
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // Mapping UV
        // u : fait le tour de 0 à 1 (mappé sur la largeur de la texture fluide)
        // v : de haut en bas
        float u = pct; // 0 à 1 car ofDisableArbTex() est actif (GL_TEXTURE_2D)
        
        // Vertex du HAUT (Niveau des murs)
        // v = 0 (Haut de la texture)
        mesh.addVertex(ofVec3f(x, height, z));
        mesh.addTexCoord(ofVec2f(u, 0));

        // Vertex du BAS (Sous le sol)
        // v = 1 (Bas de la texture)
        mesh.addVertex(ofVec3f(x, -bottomExt, z));
        mesh.addTexCoord(ofVec2f(u, 1.0f));
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
    ofSetColor(255);
    
    // On bind la texture générée par le FluidFloorLayer
    fluid.fluidImage.bind();
    
    // On dessine le maillage
    mesh.draw();
    
    fluid.fluidImage.unbind();
}
