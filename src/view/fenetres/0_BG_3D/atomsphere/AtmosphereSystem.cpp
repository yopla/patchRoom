#include "AtmosphereSystem.h"

void AtmosphereSystem::loadTexture(string path) {
    ofDisableArbTex(); // Indispensable pour que le mapping sphérique fonctionne bien
    
    ofFile file(path);
    string ext = ofToUpper(file.getExtension());
    bool success = false;

    if(ext == "MOV" || ext == "MP4") {
        if(video360.isLoaded()) video360.close();
        success = video360.load(path);
        if(success) {
            video360.play();
            bIsVideo = true;
        }
    } else {
        if(video360.isLoaded()) video360.stop();
        success = texture360.load(path);
        if(success) bIsVideo = false;
    }
    
    if(success) {
        //ofLog() << "Nouvelle texture 360 chargée : " << path;
        
        // On force l'affichage du mode 360 pour voir le résultat tout de suite
        bShow360 = true;
        bShowSphere = false;
        bShowDiscoBall = false;
    } else {
      //  ofLog(OF_LOG_ERROR, "Erreur chargement texture : " << path);
    }
}

//--------------------------------------------------------------
void AtmosphereSystem::setup() {
    // 1. Configuration de la sphère de base
    sphereEnvironnement.setRadius(5000);
    sphereEnvironnement.setResolution(48); // Augmenté pour l'image 8K (plus lisse)

    // 2. Génération des variantes (Texture et Couleurs par face)
    setupCheckerboard(512, 8);
    createColoredSphere();

    // 3. Génération de la Disco Ball
    createDiscoBall(8);
    
    // 4. Chargement de la texture 360
    // On désactive l'ARB pour s'assurer que les coords de texture 0-1 fonctionnent bien sur la sphère
    ofDisableArbTex(); 
    //bool success = texture360.load("Alexs_Apt_8k.jpg");
    bool success = texture360.load("VR0.jpg");
    if(success) ofLog() << "Texture 360 chargee avec succes !";
    else ofLog(OF_LOG_ERROR, "Erreur chargement Alexs_Apt_8k.jpg");

    // On aligne la sphère avec la position du Rig (0, 600, 0)
    offsetY = 600.0f;
}

//--------------------------------------------------------------
void AtmosphereSystem::update(float time) {
    if (rot) {
        autoRotY = time * 2.0f;
    }
    if(bIsVideo && video360.isLoaded()) {
        video360.update();
    }
}

//--------------------------------------------------------------
void AtmosphereSystem::draw(bool useTexture) {
    ofPushStyle();
    
    // --- 1. RENDU MODE 360 (ALEX APT) ---
if (bShow360) {
        ofSetColor(255);
        ofPushMatrix();
            ofTranslate(0, offsetY, 0);
            // 1. On se place au centre
            // 2. On applique les rotations contrôlées par le clavier
            ofRotateYDeg(rotY);
            ofRotateXDeg(rotX);
            ofRotateZDeg(rotZ);

            // 3. Si le mode auto est ON, on ajoute une rotation continue sur Y
            if (rot) {
                ofRotateYDeg(autoRotY);
            }
            
            ofScale(-1, 1, 1); 
            
            bool bVideoReady = bIsVideo && video360.isLoaded() && video360.getTexture().isAllocated();

            if(bVideoReady) {
                video360.getTexture().bind();
            } else {
                if(texture360.isAllocated()) texture360.bind();
            }
            sphereEnvironnement.draw();
            if(bVideoReady) {
                video360.getTexture().unbind();
            } else {
                if(texture360.isAllocated()) texture360.unbind();
            }
        ofPopMatrix();
        
        ofPopStyle();
        return; 
    }

    // --- 2. RENDU DE LA DISCO BALL ---
    if (bShowDiscoBall) {
        ofPushMatrix();
            ofRotateYDeg(autoRotY * 5.0f); // Rotation lente
            ofScale(-1, -1, -1); // Inverser pour voir l'intérieur
            meshDiscoBall.draw();
        ofPopMatrix();
    }

    // --- 3. RENDU DE LA SPHERE D'ENVIRONNEMENT (Damier/Couleur) ---
    if (bShowSphere) {
        ofPushMatrix();
            // Orientation vers la pointe du toit (coordonnées d'origine)
            ofVec3f axeCible(1452.0f - 1200.0f, 1472.0f - 20.0f, 0); 
            ofQuaternion inclinaison;
            inclinaison.makeRotate(ofVec3f(0, 1, 0), axeCible);
            
            float angle; ofVec3f axis;
            inclinaison.getRotate(angle, axis);
            ofRotateDeg(angle, axis.x, axis.y, axis.z);
            ofRotateYDeg(autoRotY);
            ofScale(-1, -1, -1); 

            if (useTexture) {
                // Mode A : Texture Damier
                textureDamier.bind();
                glMatrixMode(GL_TEXTURE);
                glPushMatrix();
                    glLoadIdentity(); 
                    ofScale(10, 10, 1); 
                    sphereEnvironnement.draw();
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                textureDamier.unbind();
            } else {
                // Mode B : Couleurs par face
                meshColoredSphere.draw();
            }
        ofPopMatrix();
    }
    
    ofPopStyle();
}

//--------------------------------------------------------------
void AtmosphereSystem::createDiscoBall(int res) {
    meshDiscoBall.clear();
    meshDiscoBall.setMode(OF_PRIMITIVE_TRIANGLES);

    float radius = 4000.0f;
    ofVec3f faceNormals[] = {
        ofVec3f(0,0,1), ofVec3f(0,0,-1), ofVec3f(1,0,0), 
        ofVec3f(-1,0,0), ofVec3f(0,1,0), ofVec3f(0,-1,0)
    };

    for (int i = 0; i < 6; i++) {
        ofVec3f normal = faceNormals[i];
        ofVec3f axisA = ofVec3f(normal.y, normal.z, normal.x);
        ofVec3f axisB = normal.getCrossed(axisA);

        for (int y = 0; y < res; y++) {
            for (int x = 0; x < res; x++) {
                auto project = [&](float u, float v) {
                    ofVec3f p = normal + axisA * (u * 2 - 1) + axisB * (v * 2 - 1);
                    return p.getNormalized() * radius;
                };

                ofVec3f v1 = project((float)x/res, (float)y/res);
                ofVec3f v2 = project((float)(x+1)/res, (float)y/res);
                ofVec3f v3 = project((float)(x+1)/res, (float)(y+1)/res);
                ofVec3f v4 = project((float)x/res, (float)(y+1)/res);

                ofColor color = ofColor::fromHsb(ofRandom(255), 200, 255);
                meshDiscoBall.addVertex(v1); meshDiscoBall.addColor(color);
                meshDiscoBall.addVertex(v2); meshDiscoBall.addColor(color);
                meshDiscoBall.addVertex(v3); meshDiscoBall.addColor(color);
                meshDiscoBall.addVertex(v1); meshDiscoBall.addColor(color);
                meshDiscoBall.addVertex(v3); meshDiscoBall.addColor(color);
                meshDiscoBall.addVertex(v4); meshDiscoBall.addColor(color);
            }
        }
    }
}

//--------------------------------------------------------------
void AtmosphereSystem::createColoredSphere() {
    meshColoredSphere.clear();
    meshColoredSphere.setMode(OF_PRIMITIVE_TRIANGLES);

    ofMesh originalMesh = sphereEnvironnement.getMesh();
    auto& vertices = originalMesh.getVertices();
    auto& indices = originalMesh.getIndices();

    for(size_t i = 0; i < indices.size(); i += 3) {
        ofColor faceColor = ofColor::fromHsb(ofRandom(255), 200, 255); 
        for(int j=0; j<3; j++) {
            meshColoredSphere.addVertex(vertices[indices[i+j]]);
            meshColoredSphere.addColor(faceColor);
        }
    }
}

//--------------------------------------------------------------
void AtmosphereSystem::setupCheckerboard(int size, int numChecks) {
    ofPixels pixels;
    pixels.allocate(size, size, OF_IMAGE_GRAYSCALE);
    int checkSize = size / numChecks;
    
    for(int y = 0; y < size; y++){
        for(int x = 0; x < size; x++){
            if(((x/checkSize) + (y/checkSize)) % 2 == 0) pixels.setColor(x, y, ofColor(255));
            else pixels.setColor(x, y, ofColor(50));
        }
    }
    textureDamier.allocate(pixels);
    textureDamier.setTextureWrap(GL_REPEAT, GL_REPEAT); 
    textureDamier.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
}

//--------------------------------------------------------------
void AtmosphereSystem::keyPressed(int key) {
    if(key == 'p' || key == 'P') {
        bShowSphere = !bShowSphere;
        if(bShowSphere) { bShow360 = false; bShowDiscoBall = false; }
    }
    if(key == 'm' || key == 'M') {
        bShowDiscoBall = !bShowDiscoBall;
        if(bShowDiscoBall) { bShowSphere = false; bShow360 = false; }
    }
    if(key == 'o' || key == 'O') {
        bShow360 = !bShow360;
        if(bShow360) {
            bShowSphere = false;
            bShowDiscoBall = false;
        }
    }
     if(key == 'i' || key == 'I') {
       rot= !rot;
    }
float speed = 2.0; // Vitesse de rotation par touche pressée
    if(key == OF_KEY_LEFT) {
        rotY -= speed;
    }
    if(key == OF_KEY_RIGHT) {
        rotY += speed;
    }
    if(key == OF_KEY_UP) {
        rotX -= speed;
    }
    if(key == OF_KEY_DOWN) {
        rotX += speed;
    }
    if(key == '1') {
        rotZ -= speed;
    }
    if(key == '2') {
        rotZ += speed;
    }
    if(key == '3') {
        offsetY -= speed * 10.0; // Monter
    }
    if(key == '4') {
        offsetY += speed * 10.0; // Descendre
    }
}