#include "PlaylistWinPosUI.h"
#include "PlaylistVisualizerApp.h"
#include "ofAppBaseWindow.h"

void PlaylistWinPosUI::setup() {
}

void PlaylistWinPosUI::toggle(float btnRightX, float btnY) {
    bIsVisible = !bIsVisible;
    editingPresetIndex = -1;
    if (bIsVisible) {
        anchorX = btnRightX;
        anchorY = btnY;
        hoveredIndex = -1;
        updateLayout();
    }
}

void PlaylistWinPosUI::close() {
    bIsVisible = false;
    editingPresetIndex = -1;
}

void PlaylistWinPosUI::updateLayout() {
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

void PlaylistWinPosUI::addPreset(PlaylistVisualizerApp* app) {
    if (!app) return;
    WinPosPreset p;
    p.name = "Pos " + ofToString(presets.size() + 1);
    for(int w=0; w<7; w++) {
        auto win = app->getAppWindow(w);
        if(win) {
            p.windows[w].set(win->getWindowPosition().x, win->getWindowPosition().y, win->getWindowSize().x, win->getWindowSize().y);
        } else {
            p.windows[w].set(0,0,0,0);
        }
    }
    presets.push_back(p);
    updateLayout();
}

void PlaylistWinPosUI::draw() {
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
    ofDrawBitmapString("+ Memoriser Pos", addBtnRect.x + 15, addBtnRect.y + 17);
    
    ofPopStyle();
}

bool PlaylistWinPosUI::mouseMoved(int x, int y) {
    if (!bIsVisible) return false;
    
    hoveredIndex = -1;
    for (size_t i = 0; i < presets.size(); ++i) {
        if (presets[i].rect.inside(x, y) && !presets[i].deleteBtnRect.inside(x, y) && !presets[i].renameBtnRect.inside(x, y)) {
            hoveredIndex = i;
            break;
        }
    }
    return boxRect.inside(x, y);
}

bool PlaylistWinPosUI::mousePressed(int x, int y, PlaylistVisualizerApp* app) {
    if (!bIsVisible) return false;
    
    if (addBtnRect.inside(x, y)) {
        addPreset(app);
        if (app) app->saveUndoState("Save Window Preset " + ofToString(presets.size()));
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
            updateLayout();
            return true;
        }
        
        if (presets[i].rect.inside(x, y)) {
            applyPresetByIndex(i, app);
            close();
            return true;
        }
    }
    
    if (boxRect.inside(x, y)) {
        editingPresetIndex = -1;
        return true;
    }
    
    editingPresetIndex = -1;
    close();
    return false;
}

bool PlaylistWinPosUI::keyPressed(int key) {
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
        close();
        return true;
    }
    return false;
}

void PlaylistWinPosUI::saveSettings(ofJson& pt) {
    pt["window_presets"] = ofJson::array();
    for(size_t i=0; i<presets.size(); i++) {
        ofJson p;
        p["name"] = presets[i].name;
        p["windows"] = ofJson::array();
        for(int w=0; w<7; w++) {
            ofJson wJson;
            wJson["x"] = presets[i].windows[w].x;
            wJson["y"] = presets[i].windows[w].y;
            wJson["w"] = presets[i].windows[w].width;
            wJson["h"] = presets[i].windows[w].height;
            p["windows"].push_back(wJson);
        }
        pt["window_presets"].push_back(p);
    }
}

void PlaylistWinPosUI::loadSettings(const ofJson& pt) {
    if(pt.contains("window_presets") && pt["window_presets"].is_array()) {
        presets.clear();
        for(size_t i=0; i<pt["window_presets"].size(); i++) {
            // Ignore empty/unsaved presets from older versions
            if(pt["window_presets"][i].contains("saved") && pt["window_presets"][i]["saved"] == false) {
                continue;
            }
            
            WinPosPreset p;
            p.name = pt["window_presets"][i].value("name", "Pos " + ofToString(presets.size()+1));
            if(pt["window_presets"][i].contains("windows") && pt["window_presets"][i]["windows"].is_array()) {
                for(int w=0; w<7 && w<pt["window_presets"][i]["windows"].size(); w++) {
                    p.windows[w].x = pt["window_presets"][i]["windows"][w].value("x", 0.0f);
                    p.windows[w].y = pt["window_presets"][i]["windows"][w].value("y", 0.0f);
                    p.windows[w].width = pt["window_presets"][i]["windows"][w].value("w", 0.0f);
                    p.windows[w].height = pt["window_presets"][i]["windows"][w].value("h", 0.0f);
                }
            }
            presets.push_back(p);
        }
        updateLayout();
    }
}

bool PlaylistWinPosUI::applyPresetByIndex(int index, PlaylistVisualizerApp* app) {
    if (!app) return false;
    if (index >= 0 && index < presets.size()) {
        for(int w=0; w<7; w++) {
            auto win = app->getAppWindow(w);
            if(win && presets[index].windows[w].width > 50) {
                win->setWindowPosition(presets[index].windows[w].x, presets[index].windows[w].y);
                win->setWindowShape(presets[index].windows[w].width, presets[index].windows[w].height);
            }
        }
        app->historyUI.addEvent("Load Window Preset " + ofToString(index+1), app->pan, app->zoom);
        return true;
    }
    return false;
}