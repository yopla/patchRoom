#include "ViewLayerManager.h"

void ViewLayerManager::addLayer(string name, float srcX, float srcY, float w, float h, float dstX, float dstY, bool center) {
    CropLayer l;
    l.sourceRect.set(srcX, srcY, w, h);
    l.destPos.set(dstX, dstY);
    l.useCenterAnchor = center;
    l.name = name;
    layers.push_back(l);
}

void ViewLayerManager::draw(ofTexture& tex) {
    for(int i = 0; i < layers.size(); i++) {
        CropLayer & l = layers[i];
        ofPushMatrix();
        ofTranslate(l.destPos.x, l.destPos.y);
        ofRotateDeg(l.rotation);
        ofScale(l.scale, l.scale);

        float drawX = l.useCenterAnchor ? -l.sourceRect.width / 2.0f : 0;
        float drawY = l.useCenterAnchor ? -l.sourceRect.height / 2.0f : 0;

        tex.drawSubsection(drawX, drawY, l.sourceRect.width, l.sourceRect.height,
                           l.sourceRect.x, l.sourceRect.y, l.sourceRect.width, l.sourceRect.height);

        // Feedback de sélection
        if(i == selectedIndex) {
            ofNoFill(); ofSetColor(255, 0, 0);
            ofDrawRectangle(drawX, drawY, l.sourceRect.width, l.sourceRect.height);
            ofSetColor(255);
            ofDrawBitmapStringHighlight(l.name + " (Selected)", drawX, drawY - 10);
        }
        ofPopMatrix();
    }
}

void ViewLayerManager::keyPressed(int key) {
    if(layers.empty()) return;
    if(key == OF_KEY_TAB) {
        selectedIndex = (selectedIndex + 1) % layers.size();
        return;
    }

    
    CropLayer & l = layers[selectedIndex];
    float speed = ofGetKeyPressed(OF_KEY_SHIFT) ? 20.0 : 5.0;

    if(ofGetKeyPressed(OF_KEY_SHIFT)) { // Mode Source
        if(key == OF_KEY_LEFT)  l.sourceRect.x -= speed;
        if(key == OF_KEY_RIGHT) l.sourceRect.x += speed;
        if(key == OF_KEY_UP)    l.sourceRect.y -= speed;
        if(key == OF_KEY_DOWN)  l.sourceRect.y += speed;
    } else { // Mode Destination
        if(key == OF_KEY_LEFT)  l.destPos.x -= speed;
        if(key == OF_KEY_RIGHT) l.destPos.x += speed;
        if(key == OF_KEY_UP)    l.destPos.y -= speed;
        if(key == OF_KEY_DOWN)  l.destPos.y += speed;
    }

    if(key == 'z') l.scale += 0.02;
    if(key == 's') l.scale -= 0.02;
    if(key == 'q') l.rotation -= 1.0;
    if(key == 'd') l.rotation += 1.0;
    if(key == 'c') l.useCenterAnchor = !l.useCenterAnchor;
}