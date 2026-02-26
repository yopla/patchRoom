#include "ProjectionSystem.h"

//--------------------------------------------------------------
void ProjectionSystem::setup() {
    float w = 500;
    float h = 550;
    
    // On alloue le FBO avec support Alpha
    planContentFbo.allocate(w, h, GL_RGBA);
    
    // Initialisation des particules
    int numParticles = 40; // Un peu moins de particules pour plus d'espace
    particles.clear();
    for(int i=0; i<numParticles; i++){
        Particle p;
        // On les fait apparaître plus écartées au départ
        p.pos = glm::vec3(ofRandom(w), ofRandom(h), 0);
        p.vel = glm::vec3(ofRandom(-2,2), ofRandom(-2,2), 0);
        p.size = ofRandom(3, 8);
        
        if(ofRandom(1) > 0.5) p.color = ofColor(0, 255, 255);
        else p.color = ofColor(255, 255, 255);
        
        particles.push_back(p);
    }

    updatePlanCollePosition();

    imgBeam.load("iconeRZEF.png");
    imgBeam.setImageType(OF_IMAGE_COLOR_ALPHA); 
    imgBeam.getTexture().bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    imgBeam.getTexture().unbind();

    currentSmoothedNormal.set(0, 0, 1);
    currentSmoothedPos.set(0, 500, 0);
    lastHitPoint.set(0, 0, 0);

    projector.setPosition(0, 500, 0); 
    projector.lookAt(ofVec3f(0, 0, -1000));
    projector.setNearClip(10);
    projector.setFarClip(5000);
}

//--------------------------------------------------------------
void ProjectionSystem::update() {
    float speedAngle = 1.0f;
    if(ofGetKeyPressed('d') || ofGetKeyPressed('D')) planAngle -= speedAngle;
    if(ofGetKeyPressed('q') || ofGetKeyPressed('Q')) planAngle += speedAngle;
    if(ofGetKeyPressed('z') || ofGetKeyPressed('Z')) planElevation += speedAngle; 
    if(ofGetKeyPressed('s') || ofGetKeyPressed('S')) planElevation -= speedAngle;

    updatePlanCollePosition();
    updateParticles();

    float distToWall = currentDist;
    if(distToWall < 1.0f) distToWall = 1000.0f; 

    float fovRad = 2.0f * atan((targetBeamHeight / 2.0f) / distToWall);
    projector.setFov(ofRadToDeg(fovRad));
    projector.setAspectRatio(targetBeamWidth / targetBeamHeight);
}

void ProjectionSystem::updateParticles() {
    float w = planContentFbo.getWidth();
    float h = planContentFbo.getHeight();
    glm::vec3 center(w/2, h/2, 0);

    // --- 1. PHYSIQUE ---
    for(int i=0; i<particles.size(); i++) {
        Particle &p = particles[i];

        // EFFET D'EXCITATION AU SURVOL
        if(isMouseOverPlan) {
            // On ajoute une force aléatoire impulsionnelle
            p.vel.x += ofRandom(-0.8, 0.8);
            p.vel.y += ofRandom(-0.8, 0.8);
            
            // Optionnel : on peut aussi faire varier la taille légèrement
            p.size = ofRandom(5, 12); 
        } else {
            // Retour à la taille normale si pas de survol
            p.size = ofMap(i, 0, particles.size(), 3, 8); 
        }

        // A. Attraction vers le centre (très douce)
        glm::vec3 dirToCenter = center - p.pos;
        float distCenter = glm::length(dirToCenter);
        if(distCenter > 0) {
            dirToCenter = glm::normalize(dirToCenter);
            p.vel += dirToCenter * 0.03f; 
        }

        // B. Répulsion entre particules
        for(int j=0; j<particles.size(); j++) {
            if(i == j) continue;
            glm::vec3 diff = p.pos - particles[j].pos;
            float d = glm::length(diff);
            if(d > 0 && d < 60) {
                diff = glm::normalize(diff);
                float repulseForce = 1.0f / d; 
                p.vel += diff * repulseForce * 2.0f; 
            }
        }

        // Friction : Moins de friction lors du survol pour garder l'agitation
        float friction = isMouseOverPlan ? 0.99f : 0.97f;
        p.vel *= friction;
        
        p.pos += p.vel;
        
        // C. Bordures (rebond)
        if(p.pos.x < 0) { p.pos.x = 0; p.vel.x *= -1; }
        if(p.pos.x > w) { p.pos.x = w; p.vel.x *= -1; }
        if(p.pos.y < 0) { p.pos.y = 0; p.vel.y *= -1; }
        if(p.pos.y > h) { p.pos.y = h; p.vel.y *= -1; }
    }

    // --- 2. RENDER DANS LE FBO ---
    planContentFbo.begin();
        // Gestion de l'opacité du fond
        // 128 = ~50% d'opacité, 255 = 100% opaque
        int alphaVal = bSolidBackground ? 128 : 0; 

        // On efface tout le contenu précédent (plus de trainées infinies)
        ofClear(255, 255, 255, alphaVal);
        
        // On dessine un rectangle de fond pour bien assurer la couleur
        bool carreOuPas = false;
        if (carreOuPas) ofSetColor(0, 0, 0, alphaVal);
        if (carreOuPas) ofDrawRectangle(0, 0, w, h);

        ofEnableAlphaBlending();
        
        // Dessin des liens
        ofSetLineWidth(1);
        for(int i=0; i<particles.size(); i++) {
            for(int j=i+1; j<particles.size(); j++) {
                float d = glm::distance(particles[i].pos, particles[j].pos);
                // On augmente un peu la distance de liaison (80) car elles sont plus écartées
                if(d < 80) { 
                    float lineAlpha = ofMap(d, 0, 80, 255, 0);
                    ofSetColor(255, 255, 255, lineAlpha);
                    ofDrawLine(particles[i].pos, particles[j].pos);
                }
            }
        }

        // Dessin des particules
        for(auto& p : particles) {
            ofSetColor(p.color);
            ofDrawCircle(p.pos, p.size);
        }
        
        // Bordure esthétique
        ofNoFill();
        ofSetColor(255, 50); // Blanc très discret
        ofDrawRectangle(0, 0, w, h);
        ofFill();
        
    planContentFbo.end();
}

//--------------------------------------------------------------
void ProjectionSystem::updateTarget(const ofCamera& viewCam, RoomWalls& walls) {
    // Inchangé
    glm::vec3 rayOrigin = viewCam.getPosition();
    glm::vec3 mouseWorld = viewCam.screenToWorld(glm::vec3(ofGetMouseX(), ofGetMouseY(), 0));
    glm::vec3 rayDir = glm::normalize(mouseWorld - rayOrigin);

    float minDistance = 1000000.0f;
    ofVec3f closestHit;
    bool foundHit = false;

    auto checkIntersection = [&](ofMesh& mesh) {
        if(mesh.getNumVertices() < 3) return;
        ofVec3f v0 = mesh.getVertex(0);
        ofVec3f v1 = mesh.getVertex(1);
        ofVec3f v2 = mesh.getVertex(2);
        
        ofVec3f planeNormal = ((v1 - v0).cross(v2 - v0)).getNormalized();
        float denom = planeNormal.dot(rayDir);
        
        if (abs(denom) > 0.0001f) { 
            float t = (v0 - (ofVec3f)rayOrigin).dot(planeNormal) / denom;
            if (t > 0 && t < minDistance) {
                ofVec3f hitP = (ofVec3f)rayOrigin + (ofVec3f)rayDir * t;
                if(hitP.distance(mesh.getCentroid()) < 2500.0f) { 
                    minDistance = t;
                    closestHit = hitP;
                    foundHit = true;
                }
            }
        }
    };

    checkIntersection(walls.meshFront);
    checkIntersection(walls.meshBack);
    checkIntersection(walls.meshCour);
    checkIntersection(walls.meshJar);
    checkIntersection(walls.meshSol);
    checkIntersection(walls.meshTopCour);
    checkIntersection(walls.meshTopJar);

    if(foundHit) {
        lastHitPoint = closestHit;
    }

    ofVec3f roomCenter(0, 600, 0);
    ofVec3f dirToCenter = (roomCenter - lastHitPoint).getNormalized();
    float distFromWall = 1200.0f;
    ofVec3f idealPos = lastHitPoint + (dirToCenter * distFromWall);

    currentSmoothedPos.interpolate(idealPos, 0.05f);

    projector.setPosition(currentSmoothedPos);
    projector.lookAt(lastHitPoint);

    currentDist = glm::distance((glm::vec3)projector.getPosition(), (glm::vec3)lastHitPoint);
}

//--------------------------------------------------------------
void ProjectionSystem::updatePlanCollePosition() {
    float rad = 1500.0f; 
    float theta = ofDegToRad(planAngle);     
    float phi   = ofDegToRad(planElevation); 
    
    float x = rad * cos(phi) * sin(theta);
    float y = rad * sin(phi);
    float z = rad * cos(phi) * cos(theta);

    planColleNode.setPosition(x, y, z);
    planColleNode.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
}

//--------------------------------------------------------------
void ProjectionSystem::drawPlanColle() {
    if(!planContentFbo.isAllocated()) return;
    
    ofEnableAlphaBlending();
    if (!bSolidBackground) glDepthMask(GL_FALSE);

    planColleNode.transformGL();
        
        ofRotateYDeg(180); 
        ofSetColor(255);
        // Le FBO contient déjà l'alpha (128 ou 255) dans ses pixels,
        // donc on le dessine en blanc plein (255) pour ne pas rajouter de transparence.
        planContentFbo.draw(-250, -275, 500, 550);
        
        if(isMouseOverPlan) {
            ofNoFill();
            ofSetLineWidth(5);
            ofSetColor(255, 0, 0);
            ofDrawRectangle(-250, -275, 0.1, 500, 550);
            
            ofVec3f worldPos = planColleNode.getGlobalPosition();
            
            float minX = -1200.0f; float maxX =  1200.0f; 
            float minZ = -1312.0f; float maxZ =  1312.0f; 
            float peakY = 1452.0f; float solY  =    0.0f; 
            
            float normX = ofMap(worldPos.x, minX, maxX, 0.0f, 1.0f);
            float normTopY = ofMap(worldPos.z, minZ, maxZ, 0.0f, 1.0f);
            float normH = ofMap(worldPos.y, peakY, solY, 0.0f, 1.0f);

            radius = normX;
            elevation = normTopY;
            azimuth = normH;

            ofSetColor(255, 255, 0); 
            ofFill();
        }
        
    planColleNode.restoreTransformGL();
    if (!bSolidBackground)    glDepthMask(GL_TRUE);
    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void ProjectionSystem::drawBeamProjection(RoomWalls& walls, bool showRoof) {
    if(!imgBeam.isAllocated()) return;
    // ... (Code identique à la version précédente) ...
    ofEnableAlphaBlending();
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0, -1.0);
    glDepthMask(GL_TRUE); 

    imgBeam.bind();
    
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.5, 0.5, 0.5); 
    glScalef(0.5, -0.5, 0.5);
    ofMatrix4x4 projMat = projector.getModelViewProjectionMatrix();
    glMultMatrixf(projMat.getPtr());

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

    glMatrixMode(GL_MODELVIEW);

    ofVec3f pos = projector.getPosition();
    ofVec3f norm = projector.getLookAtDir(); 
    double clipPlaneEq[] = { (double)norm.x, (double)norm.y, (double)norm.z, (double)(-norm.dot(pos)) };
    
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0, clipPlaneEq);

    walls.meshFront.draw();
    walls.meshBack.draw();
    walls.meshJar.draw();
    walls.meshCour.draw();
    walls.meshSol.draw();
    if(showRoof) { walls.meshTopCour.draw(); walls.meshTopJar.draw(); }

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    imgBeam.unbind();
    glDisable(GL_POLYGON_OFFSET_FILL);
}

//--------------------------------------------------------------
void ProjectionSystem::drawProjectorDebug(RoomWalls& walls) {
    // ... (Code identique à la version précédente) ...
    ofSetColor(255, 255, 0);
    projector.draw(); 
    ofPushMatrix();
    ofMultMatrix(projector.getGlobalTransformMatrix());
        ofSetColor(255, 0, 255); ofDrawBox(0, 0, 0, 100, 50, 150); 
    ofPopMatrix();
}
// ProjectionSystem.cpp


//--------------------------------------------------------------
void ProjectionSystem::checkMouseIntersection(const ofCamera& cam) {
    float w = 500.0f;
    float h = 550.0f;

    // 1. Création du Rayon (Raycasting)
    // On part de la position de la caméra
    glm::vec3 rayOrigin = cam.getPosition();
    // On trouve la direction en passant de l'écran (souris) vers le monde 3D
    glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(ofGetMouseX(), ofGetMouseY(), 0));
    glm::vec3 rayDir = glm::normalize(mouseWorld - rayOrigin);

    // 2. Passage dans l'espace LOCAL du planColle
    // C'est beaucoup plus simple de tester l'intersection si le plan est à plat (Z=0)
    ofMatrix4x4 planeTransform = planColleNode.getGlobalTransformMatrix();
    ofMatrix4x4 inverseTransform = planeTransform.getInverse();

    // On transforme l'origine du rayon et sa direction dans l'espace local du Node
    ofVec3f localRayOrigin = ofVec3f(rayOrigin) * inverseTransform;
    
    // Pour la direction, on transforme un point virtuel sur le rayon, puis on recalcul le vecteur
    ofVec3f worldPointOnRay = rayOrigin + rayDir;
    ofVec3f localPointOnRay = worldPointOnRay * inverseTransform;
    ofVec3f localRayDir = (localPointOnRay - localRayOrigin).getNormalized();

    // 3. Intersection mathématique avec le plan Z=0
    // Equation du rayon : P = Origin + t * Dir
    // On cherche quand P.z = 0  =>  Origin.z + t * Dir.z = 0  =>  t = -Origin.z / Dir.z
    
    // Si le rayon est parallèle au plan, pas de touche
    if (abs(localRayDir.z) < 1e-6) {
        isMouseOverPlan = false;
        return;
    }

    float t = -localRayOrigin.z / localRayDir.z;

    // 4. Vérification de la direction (C'est ICI qu'on corrige ton bug)
    // Si t < 0, l'intersection est DERRIÈRE la caméra -> On ignore
    if (t < 0) {
        isMouseOverPlan = false;
        return;
    }

    // 5. Calcul du point d'impact local
    ofVec3f localHit = localRayOrigin + localRayDir * t;

    // 6. Vérification des dimensions (Bounding Box)
    // Le rectangle est dessiné centré (de -w/2 à +w/2)
    // Note : Dans drawPlanColle, tu dessines : ofDrawRectangle(-250, -275, 500, 550);
    // Donc x va de -250 à 250, y va de -275 à 275.
    
    float halfW = w / 2.0f;
    float halfH = h / 2.0f;

    if (localHit.x >= -halfW && localHit.x <= halfW &&
        localHit.y >= -halfH && localHit.y <= halfH) {
        isMouseOverPlan = true;
    } else {
        isMouseOverPlan = false;
    }
}


//--------------------------------------------------------------
void ProjectionSystem::keyPressed(int key) {
    float rotSpeed = 5.0;
    if(key == OF_KEY_LEFT)  projector.panDeg(rotSpeed); 
    if(key == OF_KEY_RIGHT) projector.panDeg(-rotSpeed);
    if(key == OF_KEY_UP)    projector.tiltDeg(-rotSpeed); 
    if(key == OF_KEY_DOWN)  projector.tiltDeg(rotSpeed);
    
    // --- NOUVEAU : Touche 'o' pour l'Opacité ---
    if(key == 'c' || key == 'C') {
        bSolidBackground = !bSolidBackground;
    }

    if(key == '+') { targetBeamWidth += 25; targetBeamHeight += 25; }
    if(key == '-') { targetBeamWidth -= 25; targetBeamHeight -= 25; }
    if(key == 'w' || key == 'W') projector.rollDeg(rotSpeed);
    if(key == 'x' || key == 'X') projector.rollDeg(-rotSpeed);
}