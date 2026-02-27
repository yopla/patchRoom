#include "RippleSystem.h"

//--------------------------------------------------------------
void RippleSystem::setup(int count) {
    generateCircleTexture();

    // On crée les agents vides, ils seront initialisés au premier update
    agents.resize(count);
    for(auto& agent : agents) {
        agent.age = 100.0f; // Force le reset immédiat
        agent.lifeSpan = 1.0f;
    }
}

//--------------------------------------------------------------
void RippleSystem::generateCircleTexture() {
    int size = 256;
    ofFbo fbo;
    fbo.allocate(size, size, GL_RGBA);
    fbo.begin();
    ofClear(0, 0, 0, 0);
    
    float cx = size / 2.0f;
    float cy = size / 2.0f;
    float rOuter = size * 0.45f;
    float rInner = size * 0.30f;
    
    ofSetColor(255);
    ofSetCircleResolution(60);
    
    // On dessine un anneau
    // Façon simple : Grand cercle blanc - Petit cercle transparent (avec blending spécial)
    // Mais ici on fait simple : Path
    ofPath path;
    path.setCircleResolution(60);
    path.setColor(ofColor::white);
    path.circle(cx, cy, rOuter);
    path.close();
    path.circle(cx, cy, rInner); // Trou au milieu
    path.setPolyWindingMode(OF_POLY_WINDING_ODD);
    path.draw();
    
    fbo.end();
    
    fbo.readToPixels(circleTexture.getPixels());
    circleTexture.update();
    // Setup texture parameters pour la projection
    circleTexture.getTexture().bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    circleTexture.getTexture().unbind();
}

//--------------------------------------------------------------
void RippleSystem::spawnAgent(RippleAgent& agent, RoomWalls& walls) {
    // 1. Définir un rayon aléatoire depuis le centre de la pièce
    ofVec3f center(0, 600, 0);
    ofVec3f randomDir = ofVec3f(ofRandom(-1, 1), ofRandom(-0.5, 0.5), ofRandom(-1, 1)).getNormalized();
    
    // 2. Raycast contre les murs (Logique simplifiée inspirée de ProjectionSystem)
    float minDistance = 100000.0f;
    ofVec3f bestHit = center;
    bool found = false;

    auto check = [&](ofMesh& mesh) {
        if(mesh.getNumVertices() < 3) return;
        // On prend le plan du premier triangle (approximation suffisante pour des murs plats)
        ofVec3f v0 = mesh.getVertex(0);
        ofVec3f v1 = mesh.getVertex(1);
        ofVec3f v2 = mesh.getVertex(2);
        ofVec3f normal = ((v1 - v0).cross(v2 - v0)).getNormalized();
        
        float denom = normal.dot(randomDir);
        if (abs(denom) > 0.0001f) {
            float t = (v0 - center).dot(normal) / denom;
            if (t > 0 && t < minDistance) {
                // Vérif si le point est dans les bornes "globales" de la pièce (simple box check)
                ofVec3f p = center + randomDir * t;
                if(abs(p.x) < 1300 && abs(p.z) < 1400 && p.y > -10 && p.y < 1700) {
                    minDistance = t;
                    bestHit = p;
                    found = true;
                }
            }
        }
    };

    check(walls.meshFront);
    check(walls.meshBack);
    check(walls.meshCour);
    check(walls.meshJar);
    check(walls.meshSol);
    check(walls.meshTopCour);
    check(walls.meshTopJar);

    if(!found) {
        // Fallback si le rayon sort par un trou : on réessaie ou on tape le sol par défaut
        bestHit = ofVec3f(ofRandom(-1000, 1000), 0, ofRandom(-1000, 1000));
    }

    // 3. Setup de l'agent
    agent.hitPoint = bestHit;
    agent.age = 0.0f;
    agent.lifeSpan = 1.0f;
    
    // Couleur aléatoire vibrante
    agent.color = ofColor::fromHsb(ofRandom(255), 200, 255);
    
    // Position du projecteur : On recule depuis le mur vers le centre
    ofVec3f dirToCenter = (center - bestHit).getNormalized();
    float distFromWall = 800.0f;
    
    agent.projector.setPosition(bestHit + dirToCenter * distFromWall);
    agent.projector.lookAt(bestHit);
    agent.projector.setNearClip(10);
    agent.projector.setFarClip(2000);
    agent.projector.setAspectRatio(1.0); // Carré pour le cercle
}

//--------------------------------------------------------------
void RippleSystem::update(RoomWalls& walls) {
    float dt = 1.0f / 60.0f; // Delta time fixe approximatif
    
    for(auto& agent : agents) {
        agent.age += dt;
        
        if(agent.isDead()) {
            spawnAgent(agent, walls);
        }
        
        // Animation du FOV (Ouverture du cercle)
        // Ease Out Cubic: 1 - pow(1 - t, 3)
        float pct = agent.age / agent.lifeSpan;
        float ease = 1.0f - pow(1.0f - pct, 3.0f); 
        
        // Le cercle s'élargit -> on augmente le FOV du projecteur
        float minFov = 5.0f;
        float maxFov = 40.0f;
        float currentFov = ofMap(ease, 0, 1, minFov, maxFov);
        
        agent.projector.setFov(currentFov);
    }
}

//--------------------------------------------------------------
void RippleSystem::draw(RoomWalls& walls) {
    ofPushStyle();
    ofEnableAlphaBlending();
    
    // Optimisation OpenGL
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0, -1.0); // Evite le z-fighting avec les murs
    
    circleTexture.getTexture().bind();
    
    // Setup Texture generation (mode projecteur)
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    
    float planeS[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    float planeT[] = { 0.0f, 1.0f, 0.0f, 0.0f };
    float planeR[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float planeQ[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    glTexGenfv(GL_S, GL_OBJECT_PLANE, planeS);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, planeT);
    glTexGenfv(GL_R, GL_OBJECT_PLANE, planeR);
    glTexGenfv(GL_Q, GL_OBJECT_PLANE, planeQ);

    for(auto& agent : agents) {
        // Calcul Alpha (Fade out à la fin)
        float pct = agent.age / agent.lifeSpan;
        float alpha = 255;
        if(pct > 0.7f) alpha = ofMap(pct, 0.7f, 1.0f, 255, 0);
        
        ofSetColor(agent.color, alpha);

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(0.5, 0.5, 0.5); 
        glScalef(0.5, 0.5, 0.5);
        ofMatrix4x4 projMat = agent.projector.getModelViewProjectionMatrix();
        glMultMatrixf(projMat.getPtr());
        glMatrixMode(GL_MODELVIEW);

        // Clip Plane pour ne pas projeter derrière le projecteur
        ofVec3f pos = agent.projector.getPosition();
        ofVec3f norm = agent.projector.getLookAtDir(); 
        double clipPlaneEq[] = { (double)norm.x, (double)norm.y, (double)norm.z, (double)(-norm.dot(pos)) };
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, clipPlaneEq);

        // Dessin des murs (Cibles de la projection)
        // Note: On pourrait optimiser en ne dessinant que les murs proches
        // mais vu la config, dessiner tout passe souvent bien.
        walls.meshFront.draw();
        walls.meshBack.draw();
        walls.meshJar.draw();
        walls.meshCour.draw();
        walls.meshSol.draw();
        walls.meshTopCour.draw(); 
        walls.meshTopJar.draw(); 

        glDisable(GL_CLIP_PLANE0);
        
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    
    circleTexture.getTexture().unbind();
    glDisable(GL_POLYGON_OFFSET_FILL);
    ofPopStyle();
}