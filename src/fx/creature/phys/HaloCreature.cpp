#include "HaloCreature.h"

HaloCreature::HaloCreature(float x, float y) {
    pos = glm::vec2(x, y);
    haloRadius = 120.0f;
    birthTime = ofGetElapsedTimef();
    
    int numFlies = 15;
    for(int i=0; i<numFlies; i++) {
        HaloFly f;
        f.angle = ofRandom(TWO_PI);
        f.speed = ofRandom(0.02, 0.06) * (ofRandom(1.0) > 0.5 ? 1 : -1);
        f.radius = ofRandom(40, 100);
        f.yOffset = ofRandom(-30, 30);
        f.phase = ofRandom(100.0f);
        f.pos = glm::vec3(cos(f.angle)*f.radius, f.yOffset, sin(f.angle)*f.radius);
        flies.push_back(f);
    }
    
    // Allocation du FBO pour le masque d'ombre (taille suffisante pour le halo)
    fbo.allocate(800, 800, GL_RGBA);
}

void HaloCreature::update() {
    float fpsRec = 60.0f;
    float time = ofGetFrameNum() / fpsRec;

    for(auto& f : flies) {
        glm::vec3 target;
        
        if(isHovering) {
            // Les mouches montent au dessus de la lampe (fuite)
            float noiseX = ofSignedNoise(time * 0.5, f.phase) * 80.0f;
            float noiseY = -200.0f - abs(ofSignedNoise(time * 0.5, f.phase + 100)) * 100.0f; 
            float noiseZ = ofSignedNoise(time * 0.5, f.phase + 200) * 80.0f;
            target = glm::vec3(noiseX, noiseY, noiseZ);
        } else {
            // Orbite autour de la lampe
            f.angle += f.speed;
            float r = f.radius + sin(time * 2.0 + f.phase) * 5.0f;
            float y = f.yOffset + cos(time * 1.5 + f.phase) * 10.0f;
            target = glm::vec3(cos(f.angle)*r, y, sin(f.angle)*r);
        }
        
        f.pos += (target - f.pos) * 0.03f;
    }
}

void HaloCreature::draw(float scaleX, float scaleY) {
    // Animation d'apparition "Ting!" (Elastic Out)
    float age = ofGetElapsedTimef() - birthTime;
    float duration = 0.8f;
    if(age < duration) {
        float t = age / duration;
        float p = 0.3f;
        float animScale = pow(2, -10 * t) * sin((t - p / 4) * (2 * PI) / p) + 1;
        scaleX *= animScale;
        scaleY *= animScale;
    }

    float fpsRec = 60.0f;
    float time = ofGetFrameNum() / fpsRec;
    float pulse = 1.0f + 0.05f * sin(time * 3.0f);
    float currentRadius = haloRadius * pulse;
    float radiationRadius = currentRadius * 2.0f; // Rayonnement beaucoup plus long
    
    // --- 1. DESSIN DANS LE FBO (Halo + Masque d'Ombre) ---
    fbo.begin();
    ofClear(0, 0, 0, 0);
    
    ofPushMatrix();
    ofTranslate(400, 400); // Centre du FBO (800x800)
    
    // A. Halo Lumineux (Additive dans le FBO)
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    ofMesh haloMesh;
    haloMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    
    haloMesh.addVertex(glm::vec3(0,0,0)); // Centre
    haloMesh.addColor(ofColor(255, 255, 240, 255)); // Centre encore plus blanc/intense
    
    // Bord dégradé transparent
    int segs = 60;
    for(int i=0; i<=segs; i++) {
        float a = (TWO_PI * i) / (float)segs;
        float x = cos(a) * radiationRadius;
        float y = sin(a) * radiationRadius;
        haloMesh.addVertex(glm::vec3(x, y, 0));
        haloMesh.addColor(ofColor(255, 200, 100, 0)); 
    }
    haloMesh.draw();
    
    // Coeur de la lampe (Bulb)
    ofSetColor(255, 255, 255, 220 + 35 * sin(time * 10.0f));
    ofDrawCircle(0, 0, currentRadius * 0.25f);
    
    // B. Ombres (Soustraction / Masquage)
    // On utilise ce blend mode pour multiplier la couleur de destination par (1 - AlphaSource)
    // Là où l'ombre est opaque (Alpha 1), le halo devient noir/transparent.
    glBlendFuncSeparate(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    
    for(auto& f : flies) {
        glm::vec2 p(f.pos.x, f.pos.y);
        float d = glm::length(p);
        float r = 4.0f; // Rayon de l'objet pour l'ombre
        
        if(d > r && d < radiationRadius) {
            glm::vec2 dir = glm::normalize(p);
            glm::vec2 perp(-dir.y, dir.x);
            
            glm::vec2 t1 = p + perp * r;
            glm::vec2 t2 = p - perp * r;
            
            // Projection sur le bord du rayonnement
            glm::vec2 proj1 = glm::normalize(t1) * radiationRadius;
            glm::vec2 proj2 = glm::normalize(t2) * radiationRadius;
            
            ofMesh shadow;
            shadow.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            
            ofColor cInner(0, 0, 0, 255); // Alpha max pour effacer complètement le halo
            ofColor cOuter(0, 0, 0, 0);   // Transparent au bord
            
            shadow.addVertex(glm::vec3(t1.x, t1.y, 0)); shadow.addColor(cInner);
            shadow.addVertex(glm::vec3(t2.x, t2.y, 0)); shadow.addColor(cInner);
            shadow.addVertex(glm::vec3(proj1.x, proj1.y, 0)); shadow.addColor(cOuter);
            shadow.addVertex(glm::vec3(proj2.x, proj2.y, 0)); shadow.addColor(cOuter);
            
            shadow.draw();
        }
    }
    ofPopMatrix();
    fbo.end();
    
    // --- 2. DESSIN DU FBO SUR L'ECRAN ---
    // On dessine le résultat (Halo troué) en mode Additif pour qu'il s'ajoute au fond
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetColor(255);
    
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofScale(scaleX, scaleY);
    fbo.draw(-400, -400); // Dessin centré localement
    ofPopMatrix();
    
    // --- 3. MOUCHES (Gris par dessus) ---
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofScale(scaleX, scaleY); // On applique aussi l'échelle aux mouches
    
    for(auto& f : flies) {
        float scale = ofMap(f.pos.z, -60, 60, 0.5, 1.3);
        
        ofFill();
        ofSetColor(0); 
        ofDrawCircle(f.pos.x, f.pos.y, 4 * scale);
        
        ofNoFill();
        ofSetLineWidth(1.5);
        ofSetColor(255);
        ofDrawCircle(f.pos.x, f.pos.y, 4 * scale);
    }
    ofFill();
    
    ofPopMatrix();
}

bool HaloCreature::isInside(float mx, float my) {
    return ofDist(mx, my, pos.x, pos.y) < haloRadius;
}

void HaloCreature::setHovering(bool hovering) {
    isHovering = hovering;
}