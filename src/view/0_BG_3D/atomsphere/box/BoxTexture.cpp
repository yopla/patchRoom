#include "BoxTexture.h"

void BoxTexture::setup(float w, float h, float d) {
    box.set(w, h, d);
    box.setResolution(2);
    
    int size = 512;
    int numChecks = 16; // Damier 16x16
    ofPixels pixels;
    pixels.allocate(size, size, OF_IMAGE_GRAYSCALE);
    int checkSize = size / numChecks;
    
    for(int y = 0; y < size; y++) {
        for(int x = 0; x < size; x++) {
            if(((x / checkSize) + (y / checkSize)) % 2 == 0) pixels.setColor(x, y, ofColor(255));
            else pixels.setColor(x, y, ofColor(50)); // Gris foncé
        }
    }
    
    bool bWasArb = ofGetUsingArbTex();
    if(bWasArb) ofDisableArbTex();
    
    textureDamier.allocate(pixels);
    textureDamier.setTextureWrap(GL_REPEAT, GL_REPEAT);
    textureDamier.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    if(bWasArb) ofEnableArbTex();
    
    box.mapTexCoords(0, 0, 4, 4); // On répète la texture 4 fois sur chaque face
}

void BoxTexture::draw() {
    ofPushStyle();
    ofEnableDepthTest();
    ofSetColor(255);
    
    textureDamier.bind();
    ofPushMatrix();
    // Centre de la box ajusté pour correspondre à la Room 
    // (le sol est à y=0, le plafond à heightFrontBack)
    // On enlève 100 vu que la box a 200 de plus sur ses axes.
    ofTranslate(0, box.getHeight() / 2.0f - 100.0f, 0); 
    ofScale(-1, 1, 1); // Pour voir la texture à l'endroit depuis l'intérieur
    box.draw();
    ofPopMatrix();
    textureDamier.unbind();
    
    ofPopStyle();
}