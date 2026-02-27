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
    ofEnableAlphaBlending(); 
    ofSetColor(255, 255, 255, alpha);
    glDepthMask(GL_TRUE); 

    imgFront.bind(); meshFront.draw(); imgFront.unbind();
    imgBack.bind();  meshBack.draw();  imgBack.unbind();
    imgJar.bind();   meshJar.draw();   imgJar.unbind();
    imgCour.bind();  meshCour.draw();  imgCour.unbind();
    imgSol.bind();   meshSol.draw();   imgSol.unbind();

    if(showRoof) {
        imgTopCour.bind(); meshTopCour.draw(); imgTopCour.unbind(); 
        imgTopJar.bind();  meshTopJar.draw();  imgTopJar.unbind(); 
    }

    ofSetColor(255);
    ofDisableAlphaBlending();
    glDepthMask(GL_FALSE);
}