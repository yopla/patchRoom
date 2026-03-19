#include "PlaylistVisualFrame.h"

PlaylistVisualFrame::PlaylistVisualFrame(ofVec2f pos) {
    rect.set(pos.x - 150, pos.y - 100, 300, 200); // Taille par défaut
}

void PlaylistVisualFrame::draw(bool bIsSelected) {
    ofPushStyle();
    
    ofNoFill();
    ofSetLineWidth(bIsSelected ? 3 : 2);
    ofSetColor(bIsSelected ? ofColor(255, 200, 0) : ofColor(255, 255, 255, 120));
    ofDrawRectangle(rect);

    // Poignée de redimensionnement en bas à droite
    if (bIsSelected) {
        ofFill();
        ofSetColor(255, 200, 0);
        ofDrawRectangle(rect.getRight() - 10, rect.getBottom() - 10, 10, 10);
    }
    
    ofPopStyle();
}

bool PlaylistVisualFrame::isEdgeHit(ofVec2f pos, float tolerance) {
    ofRectangle outer(rect.x - tolerance, rect.y - tolerance, rect.width + 2*tolerance, rect.height + 2*tolerance);
    ofRectangle inner(rect.x + tolerance, rect.y + tolerance, rect.width - 2*tolerance, rect.height - 2*tolerance);
    return outer.inside(pos) && !inner.inside(pos);
}

bool PlaylistVisualFrame::isResizeHit(ofVec2f pos, float tolerance) {
    ofRectangle resizeHandle(rect.getRight() - 10 - tolerance, rect.getBottom() - 10 - tolerance, 10 + 2*tolerance, 10 + 2*tolerance);
    return resizeHandle.inside(pos);
}