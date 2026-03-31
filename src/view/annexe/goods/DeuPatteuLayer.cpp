#include "DeuPatteuLayer.h"

void DeuPatteuLayer::loadFg(const string& path) {
    if (originalFgImage.load(path)) {
        originalFgImage.setImageType(OF_IMAGE_COLOR_ALPHA);
        currentFgPixels = originalFgImage.getPixels();
        fgTex.loadData(currentFgPixels);
        ofLogNotice("DeuPatteuLayer") << "Foreground charge : " << path;
    } else {
        ofLogError("DeuPatteuLayer") << "Erreur chargement FG : " << path;
    }
}

void DeuPatteuLayer::loadBg(const string& path) {
    if (originalBgImage.load(path)) {
        originalBgImage.setImageType(OF_IMAGE_COLOR_ALPHA);
        currentBgPixels = originalBgImage.getPixels();
        bgTex.loadData(currentBgPixels);
        ofLogNotice("DeuPatteuLayer") << "Background charge : " << path;
    } else {
        ofLogError("DeuPatteuLayer") << "Erreur chargement BG : " << path;
    }
}

void DeuPatteuLayer::resetFg() {
    if (originalFgImage.isAllocated()) {
        currentFgPixels = originalFgImage.getPixels();
        fgTex.loadData(currentFgPixels);
    }
}

void DeuPatteuLayer::resetBg() {
    if (originalBgImage.isAllocated()) {
        currentBgPixels = originalBgImage.getPixels();
        bgTex.loadData(currentBgPixels);
    }
}

void DeuPatteuLayer::draw(float targetW, float targetH) {
    if (!bActive) return;
    
    ofPushStyle();
    ofEnableAlphaBlending();
    ofSetColor(255);
    if (bgTex.isAllocated()) bgTex.draw(0, 0, targetW, targetH);
    if (fgTex.isAllocated()) fgTex.draw(0, 0, targetW, targetH);
    ofDisableAlphaBlending();
    ofPopStyle();
}

void DeuPatteuLayer::smudge(ofVec2f currentPos, ofVec2f lastPos, float targetW, float targetH) {
    if (!bActive) return;
    
    bool hasFg = currentFgPixels.isAllocated();
    bool hasBg = currentBgPixels.isAllocated();
    if (!hasFg && !hasBg) return;

    // On prend la dimension du FG par défaut, sinon BG
    int imgW = hasFg ? currentFgPixels.getWidth() : currentBgPixels.getWidth();
    int imgH = hasFg ? currentFgPixels.getHeight() : currentBgPixels.getHeight();

    ofVec2f currentImgPos(ofMap(currentPos.x, 0, targetW, 0, imgW), ofMap(currentPos.y, 0, targetH, 0, imgH));
    ofVec2f lastImgPos(ofMap(lastPos.x, 0, targetW, 0, imgW), ofMap(lastPos.y, 0, targetH, 0, imgH));
    ofVec2f delta = currentImgPos - lastImgPos;
    if (delta.length() < 0.1f) return;

    float imgBrushSize = brushSize * ((float)imgW / targetW);
    
    ofPixels newFgPixels = hasFg ? currentFgPixels : ofPixels();
    ofPixels newBgPixels = hasBg ? currentBgPixels : ofPixels();

    int xMin = std::max(0, (int)(currentImgPos.x - imgBrushSize));
    int xMax = std::min(imgW - 1, (int)(currentImgPos.x + imgBrushSize));
    int yMin = std::max(0, (int)(currentImgPos.y - imgBrushSize));
    int yMax = std::min(imgH - 1, (int)(currentImgPos.y + imgBrushSize));

    for (int y = yMin; y <= yMax; ++y) {
        for (int x = xMin; x <= xMax; ++x) {
            ofVec2f p(x, y);
            float d = p.distance(currentImgPos);
            if (d <= imgBrushSize) {
                float normalizedDist = d / imgBrushSize;
                float falloff = 1.0f - normalizedDist;
                if (hardness < 0.99f) falloff = pow(falloff, 1.0f + (1.0f - hardness) * 3.0f);
                else falloff = 1.0f;

                ofVec2f sourcePos = p - delta * (falloff * intensity);
                int sx = ofClamp(sourcePos.x, 0, imgW - 1);
                int sy = ofClamp(sourcePos.y, 0, imgH - 1);

                if (hasFg) {
                    ofColor c = currentFgPixels.getColor(sx, sy);
                    c.a = std::max(0.0f, c.a - (255.0f * falloff * revealSpeed)); // Révélation du calque de fond
                    newFgPixels.setColor(x, y, c);
                }
                if (hasBg) {
                    ofColor c = currentBgPixels.getColor(sx, sy);
                    newBgPixels.setColor(x, y, c);
                }
            }
        }
    }
    if (hasFg) { currentFgPixels = newFgPixels; fgTex.loadData(currentFgPixels); }
    if (hasBg) { currentBgPixels = newBgPixels; bgTex.loadData(currentBgPixels); }
}