#include "LiquidSphereRing.h"

//--------------------------------------------------------------
void LiquidSphereRing::setup(float r, ofVec3f centerPos) {
    radius = r;
    center = centerPos;
    
    ofDisableArbTex(); // Sécurité pour s'assurer que les UV sont bien de 0.0 à 1.0
    
    // 1. Initialisation du fluide (Largeur=2048, Hauteur=1024 pour ne pas surcharger le CPU avec du 8K)
    fluid.setup(2048, 1024, 1.0f, 512, 256);
    
    // 2. On charge la texture en fond de la simulation fluide APRES fluid.setup() qui écrase l'image par défaut
    if (fluid.bgImage.load("export_360_room.png")) {
        fluid.bUseImage = true;
        fluid.resetPattern(); // Met à jour les couleurs de la grille fluide avec la nouvelle image
    } else {
        ofLogError("LiquidSphereRing") << "Impossible de charger Z_extra/Alexs_Apt_8k.jpg";
        fluid.bUseImage = false; // Fallback implicite vers le checkerboard
    }
    
    // Paramètres de texture pour la sphère (bouclage horizontal X, arrêt vertical Y)
    fluid.fluidImage.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    fluid.fluidImage.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);

    resX = 180;
    resY = 90;

    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // 3. Génération des sommets de la SPHERE
    for (int iy = 0; iy <= resY; iy++) {
        float v = (float)iy / resY;
        float theta = v * PI;
        
        for (int ix = 0; ix <= resX; ix++) {
            float u = (float)ix / resX;
            float phi = u * TWO_PI;
            
            float xPos = radius * sin(theta) * cos(phi);
            float yPos = radius * cos(theta);
            float zPos = radius * sin(theta) * sin(phi);
            
            mesh.addVertex(ofVec3f(xPos, yPos, zPos));
            mesh.addTexCoord(ofVec2f(u, v));
            mesh.addColor(ofColor(255, 255, 255, 255));
        }
    }

    // 4. Génération des triangles
    for (int iy = 0; iy < resY; iy++) {
        for (int ix = 0; ix < resX; ix++) {
            int i0 = iy * (resX + 1) + ix;
            int i1 = i0 + 1;
            int i2 = (iy + 1) * (resX + 1) + ix;
            int i3 = i2 + 1;
            
            mesh.addIndex(i0); mesh.addIndex(i2); mesh.addIndex(i1);
            mesh.addIndex(i1); mesh.addIndex(i2); mesh.addIndex(i3);
        }
    }
}

//--------------------------------------------------------------
void LiquidSphereRing::draw() {
    ofPushStyle();
    ofEnableAlphaBlending();
    glDepthMask(GL_FALSE); 
    
    ofPushMatrix();
    ofTranslate(center);
    ofRotateYDeg(-90.0f); // Rotation de 90 degrés (comme le CloudRippleRing)
    
    fluid.fluidImage.bind();
    mesh.draw();
    fluid.fluidImage.unbind();
    
    ofPopMatrix();
    
    glDepthMask(GL_TRUE);
    ofPopStyle();
}