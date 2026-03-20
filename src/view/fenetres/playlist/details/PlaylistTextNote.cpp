#include "PlaylistTextNote.h"

PlaylistTextNote::PlaylistTextNote(ofVec2f pos) {
    text = "";
    bIsEditing = false;
    textColor = ofColor::fromHsb(ofRandom(255), ofRandom(100, 200), 255);
    rect.set(pos.x, pos.y, 150, 40);
    updateSize();
}

void PlaylistTextNote::updateSize() {
    int lines = 1;
    int maxLineLen = 0;
    int curLineLen = 0;
    for(char c : text) {
        if (c == '\n') {
            lines++;
            if(curLineLen > maxLineLen) maxLineLen = curLineLen;
            curLineLen = 0;
        } else {
            curLineLen++;
        }
    }
    if(curLineLen > maxLineLen) maxLineLen = curLineLen;
    
    float scale = 1.5f;
    float minWidth = 100;
    float minHeight = 30;
    rect.width = std::max(minWidth, maxLineLen * 8.0f * scale + 20.0f);
    rect.height = std::max(minHeight, lines * 15.0f * scale + 20.0f);
}

void PlaylistTextNote::draw() {
    ofPushStyle();
    
    // Très léger fond uniquement quand on édite le texte pour savoir où on clique
    if (bIsEditing) {
        ofSetColor(255, 255, 255, 20);
        ofFill();
        ofDrawRectangle(rect);
    }
    
    ofSetColor(textColor);
    string displayText = text;
    if (bIsEditing && (ofGetFrameNum() % 60 < 30)) displayText += "_";
    else if (text.empty() && !bIsEditing) { displayText = "(Note vide)"; }
    
    ofPushMatrix();
    ofTranslate(rect.x + 10, rect.y + 22);
    ofScale(1.5f, 1.5f); // Rend le texte 50% plus gros
    ofDrawBitmapString(displayText, 0, 0);
    ofPopMatrix();
    
    ofPopStyle();
}