#include "VolumetricLayerManager.h"

void VolumetricLayerManager::draw(float targetWidth, float targetHeight) {
    if (bLayerVolumActive && layerVolumImg.isAllocated()) {
        if (!bDepthMapActive && depthMapImg.isAllocated() && volumetricMesh.getNumVertices() > 0) {
            ofPushStyle();
            ofEnableDepthTest();
            
            ofPushMatrix();
            ofTranslate(targetWidth / 2.0f, targetHeight / 2.0f, 0);
            ofRotateXDeg(rotX);
            ofRotateYDeg(rotY);
            ofTranslate(-targetWidth / 2.0f, -targetHeight / 2.0f, 0);
            
            ofSetColor(255);
            layerVolumImg.bind();
            volumetricMesh.draw();
            layerVolumImg.unbind();
            
            ofPopMatrix();
            ofDisableDepthTest();
            ofPopStyle();
        } else {
            ofSetColor(255);
            layerVolumImg.draw(0, 0, targetWidth, targetHeight);
        }
    }
    
    if (bDepthMapActive && depthMapImg.isAllocated()) {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, 112); // ~44% alpha
        depthMapImg.draw(0, 0, targetWidth, targetHeight);
        ofDisableAlphaBlending();
        ofPopStyle();
    }
}

void VolumetricLayerManager::buildMesh(float targetWidth, float targetHeight) {
    volumetricMesh.clear();
    if (!layerVolumImg.isAllocated() || !depthMapImg.isAllocated()) return;

    volumetricMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    int w = layerVolumImg.getWidth();
    int h = layerVolumImg.getHeight();
    int resX = 250; 
    int resY = (h * resX) / w; 

    ofImage scaledDepth = depthMapImg;
    scaledDepth.resize(resX, resY);

    for (int y = 0; y < resY; y++) {
        for (int x = 0; x < resX; x++) {
            float px = ofMap(x, 0, resX - 1, 0, targetWidth);
            float py = ofMap(y, 0, resY - 1, 0, targetHeight);
            float z = (scaledDepth.getColor(x, y).getBrightness() / 255.0f) * extrusionIntensity;
            volumetricMesh.addVertex(ofVec3f(px, py, z));
            
            float tx = ofMap(x, 0, resX - 1, 0.0f, 1.0f);
            float ty = ofMap(y, 0, resY - 1, 0.0f, 1.0f);
            volumetricMesh.addTexCoord(layerVolumImg.getTexture().getCoordFromPercent(tx, ty));
        }
    }

    for (int y = 0; y < resY - 1; y++) {
        for (int x = 0; x < resX - 1; x++) {
            int i1 = x + y * resX;
            int i2 = (x + 1) + y * resX;
            int i3 = x + (y + 1) * resX;
            int i4 = (x + 1) + (y + 1) * resX;
            volumetricMesh.addIndex(i1); volumetricMesh.addIndex(i2); volumetricMesh.addIndex(i3);
            volumetricMesh.addIndex(i2); volumetricMesh.addIndex(i4); volumetricMesh.addIndex(i3);
        }
    }
}

string VolumetricLayerManager::loadDepthMap(const string& path, const ofImage& fallbackImg, float targetWidth, float targetHeight) {
    if (depthMapImg.load(path)) {
        string warning = "";
        if (!layerVolumImg.isAllocated()) {
            warning = "ATTENTION: Pas de LayerVolum, utilisation de l'image de base";
            if (fallbackImg.isAllocated()) layerVolumImg = fallbackImg;
        }
        buildMesh(targetWidth, targetHeight);
        ofLogNotice("VolumetricLayerManager") << "Depth Map chargee : " << path;
        return warning;
    }
    return "";
}

string VolumetricLayerManager::loadLayerVolum(const string& path, const ofImage& fallbackImg, float targetWidth, float targetHeight) {
    if (layerVolumImg.load(path)) {
        if (depthMapImg.isAllocated()) buildMesh(targetWidth, targetHeight);
        ofLogNotice("VolumetricLayerManager") << "LayerVolum charge : " << path;
    }
    return "";
}

void VolumetricLayerManager::reset() {
    rotX = 0;
    rotY = 0;
    depthMapImg.clear();
    volumetricMesh.clear();
}