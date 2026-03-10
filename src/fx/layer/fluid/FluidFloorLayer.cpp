#include "FluidFloorLayer.h"
#include "ColliderLayer.h"

//--------------------------------------------------------------
void FluidFloorLayer::setup(float w, float h, float s, int resX, int resY) {
    width = w;
    height = h;
    scale = s;
    gridWidth = resX;
    gridHeight = resY;
    
    scaleX = width / (float)gridWidth;
    scaleY = height / (float)gridHeight;

    int size = (gridWidth + 2) * (gridHeight + 2);
    cells.resize(size);
    prevCells.resize(size);
    initialColors.resize(size);

    fluidImage.allocate(gridWidth, gridHeight, OF_IMAGE_COLOR_ALPHA);
    fluidImage.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
    fluidImage.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);

    // Chargement de l'image
    if(!bgImage.isAllocated()) bgImage.load("IMG_REF/bg2.png");

    resetPattern();
    
    prevLocalX = 0; prevLocalY = 0;
}

//--------------------------------------------------------------
void FluidFloorLayer::resetPattern() {
    if (bUseImage && bgImage.isAllocated()) {
        colorFromImage();
        return;
    }

    // Création du Checkerboard avec transparence
    for(int j = 1; j <= gridHeight; j++) {
        for(int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            
            // Logique Checkerboard
            int checkSize = 8; // Taille des cases en cellules
            bool isCheck = ((i / checkSize) + (j / checkSize)) % 2 == 0;
            
            ofColor c;
            if (isCheck) {
                // Cases colorées
                c.setHsb(ofMap(i, 1, gridWidth, 0, 255), 200, 255);
                // Quelques cases transparentes aléatoires
                if(ofRandom(1.0) > 0.8) c.a = 0; 
                else c.a = 200;
            } else {
                // Cases "vides" (ou autre couleur)
                c = ofColor(50, 0, 100, 50); // Violet sombre semi-transparent
            }

            // Stockage
            cells[idx].r = c.r / 255.0f;
            cells[idx].g = c.g / 255.0f;
            cells[idx].b = c.b / 255.0f;
            cells[idx].a = c.a / 255.0f;
            
            initialColors[idx] = c;
        }
    }
}

//--------------------------------------------------------------
void FluidFloorLayer::colorFromImage() {
    ofImage temp = bgImage;
    // On redimensionne l'image à la taille de la grille de simulation
    temp.resize(gridWidth, gridHeight);
    ofPixels& pix = temp.getPixels();
    int numChannels = pix.getNumChannels();

    for(int j = 1; j <= gridHeight; j++) {
        for(int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            ofColor c = pix.getColor(i-1, j-1);
            
            // Stockage dans la simulation
            cells[idx].r = c.r / 255.0f;
            cells[idx].g = c.g / 255.0f;
            cells[idx].b = c.b / 255.0f;
            cells[idx].a = (numChannels == 4) ? c.a / 255.0f : 1.0f;
            
            // Stockage dans la mémoire pour la régénération
            initialColors[idx] = c;
        }
    }
}

//--------------------------------------------------------------
void FluidFloorLayer::setCollider(shared_ptr<ColliderLayer> colliders) {
    this->colliderLayer = colliders;
    if (!colliderLayer) return;

    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            // Coordonnées locales du fluide
            float fluidX = (i - 0.5f) * scaleX;
            float fluidY = (j - 0.5f) * scaleY;

            // Le fluide est maintenant dans le même espace que le collider (Sim Space)
            float simX = fluidX;
            float simY = fluidY;

            int idx = getIndex(i, j);
            cells[idx].isWall = colliderLayer->isWall(simX, simY);
        }
    }
}

//--------------------------------------------------------------
void FluidFloorLayer::toggleBackground() {
    bUseImage = !bUseImage;
    resetPattern();
}

//--------------------------------------------------------------
void FluidFloorLayer::update(float mx, float my) {
    // Conversion souris Monde -> Sim
    checkInput(mx / scale, my / scale);
    
    advect();
    project();
    regenerateColors();

    // Mise à jour de la texture
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
void FluidFloorLayer::draw(float x, float y) {
    ofPushMatrix();
    ofTranslate(x, y);
    ofScale(scale, scale);
    ofSetColor(255);
    fluidImage.draw(0, 0, width, height);
    ofPopMatrix();
}

//--------------------------------------------------------------
void FluidFloorLayer::checkInput(float localX, float localY) {
    // Mapping vers la grille
    float gridX = localX / scaleX;
    float gridY = localY / scaleY;
    
    // Wrapping de la souris pour l'interaction continue
    if (gridX < 1) gridX += gridWidth;
    if (gridX > gridWidth) gridX -= gridWidth;

    // Interaction limitée aux 400px du bas
   // float interactY = (height > 400.0f) ? height - 400.0f : 0.0f;
        if (localY >= 0 && localY <= height) {

   //if (localY >= interactY && localY <= height) {
        float dx = gridX - prevLocalX;
        float dy = gridY - prevLocalY;
        
        // Gestion du saut de wrapping pour la vélocité (éviter un trait géant quand on passe du bord droit au gauche)
        if (abs(dx) > gridWidth * 0.5) dx = 0; 

        if (abs(dx) > 0.1 || abs(dy) > 0.1) {
            int radius = 6; 
            int cx = (int)gridX;
            int cy = (int)gridY;

            for(int j = cy - radius; j <= cy + radius; j++) {
                for(int i = cx - radius; i <= cx + radius; i++) {
                    // Ici on utilise getIndex qui gère le wrapping i
                    if(j > 0 && j <= gridHeight) {
                        // Distance approximative (ne gère pas parfaitement le wrap distance mais suffisant)
                        float dist = ofDist(gridX, gridY, i, j);
                        if (dist < radius) {
                            int idx = getIndex(i, j);
                            float power = (1.0 - dist / radius) * 5.0; 
                            
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
void FluidFloorLayer::regenerateColors() {
    float returnSpeed = 0.01f; 
    for(int j = 1; j <= gridHeight; j++) {
        for(int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            
            float tr = initialColors[idx].r / 255.0f;
            float tg = initialColors[idx].g / 255.0f;
            float tb = initialColors[idx].b / 255.0f;
            float ta = initialColors[idx].a / 255.0f;

            cells[idx].r += (tr - cells[idx].r) * returnSpeed;
            cells[idx].g += (tg - cells[idx].g) * returnSpeed;
            cells[idx].b += (tb - cells[idx].b) * returnSpeed;
            cells[idx].a += (ta - cells[idx].a) * returnSpeed;
        }
    }
}

//--------------------------------------------------------------
void FluidFloorLayer::advect() {
    prevCells = cells; 
    float dt = 1.0f;
    
    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            
            // Si c'est un mur, pas de mouvement
            if (cells[idx].isWall) {
                cells[idx].vx = 0;
                cells[idx].vy = 0;
                continue;
            }
            
            FloorFluidCell& c = cells[idx];
            FloorFluidCell& old = prevCells[idx];
            
            float px = i - old.vx * dt;
            float py = j - old.vy * dt;
            
            // Wrapping X
            if (px < 0.5f) px += gridWidth;
            if (px > gridWidth + 0.5f) px -= gridWidth;
            
            // Clamping Y
            if (py < 0.5f) py = 0.5f; 
            if (py > gridHeight + 0.5f) py = gridHeight + 0.5f;
            
            int ix = (int)px; int iy = (int)py;
            float dx = px - ix; float dy = py - iy;
            
            // getIndex gère le wrapping pour ix+1
            int idx1 = getIndex(ix, iy); int idx2 = getIndex(ix + 1, iy);
            int idx3 = getIndex(ix, iy + 1); int idx4 = getIndex(ix + 1, iy + 1);
            
            FloorFluidCell& adv1 = prevCells[idx1]; FloorFluidCell& adv2 = prevCells[idx2];
            FloorFluidCell& adv3 = prevCells[idx3]; FloorFluidCell& adv4 = prevCells[idx4];

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

            c.vx *= 0.99f; 
            c.vy *= 0.99f;
        }
    }
}

//--------------------------------------------------------------
void FluidFloorLayer::project() {
    // Setup pression
    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            if (cells[idx].isWall) continue;

            int l = getIndex(i-1, j); int r = getIndex(i+1, j);
            int t = getIndex(i, j-1); int b = getIndex(i, j+1);
            
            // Gestion des voisins murs pour la divergence
            float v_r = cells[r].isWall ? 0 : cells[r].vx;
            float v_l = cells[l].isWall ? 0 : cells[l].vx;
            float v_b = cells[b].isWall ? 0 : cells[b].vy;
            float v_t = cells[t].isWall ? 0 : cells[t].vy;
            
            prevCells[idx].vx = -0.5f * (v_r - v_l + v_b - v_t);
            prevCells[idx].vy = 0; 
        }
    }
    // Solver
    for (int k = 0; k < 8; k++) {
        for (int j = 1; j <= gridHeight; j++) {
            for (int i = 1; i <= gridWidth; i++) {
                int idx = getIndex(i, j);
                if (cells[idx].isWall) continue;

                int l = getIndex(i-1, j); int r = getIndex(i+1, j);
                int t = getIndex(i, j-1); int b = getIndex(i, j+1);
                
                // Gestion de la pression aux frontières (Neumann: pression voisine = pression courante)
                float p_l = cells[l].isWall ? prevCells[idx].vy : prevCells[l].vy;
                float p_r = cells[r].isWall ? prevCells[idx].vy : prevCells[r].vy;
                float p_t = cells[t].isWall ? prevCells[idx].vy : prevCells[t].vy;
                float p_b = cells[b].isWall ? prevCells[idx].vy : prevCells[b].vy;

                prevCells[idx].vy = (prevCells[idx].vx + p_l + p_r + p_t + p_b) * 0.25f;
            }
        }
    }
    // Application
    for (int j = 1; j <= gridHeight; j++) {
        for (int i = 1; i <= gridWidth; i++) {
            int idx = getIndex(i, j);
            if (cells[idx].isWall) {
                cells[idx].vx = 0;
                cells[idx].vy = 0;
                continue;
            }

            int l = getIndex(i-1, j); int r = getIndex(i+1, j);
            int t = getIndex(i, j-1); int b = getIndex(i, j+1);
            
            float p_c = prevCells[idx].vy;
            float p_l = cells[l].isWall ? p_c : prevCells[l].vy;
            float p_r = cells[r].isWall ? p_c : prevCells[r].vy;
            float p_t = cells[t].isWall ? p_c : prevCells[t].vy;
            float p_b = cells[b].isWall ? p_c : prevCells[b].vy;
            
            cells[idx].vx -= 0.5f * (p_r - p_l);
            cells[idx].vy -= 0.5f * (p_b - p_t);
        }
    }
}

//--------------------------------------------------------------
void FluidFloorLayer::addForce(float localX, float localY, float forceX, float forceY) {
    float gridX = localX / scaleX;
    float gridY = localY / scaleY;
    
    // Wrapping X
    if (gridX < 1) gridX += gridWidth;
    if (gridX > gridWidth) gridX -= gridWidth;

    // Check bounds Y
    if (gridY > 0 && gridY <= gridHeight) {
        int radius = 5; // Rayon d'impact
        int cx = (int)gridX;
        int cy = (int)gridY;

        for(int j = cy - radius; j <= cy + radius; j++) {
            for(int i = cx - radius; i <= cx + radius; i++) {
                if(j > 0 && j <= gridHeight) {
                    float dist = ofDist(gridX, gridY, i, j);
                    if (dist < radius) {
                        int idx = getIndex(i, j);
                        float power = (1.0 - dist / radius);
                        
                        cells[idx].vx += forceX * power;
                        cells[idx].vy += forceY * power;
                    }
                }
            }
        }
    }
}
