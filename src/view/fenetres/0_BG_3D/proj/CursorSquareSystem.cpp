#include "CursorSquareSystem.h"

//--------------------------------------------------------------
void CursorSquareSystem::setup() {
    currentNormal.set(0, 0, 1);
    
    // Génération texture
    int res = 512;
    cursorImg.allocate(res, res, OF_IMAGE_COLOR_ALPHA);
    ofPixels & pix = cursorImg.getPixels();
    pix.setColor(ofColor(0, 0, 0, 0)); 
    
    int border = 40; 
    for(int y=0; y<res; y++){
        for(int x=0; x<res; x++){
            if(x < border || x > res-border || y < border || y > res-border){
                pix.setColor(x,y, ofColor(255, 0, 0, 255)); 
            } 
        }
    }
    cursorImg.update();
    
    cursorImg.getTexture().bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f }; 
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    cursorImg.getTexture().unbind();
    
    projector.setNearClip(10);
    projector.setFarClip(5000);
}

//--------------------------------------------------------------
void CursorSquareSystem::updateRaycast(ofCamera& cam, RoomWalls& walls) {
    glm::vec3 rayOrigin = cam.getPosition();
    glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(ofGetMouseX(), ofGetMouseY(), 0));
    glm::vec3 rayDir = glm::normalize(mouseWorld - rayOrigin);

    float minDistance = 1000000.0f;
    isVisible = false;
    hitType = HIT_NONE; 

    auto checkIntersection = [&](ofMesh& mesh, CursorHitType type, ofVec3f forcedNormal = ofVec3f(0,0,0)) {
        if(mesh.getNumVertices() < 3) return;
        ofVec3f v0 = mesh.getVertex(0);
        ofVec3f v1 = mesh.getVertex(1);
        ofVec3f v2 = mesh.getVertex(2);
        ofVec3f geoNormal = ((v1 - v0).cross(v2 - v0)).getNormalized();
        float denom = geoNormal.dot(rayDir);
        
        if (abs(denom) > 0.0001f) { 
            float t = (v0 - ofVec3f(rayOrigin)).dot(geoNormal) / denom;
            if (t > 0 && t < minDistance) {
                ofVec3f hitP = (ofVec3f)rayOrigin + (ofVec3f)rayDir * t;
                if(hitP.distance(mesh.getCentroid()) < 2500.0f) {
                    minDistance = t;
                    currentPos = hitP;
                    hitType = type;
                    
                    if(forcedNormal.lengthSquared() > 0.1) currentNormal = forcedNormal;
                    else {
                        currentNormal = geoNormal;
                        if(currentNormal.y < 0) currentNormal = -currentNormal;
                    }
                    isVisible = true;
                }
            }
        }
    };

    // --- MISE A JOUR DES TYPES PRECIS ---
    checkIntersection(walls.meshFront, HIT_WALL_FRONT, ofVec3f(0, 0, 1));
    checkIntersection(walls.meshBack,  HIT_WALL_BACK,  ofVec3f(0, 0, -1));
    checkIntersection(walls.meshCour,  HIT_WALL_COUR,  ofVec3f(-1, 0, 0));
    checkIntersection(walls.meshJar,   HIT_WALL_JAR,   ofVec3f(1, 0, 0));
    checkIntersection(walls.meshSol,   HIT_FLOOR,      ofVec3f(0, 1, 0));
    
    // Pour les toits, on laisse le calcul auto de la normale (0,0,0)
    checkIntersection(walls.meshTopCour, HIT_ROOF_COUR, ofVec3f(0, 0, 0));
    checkIntersection(walls.meshTopJar,  HIT_ROOF_JAR,  ofVec3f(0, 0, 0));
}


//--------------------------------------------------------------
void CursorSquareSystem::drawProjected(RoomWalls& walls) {
    if(!isVisible) return;

    // --- SETUP PROJECTEUR AVEC BIAIS ---
    float distToWall = 1000.0f;
    ofVec3f offset = currentNormal * distToWall;

    // [CORRECTION RACCORD TOIT <-> FRONT/BACK]
    // Si on est sur un toit, on ajoute un décalage sur l'axe Z.
    // Cela permet au projecteur de ne pas être rasant (parallèle) au mur Front/Back.
    // Plus on est proche du bord Z (Front ou Back), plus on décale le projecteur.
    if(hitType == HIT_ROOF_COUR || hitType == HIT_ROOF_JAR) {
        // On ajoute 40% de la position Z actuelle au décalage du projecteur.
        // Si Z est négatif (Front), le projecteur recule encore plus en négatif -> Il voit le mur Front.
        float zBias = currentPos.z * 0.4f; 
        offset.z += zBias;
    }

    projector.setPosition(currentPos + offset);
    
    // Calcul de l'axe UP (inchangé)
    ofVec3f upAxis = (abs(currentNormal.y) > 0.9) ? ofVec3f(0,0,1) : ofVec3f(0,1,0);
    projector.lookAt(currentPos, upAxis);

    // Calcul FOV (inchangé)
    float adjustedDist = offset.length(); // La distance a changé à cause du biais
    float fovRad = 2.0f * atan((squareSize / 2.0f) / adjustedDist);
    projector.setFov(ofRadToDeg(fovRad));
    projector.setAspectRatio(1.0f);

    ofEnableAlphaBlending();
    
    // CORRECTION : On désactive l'écriture dans le Z-Buffer.
    // Sinon, le carré (qui est une texture majoritairement transparente) écrit sa profondeur
    // et masque les objets situés derrière le mur survolé.
    glDepthMask(GL_FALSE);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-10.0, -10.0); 

    cursorImg.getTexture().bind();

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.5, 0.5, 0.5); 
    glScalef(0.5, 0.5, 0.5);
    ofMultMatrix(projector.getModelViewProjectionMatrix());

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    float plane[] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    glTexGenfv(GL_S, GL_OBJECT_PLANE, &plane[0]);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, &plane[4]);
    glTexGenfv(GL_R, GL_OBJECT_PLANE, &plane[8]);
    glTexGenfv(GL_Q, GL_OBJECT_PLANE, &plane[12]);

    glMatrixMode(GL_MODELVIEW);

    // --- CLIP PLANE ---
    ofVec3f pos = projector.getPosition();
    ofVec3f norm = projector.getLookAtDir(); 
    norm.normalize();
    double clipPlaneEq[] = { (double)norm.x, (double)norm.y, (double)norm.z, (double)(-norm.dot(pos)) };
   glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0, clipPlaneEq);

    // --- GESTION ALPHA ---
    ofPushStyle();
    if (bLowAlpha) { // Note: bLowAlpha est une variable membre publique (bool) à ajouter à la classe.
        ofSetColor(255, 255, 255, 0.0); // 10% alpha
    } else {
        ofSetColor(255);
    }

    // --- LOGIQUE D'AFFICHAGE ---
    if (bDrawReflections) {
        // === MODE "REFLETS" (ANCIEN COMPORTEMENT) ===
        // Le carré peut déborder sur les murs voisins et le sol
        
        bool bDrawSol     = true;
        bool bDrawEnds    = true; 
        bool bDrawCourGrp = true; 
        bool bDrawJarGrp  = true; 

        if(hitType == HIT_WALL_COUR) bDrawJarGrp = false;
        else if(hitType == HIT_WALL_JAR) bDrawCourGrp = false;
        else if(hitType == HIT_ROOF_COUR || hitType == HIT_ROOF_JAR) bDrawSol = false; 
        
        if(bDrawSol) walls.meshSol.draw();
        if(bDrawEnds) {
            walls.meshFront.draw();
            walls.meshBack.draw();
        }
        if(bDrawCourGrp) {
            walls.meshCour.draw();
            if(hitType != HIT_FLOOR) walls.meshTopCour.draw(); 
        }
        if(bDrawJarGrp) {
            walls.meshJar.draw();
            if(hitType != HIT_FLOOR) walls.meshTopJar.draw();
        }

    } else {
        // === MODE STRICT (NOUVEAU PAR DÉFAUT) ===
        // On ne dessine QUE la surface exactement survolée.
        // Aucun débordement sur le sol ou les coins.
        
        switch(hitType) {
            case HIT_FLOOR:      walls.meshSol.draw();     break;
            case HIT_WALL_FRONT: walls.meshFront.draw();   break;
            case HIT_WALL_BACK:  walls.meshBack.draw();    break;
            case HIT_WALL_COUR:  walls.meshCour.draw();    break;
            case HIT_WALL_JAR:   walls.meshJar.draw();     break;
            case HIT_ROOF_COUR:  walls.meshTopCour.draw(); break;
            case HIT_ROOF_JAR:   walls.meshTopJar.draw();  break;
            default: break;
        }
    }
    
    ofPopStyle();

    glDisable(GL_CLIP_PLANE0);
    
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    cursorImg.getTexture().unbind();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_TRUE); // On réactive l'écriture Z-Buffer pour la suite
    ofDisableAlphaBlending();
}