#include "PatteuLayer.h"

void PatteuLayer::load(const string& path) {
    if (originalImage.load(path)) {
        // Assurer qu'on est en RGBA pour éviter les problèmes de canaux
        originalImage.setImageType(OF_IMAGE_COLOR_ALPHA);
        currentPixels = originalImage.getPixels();
        tex.loadData(currentPixels);
        ofLogNotice("PatteuLayer") << "Image chargee avec succes : " << path;
    } else {
        ofLogError("PatteuLayer") << "Erreur de chargement : " << path;
    }
}

void PatteuLayer::reset() {
    if (originalImage.isAllocated()) {
        currentPixels = originalImage.getPixels();
        tex.loadData(currentPixels);
    }
}

void PatteuLayer::draw(float targetW, float targetH) {
    if (bActive && tex.isAllocated()) {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255);
        tex.draw(0, 0, targetW, targetH);
        ofDisableAlphaBlending();
        ofPopStyle();
    }
}

void PatteuLayer::smudge(ofVec2f currentPos, ofVec2f lastPos, float targetW, float targetH) {
    if (!bActive || !currentPixels.isAllocated()) return;

    int imgW = currentPixels.getWidth();
    int imgH = currentPixels.getHeight();

    // Convertir de l'espace cible (target) vers l'espace de l'image réelle
    ofVec2f currentImgPos(ofMap(currentPos.x, 0, targetW, 0, imgW), ofMap(currentPos.y, 0, targetH, 0, imgH));
    ofVec2f lastImgPos(ofMap(lastPos.x, 0, targetW, 0, imgW), ofMap(lastPos.y, 0, targetH, 0, imgH));
    
    ofVec2f delta = currentImgPos - lastImgPos;
    if (delta.length() < 0.1f) return;

    float imgBrushSize = brushSize * ((float)imgW / targetW);
    ofPixels newPixels = currentPixels; // Buffer temporaire pour eviter la propagation infinie durant la meme passe

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
                else falloff = 1.0f; // Bords totalement nets

                ofVec2f sourcePos = p - delta * (falloff * intensity);
                ofColor c = currentPixels.getColor(ofClamp(sourcePos.x, 0, imgW - 1), ofClamp(sourcePos.y, 0, imgH - 1));
                newPixels.setColor(x, y, c);
            }
        }
    }
    currentPixels = newPixels;
    tex.loadData(currentPixels);
}