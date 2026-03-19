#include "PlaylistWindowControlsUI.h"
#include "ofApp.h"
#include "ViewApp.h"
#include "RoomApp.h"
#include "Scene2D_SIDE.h"
#include "RoomPreview.h"
#include "ButtonApp.h"
#include "ofAppGLFWWindow.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

void PlaylistWindowControlsUI::setup() {
    for(int i=0; i<4; i++) {
        viewBtns[i].set(i * 60, 0, 55, 30);
        viewHidden[i] = false;
    }
    
    moveV3Btn.set(4 * 60, 0, 55, 30); 
    
    for(int i=0; i<6; i++) {
        wxcvbBtns[i].set(320 + i * 60, 0, 56, 30);
        focusBtns[i].set(320 + i * 60, 35, 56, 30);
    }
    
    for(int i=0; i<4; i++) {
        gabBtns[i].set(700 + i * 60, 0, 56, 30); 
    }
}

void PlaylistWindowControlsUI::draw(ofApp* mainAppPtr) {
    ofPushStyle();
    
    for(int i=0; i<4; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
        if(!isLoaded) ofSetColor(80, 80, 80); 
        else if(!viewHidden[i]) ofSetColor(50, 200, 50); 
        else ofSetColor(30, 30, 30); 
        
        ofFill(); ofDrawRectangle(viewBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(viewBtns[i]);
        ofPushMatrix(); ofTranslate(viewBtns[i].x, viewBtns[i].y); ofScale(viewBtns[i].height / 30.0f, viewBtns[i].height / 30.0f);
        ofDrawBitmapString("V" + ofToString(i+1), 15, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    ofPushStyle();
    bool v3Loaded = (mainAppPtr && mainAppPtr->viewApps.size() > 2 && mainAppPtr->viewApps[2]);
    if(v3Loaded) ofSetColor(100, 150, 200); else ofSetColor(80, 80, 80); 
    ofFill(); ofDrawRectangle(moveV3Btn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(moveV3Btn);
    ofPushMatrix(); ofTranslate(moveV3Btn.x, moveV3Btn.y); ofScale(moveV3Btn.height / 30.0f, moveV3Btn.height / 30.0f);
    ofDrawBitmapString("->V3", 8, 20);
    ofPopMatrix();
    ofPopStyle();

    for(int i=0; i<6; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr != nullptr);
        if(isLoaded) ofSetColor(100, 150, 200); else ofSetColor(80, 80, 80);
        ofFill(); ofDrawRectangle(focusBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(focusBtns[i]);
        ofPushMatrix(); ofTranslate(focusBtns[i].x, focusBtns[i].y); ofScale(focusBtns[i].height / 30.0f, focusBtns[i].height / 30.0f);
        ofDrawBitmapString(focusNames[i], 4, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    for(int i=0; i<6; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr != nullptr);
        bool isOn = false;
        
        if(isLoaded) {
            if(i == 0) isOn = mainAppPtr->bDrawMain;
            else if(i == 1) isOn = mainAppPtr->bDrawRoom;
            else if(i == 2) isOn = mainAppPtr->bDrawZenit;
            else if(i == 3) isOn = mainAppPtr->bDrawScene2D;
            else if(i == 4 && mainAppPtr->roomPreviewApp) isOn = !mainAppPtr->roomPreviewApp->bPaused;
            else if(i == 5) isOn = mainAppPtr->bDrawButtons;
        }

        if(!isLoaded) ofSetColor(80, 80, 80);
        else if(isOn) ofSetColor(50, 200, 50); 
        else ofSetColor(30, 30, 30); 
        
        ofFill(); ofDrawRectangle(wxcvbBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(wxcvbBtns[i]);
        ofPushMatrix(); ofTranslate(wxcvbBtns[i].x, wxcvbBtns[i].y); ofScale(wxcvbBtns[i].height / 30.0f, wxcvbBtns[i].height / 30.0f);
        ofDrawBitmapString(wxcvbNames[i], 4, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    for(int i=0; i<4; i++) {
        ofPushStyle();
        ofEnableAlphaBlending();
        bool isLoaded = (mainAppPtr != nullptr);
        string text = "GAB ";
        int alpha = 255;
        bool isWireframe = false;
        
        if(isLoaded) {
            if(i == 0) {
                text += "M:" + ofToString(mainAppPtr->gabMode);
                if(mainAppPtr->gabMode == 0) alpha = 255;
                else if(mainAppPtr->gabMode == 1) alpha = 200;
                else if(mainAppPtr->gabMode == 2) alpha = 150;
                else if(mainAppPtr->gabMode == 3) alpha = 100;
                else alpha = 5; 
            }
            else if(i == 1 && mainAppPtr->roomApp) {
                bool isOn = mainAppPtr->roomApp->wallAlpha > 50;
                text += "R:" + string(isOn ? "ON" : "OFF");
                alpha = isOn ? 255 : 5;
            }
            else if(i == 2 && mainAppPtr->scene2D) {
                int mode = mainAppPtr->scene2D->bgDisplayMode;
                text += "S:" + ofToString(mode);
                if(mode == 0) alpha = 255;      
                else if(mode == 1) alpha = 100;  
                else if(mode == 2) { alpha = 255; isWireframe = true; } 
                else alpha = 5;
            }
            else if(i == 3 && mainAppPtr->scene2D) {
                int mode = mainAppPtr->scene2D->overlayMode;
                text += "O:" + (mode == 0 ? string("OFF") : (mode == 1 ? string("33") : (mode == 2 ? string("75") : string("100"))));
                if(mode == 0) alpha = 5;
                else if(mode == 1) alpha = 84;
                else if(mode == 2) alpha = 191;
                else alpha = 255;
            }
        }

        if(!isLoaded) {
            ofSetColor(80, 80, 80);
            ofFill();
            ofDrawRectangle(gabBtns[i]);
        } else {
            ofSetColor(255, 150, 0, alpha); 
            if(isWireframe) { ofNoFill(); ofSetLineWidth(3); } 
            else { ofFill(); }
            ofDrawRectangle(gabBtns[i]);
        }
        
        ofNoFill(); ofSetLineWidth(1); ofSetColor(255);
        ofDrawRectangle(gabBtns[i]);
        ofPushMatrix(); ofTranslate(gabBtns[i].x, gabBtns[i].y); ofScale(gabBtns[i].height / 30.0f, gabBtns[i].height / 30.0f);
        ofDrawBitmapString(text, 2, 20);
        ofPopMatrix();
        ofPopStyle();
    }
    
    ofPopStyle();
}

bool PlaylistWindowControlsUI::mousePressed(ofVec2f worldM, ofApp* mainAppPtr) {
    for(int i=0; i<4; i++) {
        if(viewBtns[i].inside(worldM)) {
            bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
            if(isLoaded) {
                viewHidden[i] = !viewHidden[i];
                mainAppPtr->viewApps[i]->bEnabled = !viewHidden[i]; 
                
                if(!viewHidden[i] && mainAppPtr->viewApps[i]->myWindow) {
                    auto targetGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->viewApps[i]->myWindow);
                    auto playlistGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->playlistWindowPtr);
                    if(targetGlfw) glfwFocusWindow(targetGlfw->getGLFWWindow());
                    if(playlistGlfw) glfwFocusWindow(playlistGlfw->getGLFWWindow());
                }
            }
            return true;
        }
    }

    if(moveV3Btn.inside(worldM)) {
        bool v3Loaded = (mainAppPtr && mainAppPtr->viewApps.size() > 2 && mainAppPtr->viewApps[2]);
        if(v3Loaded) {
            mainAppPtr->viewApps[2]->moveWindow(ofGetScreenWidth(), 0);
        }
        return true;
    }
    
    for(int i=0; i<6; i++) {
        if(focusBtns[i].inside(worldM)) {
            if(mainAppPtr) {
                shared_ptr<ofAppBaseWindow> targetWin;
                if(i == 0) targetWin = mainAppPtr->mainWindowPtr;
                else if(i == 1) targetWin = mainAppPtr->roomWindowPtr;
                else if(i == 2) targetWin = mainAppPtr->zenitWindowPtr;
                else if(i == 3) targetWin = mainAppPtr->scene2DWindowPtr;
                else if(i == 4) targetWin = mainAppPtr->previewWindowPtr;
                else if(i == 5) targetWin = mainAppPtr->buttonWindowPtr;
                
                if(targetWin) {
                    auto targetGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(targetWin);
                    if(targetGlfw) {
                        glfwShowWindow(targetGlfw->getGLFWWindow());
                        glfwRestoreWindow(targetGlfw->getGLFWWindow()); 
                        glfwFocusWindow(targetGlfw->getGLFWWindow());   
                    }
                }
            }
            return true;
        }
    }

    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(worldM)) {
            if(mainAppPtr) {
                if(wxcvbKeys[i] != 0) mainAppPtr->keyPressed(wxcvbKeys[i]);
                else if(i == 0) mainAppPtr->bDrawMain = !mainAppPtr->bDrawMain;
                
                shared_ptr<ofAppBaseWindow> targetWin;
                bool isOn = false;
                
                if(i == 0) { targetWin = mainAppPtr->mainWindowPtr; isOn = mainAppPtr->bDrawMain; }
                else if(i == 1) { targetWin = mainAppPtr->roomWindowPtr; isOn = mainAppPtr->bDrawRoom; }
                else if(i == 2) { targetWin = mainAppPtr->zenitWindowPtr; isOn = mainAppPtr->bDrawZenit; }
                else if(i == 3) { targetWin = mainAppPtr->scene2DWindowPtr; isOn = mainAppPtr->bDrawScene2D; }
                else if(i == 4) { targetWin = mainAppPtr->previewWindowPtr; if(mainAppPtr->roomPreviewApp) isOn = !mainAppPtr->roomPreviewApp->bPaused; }
                else if(i == 5) { targetWin = mainAppPtr->buttonWindowPtr; isOn = mainAppPtr->bDrawButtons; }
                
                if(isOn && targetWin) {
                    auto targetGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(targetWin);
                    auto playlistGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->playlistWindowPtr);
                    if(targetGlfw) glfwFocusWindow(targetGlfw->getGLFWWindow());
                    if(playlistGlfw) glfwFocusWindow(playlistGlfw->getGLFWWindow());
                }
            }
            return true;
        }
    }

    for(int i=0; i<4; i++) {
        if(gabBtns[i].inside(worldM)) {
            if(mainAppPtr) {
                if(i == 0) mainAppPtr->keyPressed('g');
                else if(i == 1 && mainAppPtr->roomApp) mainAppPtr->roomApp->keyPressed('g');
                else if(i == 2 && mainAppPtr->scene2D) mainAppPtr->scene2D->keyPressed('g');
                else if(i == 3 && mainAppPtr->scene2D) mainAppPtr->scene2D->keyPressed('h');
            }
            return true;
        }
    }
    return false;
}

string PlaylistWindowControlsUI::getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager) {
    for(int i=0; i<4; i++) if(viewBtns[i].inside(worldM)) return tooltipManager.getTooltipText("V" + ofToString(i+1));
    if(moveV3Btn.inside(worldM)) return tooltipManager.getTooltipText("->V3");
    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(worldM)) return tooltipManager.getTooltipText(wxcvbNames[i]);
        if(focusBtns[i].inside(worldM)) return tooltipManager.getTooltipText(focusNames[i]);
    }
    for(int i=0; i<4; i++) if(gabBtns[i].inside(worldM)) return tooltipManager.getTooltipText("GAB " + ofToString(i));
    return "";
}

void PlaylistWindowControlsUI::saveSettings(ofJson& pt) {
    auto saveR = [&](const string& key, const ofRectangle& r) { pt[key]["x"] = r.x; pt[key]["y"] = r.y; pt[key]["w"] = r.width; pt[key]["h"] = r.height; };
    for(int i=0; i<4; i++) saveR("view_" + ofToString(i), viewBtns[i]);
    saveR("moveV3", moveV3Btn);
    for(int i=0; i<6; i++) { saveR("wxcvb_" + ofToString(i), wxcvbBtns[i]); saveR("focus_" + ofToString(i), focusBtns[i]); }
    for(int i=0; i<4; i++) saveR("gab_" + ofToString(i), gabBtns[i]);
}

void PlaylistWindowControlsUI::loadSettings(const ofJson& pt) {
    auto loadR = [&](const string& key, ofRectangle& r) {
        if(pt.contains(key)) { 
            r.x = pt[key].value("x", r.x); r.y = pt[key].value("y", r.y); 
            if(pt[key].contains("w")) r.width = pt[key].value("w", r.width);
            if(pt[key].contains("h")) r.height = pt[key].value("h", r.height);
        }
    };
    for(int i=0; i<4; i++) loadR("view_" + ofToString(i), viewBtns[i]);
    loadR("moveV3", moveV3Btn);
    for(int i=0; i<6; i++) { loadR("wxcvb_" + ofToString(i), wxcvbBtns[i]); loadR("focus_" + ofToString(i), focusBtns[i]); }
    for(int i=0; i<4; i++) loadR("gab_" + ofToString(i), gabBtns[i]);
}

vector<ofRectangle*> PlaylistWindowControlsUI::getInteractableRects() {
    vector<ofRectangle*> rects;
    for(int i=0; i<4; i++) rects.push_back(&viewBtns[i]);
    rects.push_back(&moveV3Btn);
    for(int i=0; i<6; i++) { rects.push_back(&wxcvbBtns[i]); rects.push_back(&focusBtns[i]); }
    for(int i=0; i<4; i++) rects.push_back(&gabBtns[i]);
    return rects;
}

ofRectangle* PlaylistWindowControlsUI::findButtonAt(ofVec2f pos) {
    for(int i=0; i<4; i++) if(viewBtns[i].inside(pos)) return &viewBtns[i];
    if(moveV3Btn.inside(pos)) return &moveV3Btn;
    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(pos)) return &wxcvbBtns[i];
        if(focusBtns[i].inside(pos)) return &focusBtns[i];
    }
    for(int i=0; i<4; i++) if(gabBtns[i].inside(pos)) return &gabBtns[i];
    return nullptr;
}