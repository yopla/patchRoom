#include "LightFlyRing.h"

//--------------------------------------------------------------
void LightFlyRing::setup(float r, float topH, float botExt) {
    radius = r;
    height = topH;
    bottomExt = botExt;

    // 1. Setup Texture FBO (4096x2048 : Compromis idéal finesse/compatibilité)
    fbo.allocate(8192, 2048, GL_RGBA);
    fbo.begin(); ofClear(0,0,0,0); fbo.end();
    fbo.getTexture().setTextureWrap(GL_REPEAT, GL_CLAMP_TO_EDGE);

    // 2. Create Mesh (Cylinder/Ring)
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    int resolution = 512;
    for(int i = 0; i <= resolution; i++) {
        float pct = (float)i / (float)resolution;
        float angle = pct * TWO_PI;
        
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float u = pct;
        
        // Top Vertex (v=0)
        mesh.addVertex(ofVec3f(x, height, z));
        mesh.addTexCoord(ofVec2f(u, 0));

        // Bottom Vertex (v=1)
        mesh.addVertex(ofVec3f(x, -bottomExt, z));
        mesh.addTexCoord(ofVec2f(u, 1.0f));
    }
    
    interactPos.set(-1000, -1000);
    isInteracting = false;
}

//--------------------------------------------------------------
void LightFlyRing::update() {
    // 1. Update Creatures
    // On passe la position d'interaction (souris projetée sur le cylindre)
    float mx = isInteracting ? interactPos.x : -10000.0f;
    float my = isInteracting ? interactPos.y : -10000.0f;

    for(auto& c : creatures) {
        c->update(mx, my);
    }

    // Calcul de la correction d'aspect ratio pour que les halos soient ronds sur le cylindre
    float circumference = TWO_PI * radius;
    float totalHeight = height + bottomExt;
    
    // Combien d'unités physiques par pixel de texture ?
    float unitsPerPixelX = circumference / fbo.getWidth();
    float unitsPerPixelY = totalHeight / fbo.getHeight();
    
    // On compense l'étirement horizontal (X) par rapport au vertical (Y)
    float scaleCorrectionX = unitsPerPixelY / unitsPerPixelX;

    // ECHELLE GLOBALE : On adapte la taille du dessin à la résolution du FBO
    // pour que les créatures gardent la même taille physique (visible) quelle que soit la résolution.
    float haloSize = 0.35f; // Variable pour ajuster la taille des halos (0.35 = plus petit)
    float globalScale = (fbo.getWidth() / 2048.0f) * haloSize;

    // 2. Render to FBO
    fbo.begin();
    //ofClear(38, 38, 38, 255); // Fond gris à 15%
    ofClear(15, 15, 15, 255); // Fond gris à 15%
    float w = fbo.getWidth();
    for(auto& c : creatures) {
        c->draw(scaleCorrectionX * globalScale, globalScale);
        
        // Dessin dupliqué pour la continuité (Wrap) gauche/droite
        ofPushMatrix();
        ofTranslate(w, 0);
        c->draw(scaleCorrectionX * globalScale, globalScale);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(-w, 0);
        c->draw(scaleCorrectionX * globalScale, globalScale);
        ofPopMatrix();
    }
    fbo.end();
}

//--------------------------------------------------------------
void LightFlyRing::draw() {
    ofPushStyle();
    glDepthMask(GL_FALSE);
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    ofSetColor(255);
    fbo.getTexture().bind();
    mesh.draw();
    fbo.getTexture().unbind();

    glDepthMask(GL_TRUE);
    ofPopStyle();
}

//--------------------------------------------------------------
void LightFlyRing::addLightAt(float u, float v) {
    float x = u * fbo.getWidth();
    float y = v * fbo.getHeight();
    creatures.push_back(make_shared<HaloCreature>(x, y));
}

//--------------------------------------------------------------
void LightFlyRing::clearLights() {
    creatures.clear();
}

//--------------------------------------------------------------
void LightFlyRing::setInteraction(float u, float v) {
    if(u < 0 || v < 0) {
        isInteracting = false;
        interactPos.set(-1000, -1000);
    } else {
        isInteracting = true;
        interactPos.set(u * fbo.getWidth(), v * fbo.getHeight());
    }
}
