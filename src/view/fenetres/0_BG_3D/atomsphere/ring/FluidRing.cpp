#include "FluidRing.h"

//--------------------------------------------------------------
void FluidRing::setup(float r, float topH, float botExt) {
    radius = r;
    height = topH;
    bottomExt = botExt;

    globalAlpha = 0.0f;
    targetAlpha = 0.0f;

    // 1. Configuration de la simulation fluide
    // On utilise une résolution suffisante pour faire le tour

    // --- GÉNÉRATION PROCÉDURALE DE LA BRUME ---
    int texW = 2048;
    int texH = 1024;
    ofPixels mistPix;
    mistPix.allocate(texW, texH, OF_PIXELS_RGBA);

    for (int y = 0; y < texH; y++) {
        for (int x = 0; x < texW; x++) {
            float u = (float)x / texW;
            float v = (float)y / texH;
            
            // Projection sur un cercle 3D pour un bouclage X parfait (Seamless)
            float theta = u * TWO_PI;
            float radius = 0.75f; // Échelle globale du bruit
            
            float value = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float maxVal = 0.0f;
            
            // 4 octaves pour donner du détail ("wisps" de fumée)
            for(int o = 0; o < 4; o++) {
                float nx = cos(theta) * radius * frequency;
                float nz = sin(theta) * radius * frequency;
                float ny = v * 2.5f * frequency; // Étirement vertical
                
                value += ofNoise(nx, ny, nz) * amplitude;
                maxVal += amplitude;
                
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }
            value /= maxVal; // Normalisation 0.0 - 1.0
            
            // Stylisation : Courbe d'atténuation (pow) pour des volutes plus diffuses
            value = pow(value, 1.8f);
            
        
            
            // Interpolation élégante : d'un bleu nuit/transparent vers un blanc/cyan éthéré
            ofColor darkColor(10, 15, 30, 0);
            ofColor lightColor(180, 220, 255, 225);
            
            mistPix.setColor(x, y, darkColor.getLerped(lightColor, value));
        }
    }
    

    //fluid.bgImage.load("IMG_REF/brumes.png");
    fluid.bgImage.setFromPixels(mistPix);
    fluid.bUseImage = true;

    // Largeur texture = 2048 (pour boucler proprement), Hauteur = 1024
    //fluid.setup(2048, 1024, 1.0f, 512, 256);

    fluid.setup(texW, texH, 1.0f, 512, 256);
    
    // On s'assure que la texture boucle horizontalement
    fluid.fluidImage.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);
    fluid.fluidImage.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);

    // On force le mode image pour avoir une base colorée si besoin, 
    // ou on laisse le pattern par défaut.
    // fluid.toggleBackground(); 

    // 2. Création du Maillage (Cylindre / Anneau)
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES); // Passage en TRIANGLES pour gérer les rangées du dégradé

    resolution = 100; // Nombre de segments pour le cercle
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
    // On ajoute un facteur d'échelle (rScale) pour courber le haut et le bas vers l'intérieur (ex: 0.85)
    baseLevels.clear();
    baseLevels.push_back({height, 0.0f, 0.0f, curveScale});
    baseLevels.push_back({height - curveHeight, curveHeight / totalHeight, 255.0f, 1.0f});
    baseLevels.push_back({-bottomExt + curveHeight, (totalHeight - curveHeight) / totalHeight, 255.0f, 1.0f});
    baseLevels.push_back({-bottomExt, 1.0f, 0.0f, curveScale});

    // Génération des sommets
    for(int j = 0; j < baseLevels.size(); j++) {
        for(int i = 0; i <= resolution; i++) {
            float pct = (float)i / (float)resolution;
            float angle = pct * TWO_PI;
            
            float currentRadius = radius * baseLevels[j].rScale;
            float x = cos(angle) * currentRadius;
            float z = sin(angle) * currentRadius;
            
            mesh.addVertex(ofVec3f(x, baseLevels[j].y, z));
            mesh.addTexCoord(ofVec2f(pct, baseLevels[j].v));
            mesh.addColor(ofColor(255, 255, 255, baseLevels[j].alpha * globalAlpha));
        }
    }

    // Génération des indices (Triangles)
    int stride = resolution + 1;
    for(int j = 0; j < baseLevels.size() - 1; j++) {
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

void FluidRing::setTargetAlpha(float target) {
    targetAlpha = target;
}

//--------------------------------------------------------------
void FluidRing::update() {
    // Animation automatique supprimée pour laisser le contrôle exclusif à la souris
    // La mise à jour du fluide (fluid.update) est désormais gérée par RoomApp
    // avec les coordonnées précises du Raycast.

    // Fade effect
    if (globalAlpha != targetAlpha) {
        float speed = 2.0f * ofGetLastFrameTime(); // 0.5 sec fade
        if (globalAlpha < targetAlpha) {
            globalAlpha += speed;
            if (globalAlpha > targetAlpha) globalAlpha = targetAlpha;
        } else {
            globalAlpha -= speed;
            if (globalAlpha < targetAlpha) globalAlpha = targetAlpha;
        }

        // Update vertex colors
        int vertexIndex = 0;
        for(int j = 0; j < baseLevels.size(); j++) {
            for(int i = 0; i <= resolution; i++) {
                mesh.setColor(vertexIndex, ofColor(255, 255, 255, baseLevels[j].alpha * globalAlpha));
                vertexIndex++;
            }
        }
    }
}

//--------------------------------------------------------------
void FluidRing::draw() {
    if (globalAlpha <= 0.0f) return;

    ofEnableAlphaBlending(); // Important pour que le dégradé fonctionne
    ofSetColor(255);
    
    // On bind la texture générée par le FluidFloorLayer
    fluid.fluidImage.bind();
    
    // On dessine le maillage
    mesh.draw();
    
    fluid.fluidImage.unbind();
}
