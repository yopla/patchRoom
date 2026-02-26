#include "BeamSystem.h"

void BeamSystem::setup(string imgName, float w, float h) {
    targetBeamWidth = w;
    targetBeamHeight = h;

    imgBeam.load(imgName);
    imgBeam.setImageType(OF_IMAGE_COLOR_ALPHA); 
    imgBeam.getTexture().bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    imgBeam.getTexture().unbind();

    lastHitPoint.set(0, 0, 0);
    currentSmoothedPos.set(0, 500, 0);

    projector.setPosition(0, 500, 0); 
    projector.lookAt(ofVec3f(0, 0, -1000));
    projector.setNearClip(10);
    projector.setFarClip(5000);
}

void BeamSystem::update() {
    float distToWall = currentDist;
    if(distToWall < 1.0f) distToWall = 1000.0f; 

    float fovRad = 2.0f * atan((targetBeamHeight / 2.0f) / distToWall);
    projector.setFov(ofRadToDeg(fovRad));
    projector.setAspectRatio(targetBeamWidth / targetBeamHeight);
}

void BeamSystem::updateTarget(const ofCamera& viewCam, RoomWalls& walls) {
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
    float distFromWall = 2500.0f;
    ofVec3f idealPos = lastHitPoint + (dirToCenter * distFromWall);

    currentSmoothedPos.interpolate(idealPos, 0.05f);

    projector.setPosition(currentSmoothedPos);
    projector.lookAt(lastHitPoint);

    currentDist = glm::distance((glm::vec3)projector.getPosition(), (glm::vec3)lastHitPoint);
}

void BeamSystem::drawProjection(RoomWalls& walls, bool showRoof, float depthBias) {
    if(!imgBeam.isAllocated()) return;
    
    ofEnableAlphaBlending();
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING); // Sécurité : on s'assure que l'éclairage n'interfère pas
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(depthBias, depthBias); // Utilisation du biais dynamique
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE); // IMPORTANT : On n'écrit pas dans le depth buffer pour éviter le Z-fighting entre beams et murs

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
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); // On réactive l'écriture depth pour le reste
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void BeamSystem::drawDebug(RoomWalls& walls) {
    ofSetColor(255, 255, 0);
    projector.draw(); 
    ofPushMatrix();
    ofMultMatrix(projector.getGlobalTransformMatrix());
        ofSetColor(255, 0, 255); ofDrawBox(0, 0, 0, 100, 50, 150); 
    ofPopMatrix();
}

void BeamSystem::keyPressed(int key) {
    float rotSpeed = 5.0;
    if(key == OF_KEY_LEFT)  projector.panDeg(rotSpeed); 
    if(key == OF_KEY_RIGHT) projector.panDeg(-rotSpeed);
    if(key == OF_KEY_UP)    projector.tiltDeg(-rotSpeed); 
    if(key == OF_KEY_DOWN)  projector.tiltDeg(rotSpeed);
    
    if(key == '+') { targetBeamWidth += 25; targetBeamHeight += 25; }
    if(key == '-') { targetBeamWidth -= 25; targetBeamHeight -= 25; }
    if(key == 'w' || key == 'W') projector.rollDeg(rotSpeed);
    if(key == 'x' || key == 'X') projector.rollDeg(-rotSpeed);
}