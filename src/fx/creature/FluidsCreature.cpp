#include "FluidsCreature.h"

//--------------------------------------------------------------
FluidsCreature::FluidsCreature(float _x, float _y, float _w, float _h, ofImage& _sharedSource) {
    x = _x; y = _y; w = _w; h = _h;
    
    gridWidth = 256; 
    gridHeight = 256;
    scale = w / (float)gridWidth; 

    int size = (gridWidth + 2) * (gridHeight + 2);
    cells.resize(size);
    prevCells.resize(size);
    
    // Init du vecteur mémoire
    initialColors.resize(size); 

    fluidImage.allocate(gridWidth, gridHeight, OF_IMAGE_COLOR_ALPHA);
    fluidImage.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

    colorFromImage(_sharedSource);
    
    prevLocalX = 0; prevLocalY = 0;
}

//--------------------------------------------------------------
FluidsCreature::~FluidsCreature() {
    cells.clear(); prevCells.clear(); initialColors.clear();
}

//--------------------------------------------------------------
void FluidsCreature::colorFromImage(ofImage& img) {
    ofImage temp;
    temp = img;
    temp.resize(gridWidth, gridHeight);
    ofPixels& pix = temp.getPixels();
    int numChannels = pix.getNumChannels();
    
    for(int j = 1; j <= gridHeight; j++) {
        for(int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            ofColor c = pix.getColor(i-1, j-1);
            
            // 1. On stocke dans la simulation active
            cells[idx].r = c.r / 255.0f;
            cells[idx].g = c.g / 255.0f;
            cells[idx].b = c.b / 255.0f;
            cells[idx].a = (numChannels == 4) ? c.a / 255.0f : 1.0f;

            // 2. On stocke dans la "mémoire" (initialColors)
            initialColors[idx] = c; 
        }
    }
}

//--------------------------------------------------------------
void FluidsCreature::update(float mx, float my) {
    checkInput(mx, my);
    
    advect();   // Déplace les couleurs
    project();  // Gère la pression
    
    // On ramène un peu les couleurs vers l'original pour éviter le gris
    regenerateColors();

    ofPixels& pix = fluidImage.getPixels();
    for(int j = 1; j <= gridHeight; j++) {
        for(int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            
            unsigned char r = (unsigned char)ofClamp(cells[idx].r * 255, 0, 255);
            unsigned char g = (unsigned char)ofClamp(cells[idx].g * 255, 0, 255);
            unsigned char b = (unsigned char)ofClamp(cells[idx].b * 255, 0, 255);
            unsigned char a = (unsigned char)ofClamp(cells[idx].a * 255, 0, 255);
            
            pix.setColor(i-1, j-1, ofColor(r, g, b, a));
        }
    }
    fluidImage.update();
}

//--------------------------------------------------------------
void FluidsCreature::regenerateColors() {
    // Vitesse de retour à la normale
    float returnSpeed = 0.02f; 

    for(int j = 1; j <= gridHeight; j++) {
        for(int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            
            // Couleur cible (Originale)
            float tr = initialColors[idx].r / 255.0f;
            float tg = initialColors[idx].g / 255.0f;
            float tb = initialColors[idx].b / 255.0f;
            float ta = initialColors[idx].a / 255.0f;

            // Interpolation
            cells[idx].r += (tr - cells[idx].r) * returnSpeed;
            cells[idx].g += (tg - cells[idx].g) * returnSpeed;
            cells[idx].b += (tb - cells[idx].b) * returnSpeed;
            cells[idx].a += (ta - cells[idx].a) * returnSpeed;
        }
    }
}

//--------------------------------------------------------------
void FluidsCreature::draw() {
    ofEnableAlphaBlending(); 
    ofPushMatrix();
    ofTranslate(x, y);
    fluidImage.draw(0, 0, w, h);
    ofPopMatrix();
    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
bool FluidsCreature::isInside(float mx, float my) {
    return (mx >= x && mx < x + w && my >= y && my < y + h);
}

//--------------------------------------------------------------
void FluidsCreature::checkInput(float mx, float my) {
    float localX = mx - x;
    float localY = my - y;
    
    float gridX = ofMap(localX, 0, w, 1, gridWidth);
    float gridY = ofMap(localY, 0, h, 1, gridHeight);
    
    bool isCurrentlyInside = isInside(mx, my);
    
    if (isCurrentlyInside) {
        float dx = gridX - prevLocalX;
        float dy = gridY - prevLocalY;
        
        if (abs(dx) > 0.1 || abs(dy) > 0.1) {
            int radius = 5; 
            int cx = (int)gridX;
            int cy = (int)gridY;

            for(int j = cy - radius; j <= cy + radius; j++) {
                for(int i = cx - radius; i <= cx + radius; i++) {
                    if(i > 0 && i <= gridWidth && j > 0 && j <= gridHeight) {
                        float dist = ofDist(gridX, gridY, i, j);
                        if (dist < radius) {
                            int idx = getIndex(i, j);
                            float power = (1.0 - dist / radius) * 10.0; 
                            
                            cells[idx].vx += dx * power;
                            cells[idx].vy += dy * power;
                        }
                    }
                }
            }
        }
    }
    prevLocalX = gridX;
    prevLocalY = gridY;
}

//--------------------------------------------------------------
void FluidsCreature::advect() {
    prevCells = cells; 
    float dt = 1.0f;
    
    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            FluidCell& c = cells[idx];
            FluidCell& old = prevCells[idx];
            
            float px = i - old.vx * dt;
            float py = j - old.vy * dt;
            if (px < 0.5f) px = 0.5f; if (px > gridWidth + 0.5f) px = gridWidth + 0.5f;
            if (py < 0.5f) py = 0.5f; if (py > gridHeight + 0.5f) py = gridHeight + 0.5f;
            
            int ix = (int)px; int iy = (int)py;
            float dx = px - ix; float dy = py - iy;
            
            int idx1 = getIndex(ix, iy); int idx2 = getIndex(ix + 1, iy);
            int idx3 = getIndex(ix, iy + 1); int idx4 = getIndex(ix + 1, iy + 1);
            
            FluidCell& adv1 = prevCells[idx1]; FluidCell& adv2 = prevCells[idx2];
            FluidCell& adv3 = prevCells[idx3]; FluidCell& adv4 = prevCells[idx4];

            // Vélocité
            float vx12 = adv1.vx + dx * (adv2.vx - adv1.vx);
            float vy12 = adv1.vy + dx * (adv2.vy - adv1.vy);
            float vx34 = adv3.vx + dx * (adv4.vx - adv3.vx);
            float vy34 = adv3.vy + dx * (adv4.vy - adv3.vy);
            c.vx = vx12 + dy * (vx34 - vx12);
            c.vy = vy12 + dy * (vy34 - vy12);

            // Couleur
            float r12 = adv1.r + dx * (adv2.r - adv1.r);
            float g12 = adv1.g + dx * (adv2.g - adv1.g);
            float b12 = adv1.b + dx * (adv2.b - adv1.b);
            float r34 = adv3.r + dx * (adv4.r - adv3.r);
            float g34 = adv3.g + dx * (adv4.g - adv3.g);
            float b34 = adv3.b + dx * (adv4.b - adv3.b);
            c.r = r12 + dy * (r34 - r12);
            c.g = g12 + dy * (g34 - g12);
            c.b = b12 + dy * (b34 - b12);

            float a12 = adv1.a + dx * (adv2.a - adv1.a);
            float a34 = adv3.a + dx * (adv4.a - adv3.a);
            c.a = a12 + dy * (a34 - a12);

            // Decay minimal de vitesse
            c.vx *= 0.99f; 
            c.vy *= 0.99f;
        }
    }
}

//--------------------------------------------------------------
void FluidsCreature::project() {
    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            int l = getIndex(i-1, j); int r = getIndex(i+1, j);
            int t = getIndex(i, j-1); int b = getIndex(i, j+1);
            prevCells[idx].vx = -0.5f * (cells[r].vx - cells[l].vx + cells[b].vy - cells[t].vy);
            prevCells[idx].vy = 0; 
        }
    }
    for (int k = 0; k < 8; k++) {
        for (int j = 1; j <= gridHeight; j++) {
            for (int i = 1; i <= gridWidth; i++) {
                int idx = getIndex(i, j);
                int l = getIndex(i-1, j); int r = getIndex(i+1, j);
                int t = getIndex(i, j-1); int b = getIndex(i, j+1);
                prevCells[idx].vy = (prevCells[idx].vx + prevCells[l].vy + prevCells[r].vy + prevCells[t].vy + prevCells[b].vy) * 0.25f;
            }
        }
    }
    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            int l = getIndex(i-1, j); int r = getIndex(i+1, j);
            int t = getIndex(i, j-1); int b = getIndex(i, j+1);
            cells[idx].vx -= 0.5f * (prevCells[r].vy - prevCells[l].vy);
            cells[idx].vy -= 0.5f * (prevCells[b].vy - prevCells[t].vy);
        }
    }
}