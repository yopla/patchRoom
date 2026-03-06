#include "UndulatingFloor.h"

void UndulatingFloor::setup(float w, float zStart, float zEnd) {
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    float stepX = w / (float)(cols - 1);
    float stepZ = (zEnd - zStart) / (float)(rows - 1);
    float xStart = -w / 2.0f;

    // Création des vertices
    for(int y = 0; y < rows; y++) {
        for(int x = 0; x < cols; x++) {
            mesh.addVertex(ofVec3f(xStart + x * stepX, 0, zStart + y * stepZ));
            mesh.addColor(ofColor(255)); // Couleur initiale
        }
    }

    // Création des indices (Triangles)
    for(int y = 0; y < rows - 1; y++) {
        for(int x = 0; x < cols - 1; x++) {
            int i1 = x + y * cols;
            int i2 = (x+1) + y * cols;
            int i3 = x + (y+1) * cols;
            int i4 = (x+1) + (y+1) * cols;
            
            mesh.addIndex(i1); mesh.addIndex(i2); mesh.addIndex(i3);
            mesh.addIndex(i2); mesh.addIndex(i4); mesh.addIndex(i3);
        }
    }
}

void UndulatingFloor::update(float time) {
    int numV = mesh.getNumVertices();
    for(int i=0; i<numV; i++) {
        ofVec3f v = mesh.getVertex(i);
        
        // Calcul de la hauteur (Y) avec Sinus + Noise
        float y = sin(v.x * 0.003f + time * 1.5f) * 60.0f + cos(v.z * 0.004f + time * 1.2f) * 60.0f;
        y += ofSignedNoise(v.x * 0.006f, v.z * 0.006f, time * 0.6f) * 40.0f;
        
        v.y = y;
        mesh.setVertex(i, v);
        
        // Couleur dynamique basée sur la hauteur (Cyan/Bleu)
        ofColor c;
        c.setHsb(ofMap(y, -120, 120, 130, 170, true), 200, 255, 180);
        mesh.setColor(i, c);
    }
}

void UndulatingFloor::draw() {
    ofPushStyle();
    glDepthMask(GL_FALSE); // Transparence sans écriture dans le depth buffer
    
    mesh.draw(); // Dessin plein (couleurs vertex)
    
    ofSetColor(255, 255, 255, 80);
    mesh.drawWireframe(); // Fil de fer par dessus
    
    glDepthMask(GL_TRUE);
    ofPopStyle();
}