#include "Crayon.h"

void Crayon::setup() {
    uiRect.set(0, 0, 160, 160);
    btnMinus.set(10, 40, 40, 30);
    btnPlus.set(110, 40, 40, 30);
    btnWhite.set(10, 80, 140, 20);
    btnBlack.set(10, 105, 140, 20);
    btnEraser.set(10, 130, 140, 20);
}

void Crayon::drawUI(float x, float y) {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x, y);

    ofSetColor(40, 40, 40, 220);
    ofFill();
    ofDrawRectangle(uiRect);
    ofSetColor(200);
    ofNoFill();
    ofDrawRectangle(uiRect);

    ofSetColor(255);
    ofDrawBitmapString("OUTIL CRAYON", 30, 20);

    ofDrawBitmapString("Taille: " + ofToString((int)radius), 60, 60);
    ofSetColor(100); ofFill(); ofDrawRectangle(btnMinus); ofDrawRectangle(btnPlus);
    ofSetColor(255); ofDrawBitmapString("-", btnMinus.x + 15, btnMinus.y + 20);
    ofDrawBitmapString("+", btnPlus.x + 15, btnPlus.y + 20);

    auto drawBtn = [&](ofRectangle& r, string txt, int type) {
        if (colorType == type) ofSetColor(100, 200, 100);
        else ofSetColor(80);
        ofFill(); ofDrawRectangle(r);
        ofSetColor(255);
        ofDrawBitmapString(txt, r.x + 10, r.y + 15);
    };

    drawBtn(btnWhite, "BLANC (Ajout)", 1);
    drawBtn(btnBlack, "NOIR (Vide)", 2);
    drawBtn(btnEraser, "GOMME (Alpha 0)", 0);

    ofPopMatrix();
    ofPopStyle();
}

void Crayon::drawCursor(ofVec2f worldPos) {
    ofPushStyle();
    if (colorType == 0) ofSetColor(255, 0, 0, 150);
    else if (colorType == 1) ofSetColor(255, 255, 255, 150);
    else ofSetColor(0, 0, 0, 150);
    
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawCircle(worldPos.x, worldPos.y, radius);
    ofPopStyle();
}

bool Crayon::mousePressedScreen(float mx, float my, float uiX, float uiY) {
    ofVec2f p(mx - uiX, my - uiY);
    if (!uiRect.inside(p)) return false;

    if (btnMinus.inside(p)) { radius = max(5.0f, radius - 5.0f); return true; }
    if (btnPlus.inside(p)) { radius += 5.0f; return true; }
    if (btnWhite.inside(p)) { colorType = 1; return true; }
    if (btnBlack.inside(p)) { colorType = 2; return true; }
    if (btnEraser.inside(p)) { colorType = 0; return true; }

    return true; 
}