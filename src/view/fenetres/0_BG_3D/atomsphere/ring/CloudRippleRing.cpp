#include "CloudRippleRing.h"

ofImage CloudRippleRing::cloudTexture;
bool CloudRippleRing::bTextureAllocated = false;

//--------------------------------------------------------------
void CloudRippleRing::allocateTexture() {
    if (bTextureAllocated) return;
    
    ofDisableArbTex(); // Indispensable pour le mapping sphérique parfait (UV 0.0 à 1.0)
    
    // On charge la texture de la forêt
    if (cloudTexture.load("Z_extra/1Forets.jpg")) {
        // Le miroir est retiré. On utilise ofScale(-1,1,1) au rendu pour correspondre à bShow360
        cloudTexture.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
        cloudTexture.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
        bTextureAllocated = true;
        return; // On arrête ici si l'image est bien chargée
    }

    // --- FALLBACK SI L'IMAGE MANQUE (Nuages procéduraux par défaut) ---
    int texW = 1024;
    int texH = 512;
    ofPixels pix;
    pix.allocate(texW, texH, OF_PIXELS_RGBA);
    
    float noiseOffset = ofRandom(1000.0f);

    for (int y = 0; y < texH; y++) {
        for (int x = 0; x < texW; x++) {
            float u = (float)x / texW;
            float v = (float)y / texH;
            
            // MAPPING SPHÉRIQUE : On génère le bruit en 3D pour éviter tout raccord ou étirement
            float phi = u * TWO_PI;
            float theta = v * PI;
            
            float nx = sin(theta) * cos(phi) * 2.0f;
            float nz = sin(theta) * sin(phi) * 2.0f;
            float ny = cos(theta) * 2.0f;
            
            float noiseVal = ofNoise(nx, ny, nz + noiseOffset);
            noiseVal = pow(noiseVal, 1.2f);
            
            float edgeFade = sin(v * PI); 
            
            // Nuages bien clairs et éthérés
            ofColor color(180, 220, 255, 255 * noiseVal * edgeFade * 1.5f);
            pix.setColor(x, y, color);
        }
    }
    cloudTexture.setFromPixels(pix);
    cloudTexture.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    cloudTexture.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
    bTextureAllocated = true;
}

//--------------------------------------------------------------
void CloudRippleRing::loadTexture(string path) {
    if (cloudTexture.load(path)) {
        cloudTexture.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
        cloudTexture.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
        bTextureAllocated = true;
    }
}

//--------------------------------------------------------------
void CloudRippleRing::setup(float r, ofVec3f centerPos) {
    radius = r;
    center = centerPos;
    
    allocateTexture();
    
    resX = 180;
    resY = 90; // Plus de résolution en Y pour une belle sphère

    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    baseUVs.clear();

    // Génération des sommets de la SPHERE
    for (int iy = 0; iy <= resY; iy++) {
        float v = (float)iy / resY;
        float theta = v * PI;
        
        for (int ix = 0; ix <= resX; ix++) {
            float u = (float)ix / resX;
            float phi = u * TWO_PI;
            
            float xPos = radius * sin(theta) * cos(phi);
            float yPos = radius * cos(theta);
            float zPos = radius * sin(theta) * sin(phi);
            
            // On centre le mesh sur 0,0,0 (le positionnement exact se fera dans draw() via scale/translate)
            mesh.addVertex(ofVec3f(xPos, yPos, zPos));
            mesh.addTexCoord(ofVec2f(u, v));
            mesh.addColor(ofColor(255, 255, 255, 255));
            
            baseUVs.push_back(ofVec2f(u, v));
        }
    }

    // Génération des triangles
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
void CloudRippleRing::addRipple(float u, float v) {
    Ripple r;
    r.u = u;
    r.v = v;
    r.life = 0.0f;
    r.maxLife = 5.0f; // Longue onde lente
    ripples.push_back(r);
}

//--------------------------------------------------------------
void CloudRippleRing::update(float dt) {
    for (auto& r : ripples) r.life += dt;
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return r.life >= r.maxLife; }), ripples.end());

    float circ = TWO_PI * radius;
    float totH = PI * radius; // Périmètre vertical (Pôle Nord au Pôle Sud)

    int idx = 0;
    // Déformation dynamique des UV pour simuler une "lentille" ou onde de choc
    for (int iy = 0; iy <= resY; iy++) {
        for (int ix = 0; ix <= resX; ix++) {
            ofVec2f uv = baseUVs[idx];
            ofVec2f uvOffset(0, 0);

            for (const auto& r : ripples) {
                // Distance circulaire pour U
                float du = uv.x - r.u;
                if (du > 0.5f) du -= 1.0f;
                else if (du < -0.5f) du += 1.0f;
                
                float dv = uv.y - r.v;
                
                // Correction sphérique : la distance horizontale rétrécit aux pôles
                float latScale = sin(uv.y * PI);
                float dx = du * circ * latScale;
                float dy = dv * totH;
                float dist = sqrt(dx*dx + dy*dy);
                
                // Évolution de l'onde
                float t = r.life / r.maxLife;
                // DOUCEUR : L'onde ralentit progressivement (Ease Out Quad)
                float easedT = 1.0f - pow(1.0f - t, 2.0f);
                float maxWaveRadius = 3000.0f;
                float currentRadius = easedT * maxWaveRadius;
                float waveDist = dist - currentRadius;
                
                float waveWidth = 500.0f; // Onde plus large pour lisser la transition
                
                if (abs(waveDist) < waveWidth && dist > 5.0f) {
                    // Enveloppe cosinus pour un fondu très doux aux bords de l'onde (au lieu de linéaire)
                    float normalizedDist = waveDist / waveWidth; // -1 à 1
                    float waveIntensity = (cos(normalizedDist * PI) + 1.0f) * 0.5f; 
                    
                    waveIntensity *= (1.0f - t); // S'estompe avec le temps
                    
                    // Fade-in au début pour éviter le saut d'image (pop) à la 1ère frame
                    float fadeIn = std::min(1.0f, r.life * 3.0f); 
                    waveIntensity *= fadeIn;
                    
                    float wavePulse = sin(waveDist * 0.02f); // Fréquence plus basse (ondes amples)
                    float displacement = wavePulse * waveIntensity * 0.05f; // Amplitude réduite
                    
                    float safeLatScale = max(0.15f, latScale); // Limite relevée pour les pôles
                    float dispX = (dx / dist) * displacement / safeLatScale;
                    float dispY = (dy / dist) * displacement;
                    
                    // Clamping de sécurité pour s'assurer qu'aucun sommet ne "saute"
                    uvOffset.x += ofClamp(dispX, -0.1f, 0.1f);
                    uvOffset.y += ofClamp(dispY, -0.1f, 0.1f);
                }
            }
            
            mesh.setTexCoord(idx, uv + uvOffset);
            idx++;
        }
    }
}

//--------------------------------------------------------------
void CloudRippleRing::draw() {
    ofPushStyle();
    // On utilise l'Alpha Blending classique pour voir le paysage solidement
    ofEnableAlphaBlending();
    glDepthMask(GL_FALSE); 
    
    ofPushMatrix();
    ofTranslate(center);
    ofRotateYDeg(-90.0f); // Rotation de 90 degrés supplémentaire (vers la gauche)
    // Le ofScale(-1, 1, 1) a été retiré pour corriger l'effet miroir (inversion Cour/Jar)
    
    cloudTexture.bind();
    mesh.draw();
    cloudTexture.unbind();
    
    ofPopMatrix();
    
    glDepthMask(GL_TRUE);
    ofPopStyle();
}