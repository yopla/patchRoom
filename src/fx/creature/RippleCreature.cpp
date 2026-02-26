#include "RippleCreature.h"

RippleCreature::RippleCreature(float _x, float _y, float _w, float _h, ofImage& _sharedSource) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    
    // Paramètres (identiques à ton script original)
    bufferScale = 0.525;
    rippleSize = 3;
    
    // 1. Préparation de l'image locale
    // On clone l'image partagée et on la redimensionne à la taille de cette instance
    sourceImage = _sharedSource; // Copie
    sourceImage.crop(0, 0, _sharedSource.getWidth(), _sharedSource.getHeight()); // Clean copy
    sourceImage.resize((int)w, (int)h);
    sourceImage.setImageType(OF_IMAGE_COLOR); // Force RGB
    
    // 2. Image de sortie
    outputImage.allocate((int)w, (int)h, OF_IMAGE_COLOR);
    
    // 3. Init Buffers
    cols = (int)(w * bufferScale);
    rows = (int)(h * bufferScale);
    
    buffer1.assign(cols * rows, 0);
    buffer2.assign(cols * rows, 0);
    
    prevLocalX = 0;
    prevLocalY = 0;
}

RippleCreature::~RippleCreature() {
    // Nettoyage automatique des vectors et images
}

void RippleCreature::update() {
    // Calcul de la physique de l'eau
    processRipples();
    
    // Rendu des pixels déformés
    renderWater();
    
    // Échange des buffers pour la frame suivante
    std::swap(buffer1, buffer2);
}

void RippleCreature::draw() {
    //ofSetColor(255);
    // On dessine l'image résultante à la position de l'instance
    //outputImage.draw(x, y, w, h);

    ofPushMatrix();
    
    // 1. On se place au CENTRE de la créature
    ofTranslate(x + w/2, y + h/2);
    
    // 2. On tourne
    ofRotateDeg(rotation);
    outputImage.draw(-w/2, -h/2, w, h);
    ofPopMatrix();
}

bool RippleCreature::isInside(float mx, float my) {
    return (mx >= x && mx < x + w && my >= y && my < y + h);
}

void RippleCreature::checkInput(float mx, float my) {
    if (isInside(mx, my)) {
        // Conversion en coordonnées locales
        float localX = mx - x;
        float localY = my - y;
        
        // Si la souris bouge à l'intérieur de la zone
        if (localX != prevLocalX || localY != prevLocalY) {
            createRipple((int)localX, (int)localY);
        }
        
        prevLocalX = localX;
        prevLocalY = localY;
    }
}

// --- LOGIQUE INTERNE (Adaptée pour le Local) ---

void RippleCreature::createRipple(int localX, int localY) {
    int bx = (int)(localX * bufferScale);
    int by = (int)(localY * bufferScale);

    for (int j = by - rippleSize; j < by + rippleSize; j++) {
        for (int k = bx - rippleSize; k < bx + rippleSize; k++) {
            if (j >= 1 && j < rows - 1 && k >= 1 && k < cols - 1) {
                // On écrit dans le buffer
                buffer1[k + j * cols] = 255; 
            }
        }
    }
}

void RippleCreature::processRipples() {
    // Optimisation : accès direct via pointeurs ou iterateurs est possible, 
    // mais l'accès vector [] est sécurisé et assez rapide ici.
    
    // Attention : boucles basées sur 'cols' et 'rows' locaux
    for (int y = 1; y < rows - 1; y++) {
        for (int x = 1; x < cols - 1; x++) {
            int index = x + y * cols;
            
            // Propagation
            int val = (buffer1[index - 1] + 
                       buffer1[index + 1] + 
                       buffer1[index - cols] + 
                       buffer1[index + cols]) >> 1;
            
            val -= buffer2[index];
            val -= val >> 5; // Damping
            
            buffer2[index] = val;
        }
    }
}

void RippleCreature::renderWater() {
    // Accès brut aux pixels pour la performance
    const unsigned char* srcPixels = sourceImage.getPixels().getData();
    unsigned char* dstPixels = outputImage.getPixels().getData();
    
    int iw = (int)w; // Largeur image locale
    int ih = (int)h; // Hauteur image locale
    int channels = 3; 

    for (int y = 0; y < ih; y++) {
        for (int x = 0; x < iw; x++) {
            
            int xBuffer = (int)(x * bufferScale);
            int yBuffer = (int)(y * bufferScale);
            
            if (xBuffer > 0 && xBuffer < cols - 1 && yBuffer > 0 && yBuffer < rows - 1) {
                
                int index = xBuffer + yBuffer * cols;
                
                // Calcul du déplacement (Offsets)
                int xOffset = buffer1[index - 1] - buffer1[index + 1];
                int yOffset = buffer1[index - cols] - buffer1[index + cols];
                
                int xCoord = x + xOffset;
                int yCoord = y + yOffset;
                
                // Clamping local
                xCoord = ofClamp(xCoord, 0, iw - 1);
                yCoord = ofClamp(yCoord, 0, ih - 1);
                
                int pixelIndex = (x + y * iw) * channels;
                int sourceIndex = (xCoord + yCoord * iw) * channels;
                
                dstPixels[pixelIndex]     = srcPixels[sourceIndex];
                dstPixels[pixelIndex + 1] = srcPixels[sourceIndex + 1];
                dstPixels[pixelIndex + 2] = srcPixels[sourceIndex + 2];
                
            } else {
                int pixelIndex = (x + y * iw) * channels;
                dstPixels[pixelIndex]     = srcPixels[pixelIndex];
                dstPixels[pixelIndex + 1] = srcPixels[pixelIndex + 1];
                dstPixels[pixelIndex + 2] = srcPixels[pixelIndex + 2];
            }
        }
    }
    outputImage.update(); // Envoi au GPU
}