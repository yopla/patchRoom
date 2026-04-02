#include "PlaylistHistoryUI.h"

void PlaylistHistoryUI::setup() {
    boxRect.set(ofGetWidth() - 320, 50, 300, 400); 
}

void PlaylistHistoryUI::addEvent(const std::string& desc, ofVec2f pan, float zoom) {
    HistoryEvent ev;
    ev.description = desc;
    ev.pan = pan;
    ev.zoom = zoom;
    events.push_front(ev); 
    if (events.size() > maxEvents) {
        events.pop_back();
    }
}

void PlaylistHistoryUI::toggle(ofVec2f currentPan, float currentZoom) {
    bIsVisible = !bIsVisible;
    if (bIsVisible) {
        originalPan = currentPan;
        originalZoom = currentZoom;
        bIsHoveringEvent = false;
        hoveredIndex = -1;
        scrollY = 0;
    }
}

void PlaylistHistoryUI::close() {
    bIsVisible = false;
}

void PlaylistHistoryUI::draw() {
    if (!bIsVisible) return;
    
    boxRect.x = ofGetWidth() - boxRect.width - 20; 
    boxRect.height = std::max(100.0f, std::min(400.0f, (float)ofGetHeight() - 100.0f));
    
    ofPushStyle();
    
    ofSetColor(30, 30, 30, 240);
    ofFill();
    ofDrawRectangle(boxRect);
    ofSetColor(200);
    ofNoFill();
    ofDrawRectangle(boxRect);
    
    ofSetColor(255);
    ofDrawBitmapStringHighlight("Historique des actions", boxRect.x + 10, boxRect.y + 20, ofColor(0,0,0,0), ofColor(255, 200, 0));
    
    float contentHeight = events.size() * itemHeight;
    float viewHeight = boxRect.height - 30; 
    maxScroll = std::max(0.0f, contentHeight - viewHeight);
    scrollY = ofClamp(scrollY, 0, maxScroll);
    
    float startY = boxRect.y + 30;
    
    int startIndex = scrollY / itemHeight;
    int numVisible = ceil(viewHeight / itemHeight) + 1;
    
    for (int i = startIndex; i < startIndex + numVisible && i < events.size(); ++i) {
        float itemY = startY + i * itemHeight - scrollY;
        if (itemY < startY || itemY > boxRect.getBottom() - itemHeight + 5) continue;
        
        ofRectangle itemRect(boxRect.x, itemY, boxRect.width, itemHeight);
        
        if (i == hoveredIndex) {
            ofSetColor(80, 120, 200, 200);
            ofFill();
            ofDrawRectangle(itemRect);
        }
        
        ofSetColor(255);
        std::string text = ofToString(events.size() - i) + ". " + events[i].description;
        ofDrawBitmapString(text, itemRect.x + 10, itemRect.y + 17);
    }
    
    if (maxScroll > 0) {
        ofSetColor(100);
        ofFill();
        float sbHeight = (viewHeight / contentHeight) * viewHeight;
        float sbY = startY + (scrollY / maxScroll) * (viewHeight - sbHeight);
        ofDrawRectangle(boxRect.getRight() - 10, sbY, 8, sbHeight);
    }
    
    ofPopStyle();
}

void PlaylistHistoryUI::applyHoverView(ofVec2f& pan, float& zoom) {
    if (hoveredIndex != -1 && hoveredIndex < events.size()) {
        if (!bIsHoveringEvent) {
            originalPan = pan;
            originalZoom = zoom;
            bIsHoveringEvent = true;
        }
        pan = events[hoveredIndex].pan;
        zoom = events[hoveredIndex].zoom;
    } else {
        if (bIsHoveringEvent) {
            pan = originalPan;
            zoom = originalZoom;
            bIsHoveringEvent = false;
        }
    }
}

bool PlaylistHistoryUI::mouseMoved(int x, int y, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    int newHoveredIndex = -1;
    if (boxRect.inside(x, y) && y > boxRect.y + 30) {
        float localY = y - (boxRect.y + 30) + scrollY;
        int idx = localY / itemHeight;
        if (idx >= 0 && idx < events.size()) {
            newHoveredIndex = idx;
        }
    }
    
    if (newHoveredIndex != hoveredIndex) {
        hoveredIndex = newHoveredIndex;
        applyHoverView(pan, zoom);
    }
    
    return boxRect.inside(x, y);
}

bool PlaylistHistoryUI::mousePressed(int x, int y, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    if (boxRect.inside(x, y)) {
        if (hoveredIndex != -1) {
            pan = events[hoveredIndex].pan;
            zoom = events[hoveredIndex].zoom;
            close();
        }
        return true;
    }
    
    if (bIsHoveringEvent) {
        pan = originalPan;
        zoom = originalZoom;
        bIsHoveringEvent = false;
    }
    close();
    return false;
}

bool PlaylistHistoryUI::mouseScrolled(int x, int y, float sy) {
    if (!bIsVisible || !boxRect.inside(x, y)) return false;
    
    scrollY -= sy * 20.0f;
    scrollY = ofClamp(scrollY, 0, maxScroll);
    return true; 
}

bool PlaylistHistoryUI::keyPressed(int key, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    if (key == OF_KEY_ESC) {
        if (bIsHoveringEvent) {
            pan = originalPan;
            zoom = originalZoom;
            bIsHoveringEvent = false;
        }
        close();
        return true;
    }
    return false;
}