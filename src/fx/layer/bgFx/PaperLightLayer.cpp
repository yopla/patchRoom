#include "PaperLightLayer.h"

//--------------------------------------------------------------
void PaperLight::setup(float x, float y) {
    pos.set(x, y);
    
    ofFbo::Settings settings;
    settings.width = 1000;
    settings.height = 1000;
    settings.internalformat = GL_RGBA;
    settings.useDepth = true; // IMPORTANT : gère le z-sorting en interne sans polluer la Scene2D
    fbo.allocate(settings);

    // Nettoyage immédiat pour éviter les artefacts visuels
    fbo.begin();
    ofClear(0, 0, 0, 0);
    fbo.end();
}

void PaperLight::update(float mx, float my, bool spacePressed, ofVboMesh& sharedMesh, ofTexture& tex) {
    float targetDx = 0;
    float targetDy = 0;
    
    // Si la barre d'espace n'est pas appuyée et que la souris est proche, on interagit
    if (!spacePressed && ofDist(mx, my, pos.x, pos.y) < 400) {
        targetDx = (mx - pos.x);
        targetDy = (my - pos.y);
    } else {
        // Mouvement de base idle s'il n'y a pas d'interaction
        targetDx = sin(ofGetElapsedTimef() * 1.5f + pos.x * 0.01f) * 60.0f;
        targetDy = cos(ofGetElapsedTimef() * 1.5f + pos.y * 0.01f) * 60.0f;
    }

    // Inertie similaire au script AS3
    dx += (targetDx - dx) * 0.1f;
    dy += (targetDy - dy) * 0.1f;

    fbo.begin();
    ofClear(0, 0, 0, 0); // Fond transparent pour se fondre dans la scene
    
    ofEnableDepthTest();
    glDisable(GL_CULL_FACE); // Sécurité : On affiche les deux faces pour éviter les erreurs d'orientation

    // On recrée la perspective identique à l'AS3 avec FOV 45
    ofCamera cam;
    cam.setPosition(0, 0, 800);
    cam.lookAt(ofVec3f(0, 0, 0));
    cam.setFov(45);
    cam.begin(ofRectangle(0, 0, 1000, 1000)); // Fixe le viewport sur le FBO

    ofPushMatrix();
    ofRotateXDeg(dy);
    ofRotateYDeg(dx);
    
    ofSetColor(255, 255, 255, 255); // Indispensable pour que la texture soit dessinée correctement
    tex.bind();
    sharedMesh.draw();
    tex.unbind();

    ofPopMatrix();
    
    cam.end();
    ofDisableDepthTest();
    fbo.end();
}

void PaperLight::draw() {
    ofPushStyle();
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255, 255);
    // On dessine l'autocollant 2D résultant, centré
    fbo.draw(pos.x - 500, pos.y - 500, 1000, 1000);
    ofPopStyle();
}

//--------------------------------------------------------------
void PaperLightLayer::setup(float w, float h) {
    // Création de la texture gradient (identique au script AS3)
    ofFbo::Settings texSettings;
    texSettings.width = 400;
    texSettings.height = 400;
    texSettings.internalformat = GL_RGB;
    texFbo.allocate(texSettings);

    texFbo.begin();
    ofClear(0);
    ofDisableDepthTest();
    ofSetColor(255);
    
    ofMesh grad;
    grad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    ofColor colorLeft = ofColor::white;
    ofColor colorRight = ofColor(0, 34, 68); // 0x002244

    grad.addVertex(ofVec3f(0, 0, 0));       grad.addColor(colorLeft);
    grad.addVertex(ofVec3f(400, 0, 0));     grad.addColor(colorRight);
    grad.addVertex(ofVec3f(0, 400, 0));     grad.addColor(colorLeft);
    grad.addVertex(ofVec3f(400, 400, 0));   grad.addColor(colorRight);
    grad.draw();
    texFbo.end();

    // Génération du Mesh partagé par toutes les lumières pour maximiser les performances
    sharedMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    int quadNum = 2200;
    
    for(int i = 0; i < quadNum; i++) {
        float s = (ofRandom(50) < 1.0f) ? ofRandom(2.0f, 4.0f) : ofRandom(0.1f, 2.1f);
        float mult = ofRandom(100.0f, 300.0f);
        
        float rotX = ofRandom(360.0f);
        float rotY = ofRandom(360.0f);
        float rotZ = ofRandom(360.0f);

        ofVec3f verts[4] = {
            ofVec3f(-10, -10, 0),
            ofVec3f( 10, -10, 0),
            ofVec3f(-10,  10, 0),
            ofVec3f( 10,  10, 0)
        };

        int startIdx = sharedMesh.getNumVertices();

        // Pour garantir un ordre d'application strictement identique à AS3
        for(int v = 0; v < 4; v++) {
            ofVec3f pt = verts[v];
            pt *= s;                             // 1. Scale
            pt.rotate(90, ofVec3f(0, 1, 0));     // 2. Met le quad de profil (vers l'extérieur)
            pt.x += mult;                        // 3. L'éloigne du centre
            pt.rotate(rotX, ofVec3f(1, 0, 0));   // 4. Rotations aléatoires
            pt.rotate(rotY, ofVec3f(0, 1, 0));
            pt.rotate(rotZ, ofVec3f(0, 0, 1));
            sharedMesh.addVertex(pt);
        }

        float uvBase = mult / 300.0f; // Mapping de l'UV basé sur la distance
        if (uvBase > 0.9f) uvBase = 0.9f; // Sécurité de dépassement

        sharedMesh.addTexCoord(ofVec2f(uvBase, uvBase));
        sharedMesh.addTexCoord(ofVec2f(uvBase + 0.1f, uvBase));
        sharedMesh.addTexCoord(ofVec2f(uvBase, uvBase - 0.1f));
        sharedMesh.addTexCoord(ofVec2f(uvBase + 0.1f, uvBase + 0.1f));

        sharedMesh.addIndex(startIdx + 0); sharedMesh.addIndex(startIdx + 1); sharedMesh.addIndex(startIdx + 2);
        sharedMesh.addIndex(startIdx + 1); sharedMesh.addIndex(startIdx + 3); sharedMesh.addIndex(startIdx + 2);
    }
}

void PaperLightLayer::addLight(float x, float y) {
    auto pl = make_shared<PaperLight>();
    pl->setup(x, y);
    lights.push_back(pl);
    ofLogNotice("PaperLight") << "Created PaperLight at " << x << ", " << y;
}

void PaperLightLayer::update(float mx, float my, bool spacePressed) {
    for(auto& pl : lights) {
        pl->update(mx, my, spacePressed, sharedMesh, texFbo.getTexture());
    }
}

void PaperLightLayer::draw() {
    for(auto& pl : lights) {
        pl->draw();
    }
}