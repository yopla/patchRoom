#include "PlaylistViewsUI.h"

void PlaylistViewsUI::setup() {
}

void PlaylistViewsUI::toggle(ofVec2f currentPan, float currentZoom, float btnRightX, float btnY) {
    bIsVisible = !bIsVisible;
    editingPresetIndex = -1;
    if (bIsVisible) {
        originalPan = currentPan;
        originalZoom = currentZoom;
        bIsHoveringPreset = false;
        hoveredIndex = -1;
        anchorX = btnRightX;
        anchorY = btnY;
        updateLayout();
    } else {
        if (bIsHoveringPreset) {
            bIsHoveringPreset = false;
        }
    }
}

void PlaylistViewsUI::close() {
    bIsVisible = false;
    editingPresetIndex = -1;
}

void PlaylistViewsUI::updateLayout() {
    float width = 200;
    float itemH = 30;
    float height = (presets.size() + 1) * itemH + 15; 
    
    boxRect.set(anchorX + 10, anchorY, width, height);
    
    float currentY = boxRect.y + 5;
    for (size_t i = 0; i < presets.size(); ++i) {
        presets[i].rect.set(boxRect.x + 5, currentY, width - 10, itemH - 4);
        presets[i].deleteBtnRect.set(boxRect.x + width - 30, currentY + 4, 20, itemH - 12);
        presets[i].renameBtnRect.set(boxRect.x + width - 55, currentY + 4, 20, itemH - 12);
        currentY += itemH;
    }
    
    addBtnRect.set(boxRect.x + 5, currentY + 5, width - 10, itemH - 4);
}

void PlaylistViewsUI::addPreset(ofVec2f pan, float zoom) {
    ViewPreset p;
    p.name = "Vue " + ofToString(presets.size() + 1);
    p.pan = pan;
    p.zoom = zoom;
    presets.push_back(p);
    updateLayout();
}

void PlaylistViewsUI::draw() {
    if (!bIsVisible) return;
    
    ofPushStyle();
    
    ofSetColor(30, 30, 30, 240);
    ofFill();
    ofDrawRectangle(boxRect);
    ofSetColor(200);
    ofNoFill();
    ofDrawRectangle(boxRect);
    
    for (size_t i = 0; i < presets.size(); ++i) {
        if ((int)i == hoveredIndex) {
            ofSetColor(80, 120, 200, 200);
        } else {
            ofSetColor(50, 50, 50, 200);
        }
        ofFill();
        ofDrawRectangle(presets[i].rect);
        
        ofSetColor(200);
        ofNoFill();
        ofDrawRectangle(presets[i].rect);
        
        std::string display = presets[i].name;
        if (editingPresetIndex == (int)i) {
            ofSetColor(255, 200, 0);
            if (ofGetFrameNum() % 60 < 30) display += "_";
        } else {
            ofSetColor(255);
        }
        ofDrawBitmapString(display, presets[i].rect.x + 10, presets[i].rect.y + 17);
        
        ofSetColor(200, 50, 50);
        ofFill();
        ofDrawRectangle(presets[i].deleteBtnRect);
        ofSetColor(255);
        ofDrawBitmapString("X", presets[i].deleteBtnRect.x + 6, presets[i].deleteBtnRect.y + 12);
        
        ofSetColor(50, 150, 200);
        ofFill();
        ofDrawRectangle(presets[i].renameBtnRect);
        ofSetColor(255);
        ofDrawBitmapString("R", presets[i].renameBtnRect.x + 6, presets[i].renameBtnRect.y + 12);
    }
    
    ofSetColor(50, 150, 50, 200);
    ofFill();
    ofDrawRectangle(addBtnRect);
    ofSetColor(200);
    ofNoFill();
    ofDrawRectangle(addBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("+ Memoriser Vue", addBtnRect.x + 15, addBtnRect.y + 17);
    
    ofPopStyle();
}

void PlaylistViewsUI::applyHoverView(ofVec2f& pan, float& zoom) {
    if (hoveredIndex != -1 && hoveredIndex < presets.size()) {
        if (!bIsHoveringPreset) {
            originalPan = pan;
            originalZoom = zoom;
            bIsHoveringPreset = true;
        }
        pan = presets[hoveredIndex].pan;
        zoom = presets[hoveredIndex].zoom;
    } else {
        if (bIsHoveringPreset) {
            pan = originalPan;
            zoom = originalZoom;
            bIsHoveringPreset = false;
        }
    }
}

bool PlaylistViewsUI::mouseMoved(int x, int y, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    int newHoveredIndex = -1;
    for (size_t i = 0; i < presets.size(); ++i) {
        if (presets[i].rect.inside(x, y) && !presets[i].deleteBtnRect.inside(x, y) && !presets[i].renameBtnRect.inside(x, y)) {
            newHoveredIndex = i;
            break;
        }
    }
    
    if (newHoveredIndex != hoveredIndex) {
        hoveredIndex = newHoveredIndex;
        applyHoverView(pan, zoom);
    }
    
    return boxRect.inside(x, y);
}

bool PlaylistViewsUI::mousePressed(int x, int y, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    if (addBtnRect.inside(x, y)) {
        addPreset(originalPan, originalZoom);
        return true;
    }
    
    for (size_t i = 0; i < presets.size(); ++i) {
        if (presets[i].renameBtnRect.inside(x, y)) {
            editingPresetIndex = i;
            return true;
        }
        
        if (presets[i].deleteBtnRect.inside(x, y)) {
            presets.erase(presets.begin() + i);
            hoveredIndex = -1;
            applyHoverView(pan, zoom); 
            updateLayout();
            return true;
        }
        
        if (presets[i].rect.inside(x, y)) {
            pan = presets[i].pan;
            zoom = presets[i].zoom;
            close();
            return true;
        }
    }
    
    if (boxRect.inside(x, y)) {
        editingPresetIndex = -1; // Quitte l'édition si clic dans le fond de la box
        return true;
    }
    
    editingPresetIndex = -1;
    if (bIsHoveringPreset) {
        pan = originalPan;
        zoom = originalZoom;
        bIsHoveringPreset = false;
    }
    close();
    return false;
}

bool PlaylistViewsUI::keyPressed(int key, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    if (editingPresetIndex != -1 && editingPresetIndex < presets.size()) {
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if (!presets[editingPresetIndex].name.empty()) {
                presets[editingPresetIndex].name.pop_back();
            }
        } else if (key == OF_KEY_RETURN) {
            editingPresetIndex = -1;
        } else if (key == OF_KEY_ESC) {
            editingPresetIndex = -1;
        } else if (key >= 32 && key <= 126) {
            presets[editingPresetIndex].name += (char)key;
        }
        return true; // On consomme la touche
    }
    
    if (key == OF_KEY_ESC) {
        if (bIsHoveringPreset) {
            pan = originalPan;
            zoom = originalZoom;
            bIsHoveringPreset = false;
        }
        close();
        return true;
    }
    return false;
}

void PlaylistViewsUI::saveSettings(ofJson& pt) {
    pt["presets"] = ofJson::array();
    for(size_t i=0; i<presets.size(); i++) {
        ofJson p;
        p["name"] = presets[i].name;
        p["pan_x"] = presets[i].pan.x;
        p["pan_y"] = presets[i].pan.y;
        p["zoom"] = presets[i].zoom;
        pt["presets"].push_back(p);
    }
}

void PlaylistViewsUI::loadSettings(const ofJson& pt) {
    if(pt.contains("presets") && pt["presets"].is_array()) {
        presets.clear();
        for(size_t i=0; i<pt["presets"].size(); i++) {
            ViewPreset p;
            p.name = pt["presets"][i].value("name", "Vue " + ofToString(i+1));
            p.pan.x = pt["presets"][i].value("pan_x", 250.0f);
            p.pan.y = pt["presets"][i].value("pan_y", 0.0f);
            p.zoom = pt["presets"][i].value("zoom", 1.0f);
            presets.push_back(p);
        }
    }
}

bool PlaylistViewsUI::applyPresetByIndex(int index, ofVec2f& pan, float& zoom) {
    if (index >= 0 && index < presets.size()) {
        pan = presets[index].pan;
        zoom = presets[index].zoom;
        return true;
    }
    return false;
}