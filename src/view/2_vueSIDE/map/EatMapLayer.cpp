#include "EatMapLayer.h"

void EatMapLayer::setup(float simulationWidth, float simulationHeight, float displayScale) {
    simWidth = simulationWidth;
    simHeight = simulationHeight;
    scale = displayScale;
}

void EatMapLayer::loadMap(string path) {
    currentMapPath = path;
    ofImage img;
    if (img.load(path)) {
        float imgRatio = img.getWidth() / img.getHeight();
        bool isFullSize = (imgRatio < 4.0f);
        float offsetWorldY = isFullSize ? 912.0f : 0.0f;
        mapSimOffsetY = offsetWorldY / scale;
        
        float targetHeight = simWidth / imgRatio;
        img.resize(simWidth, targetHeight);
        
        img.setImageType(OF_IMAGE_COLOR_ALPHA);
        
        mapPixels = img.getPixels();
        originalPixels = mapPixels; 
        mapImage.setFromPixels(mapPixels);
        
        mapW = mapPixels.getWidth();
        mapH = mapPixels.getHeight();
        mapC = mapPixels.getNumChannels();
        bHasMap = true;
    } else {
        bHasMap = false;
        ofLogWarning("EatMapLayer") << path << " introuvable !";
    }
}

void EatMapLayer::reset() {
    if (bHasMap && originalPixels.isAllocated()) {
        mapPixels = originalPixels;
        mapImage.setFromPixels(mapPixels);
    }
}

bool EatMapLayer::isWall(float x, float y) {
    if (!bHasMap) return false;
    int ix = (int)x;
    int iy = (int)(y + mapSimOffsetY);
    
    if (iy < 0 || iy >= mapH) return false;
    if (ix >= 0 && ix < mapW) {
        ofColor c = mapPixels.getColor(ix, iy);
        if (c.getBrightness() > 128 && c.a > 128) return true;
    }
    return false;
}

void EatMapLayer::explode(float x, float y, float radius) {
    if (!bHasMap) return;
    int cx = (int)x;
    int cy = (int)(y + mapSimOffsetY);
    int r = (int)radius;
    
    bool modified = false;
    for (int iy = cy - r; iy <= cy + r; iy++) {
        for (int ix = cx - r; ix <= cx + r; ix++) {
            if (ix >= 0 && ix < mapW && iy >= 0 && iy < mapH) {
                float distSq = (ix - cx) * (ix - cx) + (iy - cy) * (iy - cy);
                if (distSq <= r * r) {
                    mapPixels.setColor(ix, iy, ofColor(0, 0, 0, 0));
                    modified = true;
                }
            }
        }
    }
    if (modified) {
        mapImage.setFromPixels(mapPixels);
    }
}

void EatMapLayer::drawBrush(float x, float y, float radius, int colorType) {
    if (!bHasMap) {
        mapW = simWidth;
        mapH = simHeight;
        mapC = 4;
        mapPixels.allocate(mapW, mapH, OF_IMAGE_COLOR_ALPHA);
        mapPixels.setColor(ofColor(0, 0, 0, 0));
        mapImage.allocate(mapW, mapH, OF_IMAGE_COLOR_ALPHA);
        originalPixels = mapPixels;
        mapSimOffsetY = 0;
        bHasMap = true;
    }
    int cx = (int)x;
    int cy = (int)(y + mapSimOffsetY);
    int r = (int)radius;
    bool modified = false;
    ofColor col = (colorType == 1) ? ofColor(255, 255, 255, 255) : ofColor(0, 0, 0, 0);
    
    for (int iy = cy - r; iy <= cy + r; iy++) {
        for (int ix = cx - r; ix <= cx + r; ix++) {
            if (ix >= 0 && ix < mapW && iy >= 0 && iy < mapH) {
                if ((ix - cx) * (ix - cx) + (iy - cy) * (iy - cy) <= r * r) {
                    mapPixels.setColor(ix, iy, col);
                    modified = true;
                }
            }
        }
    }
    if (modified) {
        mapImage.setFromPixels(mapPixels);
    }
}

void EatMapLayer::draw() {
    if (bHasMap) {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, 127);
        mapImage.draw(0, -mapSimOffsetY * scale, simWidth * scale, mapH * scale);
        ofPopStyle();
    }
}