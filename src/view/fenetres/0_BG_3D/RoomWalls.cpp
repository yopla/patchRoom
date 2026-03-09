#include "RoomWalls.h"

void RoomWalls::setup() {
    imgFront.load("GAB2/FRONT.png"); 
    imgBack.load("GAB2/BACK.png");
    imgCour.load("GAB2/COUR.png");   
    imgJar.load("GAB2/JAR.png");
    imgSol.load("GAB2/SOL.png");     
    imgTopCour.load("GAB2/TOP_COUR.png"); 
    imgTopJar.load("GAB2/TOP_JAR.png");

    float w2 = roomWidth / 2.0f;
    float d2 = roomDepth / 2.0f;

    // --- 1. MURS VERTICAUX (Meshes) ---
    
    // FRONT
    meshFront.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshFront.addVertex(ofVec3f(-w2, heightFrontBack, -d2)); meshFront.addTexCoord(ofVec2f(0, 0));
    meshFront.addVertex(ofVec3f(w2, heightFrontBack, -d2));  meshFront.addTexCoord(ofVec2f(1, 0));
    meshFront.addVertex(ofVec3f(w2, 0, -d2));               meshFront.addTexCoord(ofVec2f(1, 1));
    meshFront.addVertex(ofVec3f(-w2, 0, -d2));              meshFront.addTexCoord(ofVec2f(0, 1));

    // BACK
    meshBack.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshBack.addVertex(ofVec3f(w2, heightFrontBack, d2));  meshBack.addTexCoord(ofVec2f(0, 0));
    meshBack.addVertex(ofVec3f(-w2, heightFrontBack, d2)); meshBack.addTexCoord(ofVec2f(1, 0));
    meshBack.addVertex(ofVec3f(-w2, 0, d2));              meshBack.addTexCoord(ofVec2f(1, 1));
    meshBack.addVertex(ofVec3f(w2, 0, d2));               meshBack.addTexCoord(ofVec2f(0, 1));

    // JAR (Mur Gauche)
    meshJar.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshJar.addVertex(ofVec3f(-w2, heightJar, d2));  meshJar.addTexCoord(ofVec2f(0, 0));
    meshJar.addVertex(ofVec3f(-w2, heightJar, -d2)); meshJar.addTexCoord(ofVec2f(1, 0));
    meshJar.addVertex(ofVec3f(-w2, 0, -d2));         meshJar.addTexCoord(ofVec2f(1, 1));
    meshJar.addVertex(ofVec3f(-w2, 0, d2));          meshJar.addTexCoord(ofVec2f(0, 1));

    // COUR (Mur Droit)
    meshCour.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshCour.addVertex(ofVec3f(w2, heightCour, -d2)); meshCour.addTexCoord(ofVec2f(0, 0));
    meshCour.addVertex(ofVec3f(w2, heightCour, d2));  meshCour.addTexCoord(ofVec2f(1, 0));
    meshCour.addVertex(ofVec3f(w2, 0, d2));           meshCour.addTexCoord(ofVec2f(1, 1));
    meshCour.addVertex(ofVec3f(w2, 0, -d2));          meshCour.addTexCoord(ofVec2f(0, 1));

    // --- 2. SOL & TOITS (Meshes existants) ---

    float zStartSol = -d2;              
    float zEndSol   = -d2 + roomSolDepth; 
    meshSol.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshSol.addVertex(ofVec3f(-w2, 0, zStartSol)); meshSol.addTexCoord(ofVec2f(0, 0));
    meshSol.addVertex(ofVec3f(w2, 0, zStartSol));  meshSol.addTexCoord(ofVec2f(1, 0));
    meshSol.addVertex(ofVec3f(w2, 0, zEndSol));    meshSol.addTexCoord(ofVec2f(1, 1));
    meshSol.addVertex(ofVec3f(-w2, 0, zEndSol));   meshSol.addTexCoord(ofVec2f(0, 1));

    peakX = 1452.0f - 1200.0f; peakY = heightFrontBack - 20.0f; 
    ofVec3f peakFront(peakX, peakY, -d2), peakBack(peakX, peakY, d2);
    ofVec3f wCF(w2, heightCour, -d2), wCB(w2, heightCour, d2);
    ofVec3f wJF(-w2, heightJar, -d2), wJB(-w2, heightJar, d2);

    meshTopCour.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshTopCour.addVertex(peakFront); meshTopCour.addTexCoord(ofVec2f(0, 0)); 
    meshTopCour.addVertex(peakBack);  meshTopCour.addTexCoord(ofVec2f(1, 0)); 
    meshTopCour.addVertex(wCB);       meshTopCour.addTexCoord(ofVec2f(1, 1)); 
    meshTopCour.addVertex(wCF);       meshTopCour.addTexCoord(ofVec2f(0, 1)); 

    meshTopJar.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    meshTopJar.addVertex(peakFront); meshTopJar.addTexCoord(ofVec2f(1, 0)); 
    meshTopJar.addVertex(peakBack);  meshTopJar.addTexCoord(ofVec2f(0, 0)); 
    meshTopJar.addVertex(wJB);       meshTopJar.addTexCoord(ofVec2f(0, 1)); 
    meshTopJar.addVertex(wJF);       meshTopJar.addTexCoord(ofVec2f(1, 1)); 

    // --- 3. POINTS DE RÉFÉRENCE ---
    pFront = { ofVec3f(-w2, heightFrontBack, -d2), ofVec3f(-w2, 0, -d2), ofVec3f(w2, 0, -d2) };
    pBack  = { ofVec3f(w2, heightFrontBack, d2), ofVec3f(w2, 0, d2), ofVec3f(-w2, 0, d2) };
    pJar   = { ofVec3f(-w2, heightJar, d2), ofVec3f(-w2, 0, d2), ofVec3f(-w2, 0, -d2) };
    pCour  = { ofVec3f(w2, heightCour, -d2), ofVec3f(w2, 0, -d2), ofVec3f(w2, 0, d2) };
    pSolTL = ofVec3f(-w2, 0, zStartSol); pSolBL = ofVec3f(-w2, 0, zEndSol); pSolBR = ofVec3f(w2, 0, zEndSol);
    pTopCourTL = peakFront; pTopCourBL = wCF; pTopCourBR = wCB;
    pTopJarTL = peakBack; pTopJarBL = wJB; pTopJarBR = wJF;
}

void RoomWalls::draw(bool showRoof, float alpha) {
    ofPushStyle();
    ofEnableAlphaBlending(); 
    ofSetColor(255, 255, 255, alpha);

    // CORRECTION: Pour que la transparence fonctionne, on ne doit pas écrire dans le
    // buffer de profondeur (z-buffer) si les murs sont transparents.
    // Sinon, ils masquent les objets qui se trouvent derrière eux.
    if (alpha < 255.0f) {
        glDepthMask(GL_FALSE);
    }

    imgFront.bind(); meshFront.draw(); imgFront.unbind();
    imgBack.bind();  meshBack.draw();  imgBack.unbind();
    imgJar.bind();   meshJar.draw();   imgJar.unbind();
    imgCour.bind();  meshCour.draw();  imgCour.unbind();
    imgSol.bind();   meshSol.draw();   imgSol.unbind();

    if(showRoof) {
        imgTopCour.bind(); meshTopCour.draw(); imgTopCour.unbind(); 
        imgTopJar.bind();  meshTopJar.draw();  imgTopJar.unbind(); 
    }

    // On restaure l'état par défaut pour le reste du rendu.
    glDepthMask(GL_TRUE);
    ofPopStyle();
}

//--------------------------------------------------------------
// Algorithme d'intersection Rayon-Triangle (Möller–Trumbore)
bool RoomWalls::rayTriangleIntersect(const ofVec3f &orig, const ofVec3f &dir,
                                     const ofVec3f &v0, const ofVec3f &v1, const ofVec3f &v2,
                                     float &t, float &u, float &v) {
    ofVec3f v0v1 = v1 - v0;
    ofVec3f v0v2 = v2 - v0;
    ofVec3f pvec = dir.getCrossed(v0v2);
    float det = v0v1.dot(pvec);

    // Si le déterminant est proche de 0, le rayon est parallèle au triangle
    if (fabs(det) < 0.00001) return false;

    float invDet = 1.0 / det;
    ofVec3f tvec = orig - v0;
    u = tvec.dot(pvec) * invDet;
    if (u < 0 || u > 1) return false;

    ofVec3f qvec = tvec.getCrossed(v0v1);
    v = dir.dot(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = v0v2.dot(qvec) * invDet;
    return true;
}

//--------------------------------------------------------------
ofColor RoomWalls::getPixelFromRay(const ofVec3f& origin, const ofVec3f& dir) {
    float minT = 100000.0f; // Distance infinie
    ofColor finalColor(0, 0, 0, 0); // Transparent par défaut
    bool hit = false;

    // Liste des meshes et textures associées à tester
    struct WallObj { ofMesh* m; ofImage* i; };
    vector<WallObj> walls = {
        {&meshFront, &imgFront}, {&meshBack, &imgBack},
        {&meshCour, &imgCour}, {&meshJar, &imgJar},
        {&meshSol, &imgSol}, {&meshTopCour, &imgTopCour},
        {&meshTopJar, &imgTopJar}
    };

    for(auto& w : walls) {
        if(w.m->getNumVertices() < 3) continue;

        // On assume que les meshes sont des Triangle Fans (Rectangles = 2 triangles : 0-1-2 et 0-2-3)
        // ou simplement une liste de triangles.
        // Pour un FAN à 4 sommets (0,1,2,3) -> Triangles (0,1,2) et (0,2,3)
        
        int numTris = w.m->getNumVertices() - 2; // Pour un FAN
        
        for(int i = 0; i < numTris; i++) {
            int idx0 = 0;
            int idx1 = i + 1;
            int idx2 = i + 2;

            ofVec3f v0 = w.m->getVertex(idx0);
            ofVec3f v1 = w.m->getVertex(idx1);
            ofVec3f v2 = w.m->getVertex(idx2);

            float t, u, v;
            if(rayTriangleIntersect(origin, dir, v0, v1, v2, t, u, v)) {
                if(t > 0 && t < minT) {
                    minT = t;
                    hit = true;

                    // Interpolation des coordonnées de texture (Barycentric)
                    // UV = w*uv0 + u*uv1 + v*uv2, avec w = 1-u-v
                    ofVec2f uv0 = w.m->getTexCoord(idx0);
                    ofVec2f uv1 = w.m->getTexCoord(idx1);
                    ofVec2f uv2 = w.m->getTexCoord(idx2);

                    float w_bary = 1.0f - u - v;
                    ofVec2f texCoord = uv0 * w_bary + uv1 * u + uv2 * v;

                    // Sampling de la texture
                    // On s'assure que les coords sont entre 0 et 1
                    float tx = ofClamp(texCoord.x, 0.0f, 1.0f);
                    float ty = ofClamp(texCoord.y, 0.0f, 1.0f);
                    
                    // Conversion en pixels
                    float px = tx * w.i->getWidth();
                    float py = ty * w.i->getHeight();
                    
                    // On récupère la couleur (getColor fait le clamp interne)
                    finalColor = w.i->getColor(px, py);
                }
            }
        }
    }

    return finalColor;
}