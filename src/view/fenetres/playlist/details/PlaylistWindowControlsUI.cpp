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
    float startX = 200;
    float startY = -250;

    for(int i=0; i<4; i++) {
        viewBtns[i].set(startX + i * 60, startY, 55, 30);
        viewHidden[i] = false;
        toggleWinBtns[i].set(startX + i * 60, startY + 35, 55, 30);
        moveWinBtns[i].set(startX + i * 60, startY + 70, 55, 30);
        recWinBtns[i].set(startX + i * 60, startY + 105, 55, 30);
    }
    formatBtn.set(startX + 240, startY, 75, 30);
    qualityBtn.set(startX + 320, startY, 75, 30);
    arrangeWinBtn.set(startX + 400, startY, 100, 30);
    
    for(int i=0; i<6; i++) {
        wxcvbBtns[i].set(startX + 240 + i * 60, startY + 35, 56, 30);
        focusBtns[i].set(startX + 240 + i * 60, startY + 70, 56, 30);
    }
    
    for(int i=0; i<4; i++) {
        gabBtns[i].set(startX + 240 + i * 60, startY + 105, 56, 30); 
    }
    
    gabOptions[0] = {"100%", "75%", "33%", "10%", "OFF"};
    gabOptions[1] = {"ON", "OFF", "Scene2D"};
    gabOptions[2] = {"Opaque", "Transp", "FBOs", "Rien"};
    gabOptions[3] = {"OFF", "33%", "75%", "100%"};
    
    roomAlphaBtn.set(startX + 240 + 1 * 60, startY + 140, 56, 30);
    roomAlphaOptions = {"100%", "75%", "33%", "0%"};

    diffuseRoomBtn.set(startX + 240 + 1 * 60, startY + 175, 56, 30);
    diffuseScene2DBtn.set(startX + 240 + 3 * 60, startY + 140, 56, 30);
}

void PlaylistWindowControlsUI::draw(ofApp* mainAppPtr) {
    ofPushStyle();
    
    // Helper pour générer une légère teinte par Vue (Rouge, Vert, Bleu, Jaune)
    auto getBtnColor = [&](int i, bool isLoaded, bool isOn, int btnType) -> ofColor {
        if (!isLoaded) return ofColor(30, 30, 30); // Gris très sombre si non instanciée
        
        ofColor offTints[4] = { ofColor(60, 30, 30), ofColor(30, 60, 30), ofColor(30, 45, 70), ofColor(60, 60, 30) };
        ofColor onTints[4] = { ofColor(200, 60, 60), ofColor(60, 200, 60), ofColor(60, 120, 255), ofColor(200, 200, 60) };
        ofColor actionTints[4] = { ofColor(120, 50, 50), ofColor(50, 120, 50), ofColor(50, 80, 150), ofColor(120, 120, 50) };
        
        if (btnType == 0) return isOn ? onTints[i] : offTints[i]; // Toggles
        if (btnType == 1) return actionTints[i];                  // Move
        if (btnType == 2) return isOn ? (sin(ofGetElapsedTimef() * 10.0f) > 0 ? ofColor(255, 50, 50) : ofColor(150, 0, 0)) : offTints[i]; // REC
        return ofColor(80);
    };

    for(int i=0; i<4; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
        ofSetColor(getBtnColor(i, isLoaded, !viewHidden[i], 0));
        
        ofFill(); ofDrawRectangle(viewBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(viewBtns[i]);
        ofPushMatrix(); ofTranslate(viewBtns[i].x, viewBtns[i].y); ofScale(viewBtns[i].height / 30.0f, viewBtns[i].height / 30.0f);
        ofDrawBitmapString("V" + ofToString(i+1), 15, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    for(int i=0; i<4; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
        ofSetColor(getBtnColor(i, isLoaded, false, 1));
        ofFill(); ofDrawRectangle(moveWinBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(moveWinBtns[i]);
        ofPushMatrix(); ofTranslate(moveWinBtns[i].x, moveWinBtns[i].y); ofScale(moveWinBtns[i].height / 30.0f, moveWinBtns[i].height / 30.0f);
        ofDrawBitmapString("->V" + ofToString(i+1), 8, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    for(int i=0; i<4; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
        ofSetColor(getBtnColor(i, isLoaded, !bWinHidden[i], 0));
        ofFill(); ofDrawRectangle(toggleWinBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(toggleWinBtns[i]);
        ofPushMatrix(); ofTranslate(toggleWinBtns[i].x, toggleWinBtns[i].y); ofScale(toggleWinBtns[i].height / 30.0f, toggleWinBtns[i].height / 30.0f);
        ofDrawBitmapString("V" + ofToString(i+1) + " WIN", 4, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    for(int i=0; i<4; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
        bool isRec = isLoaded && mainAppPtr->viewApps[i]->bRecording;

        ofSetColor(getBtnColor(i, isLoaded, isRec, 2));

        ofFill(); ofDrawRectangle(recWinBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(recWinBtns[i]);
        ofPushMatrix(); ofTranslate(recWinBtns[i].x, recWinBtns[i].y); ofScale(recWinBtns[i].height / 30.0f, recWinBtns[i].height / 30.0f);
        ofDrawBitmapString("REC V" + ofToString(i+1), 4, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    if (mainAppPtr) {
        ofPushStyle();
        ofSetColor(100, 100, 150);
        ofFill(); ofDrawRectangle(formatBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(formatBtn);
        ofDrawBitmapString("FMT: " + mainAppPtr->recordFormat, formatBtn.x + 4, formatBtn.y + 20);

        ofSetColor(100, 150, 100);
        ofFill(); ofDrawRectangle(qualityBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(qualityBtn);
        string qStr = "BEST";
        if(mainAppPtr->qualityIndex == 1) qStr = "HIGH";
        else if(mainAppPtr->qualityIndex == 2) qStr = "MED";
        else if(mainAppPtr->qualityIndex == 3) qStr = "LOW";
        else if(mainAppPtr->qualityIndex == 4) qStr = "WORST";
        ofDrawBitmapString("Q: " + qStr, qualityBtn.x + 4, qualityBtn.y + 20);

        ofSetColor(150, 100, 150);
        ofFill(); ofDrawRectangle(arrangeWinBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(arrangeWinBtn);
        ofDrawBitmapString("ARRANGE", arrangeWinBtn.x + 15, arrangeWinBtn.y + 20);
        ofPopStyle();
    }

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
        string text = "";
        int alpha = 255;
        bool isWireframe = false;
        
        int currentState = 0;
        
        if(isLoaded) {
            if(i == 0) {
                currentState = mainAppPtr->gabMode;
                text = "M:" + ofToString(currentState);
                if(currentState == 0) alpha = 255;
                else if(currentState == 1) alpha = 200;
                else if(currentState == 2) alpha = 150;
                else if(currentState == 3) alpha = 100;
                else alpha = 5; 
            }
            else if(i == 1 && mainAppPtr->roomApp) {
                currentState = mainAppPtr->roomApp->bgMode;
                text = "R:" + gabOptions[1][currentState];
                alpha = (currentState == 1) ? 5 : 255;
            }
            else if(i == 2 && mainAppPtr->scene2D) {
                currentState = mainAppPtr->scene2D->bgDisplayMode;
                text = "S:" + ofToString(currentState);
                if(currentState == 0) alpha = 255;      
                else if(currentState == 1) alpha = 100;  
                else if(currentState == 2) { alpha = 255; isWireframe = true; } 
                else alpha = 5;
            }
            else if(i == 3 && mainAppPtr->scene2D) {
                currentState = mainAppPtr->scene2D->overlayMode;
                text = "O:" + (currentState == 0 ? string("OFF") : (currentState == 1 ? string("33") : (currentState == 2 ? string("75") : string("100"))));
                if(currentState == 0) alpha = 5;
                else if(currentState == 1) alpha = 84;
                else if(currentState == 2) alpha = 191;
                else alpha = 255;
            }
        }

        if(!isLoaded) {
            ofSetColor(80, 80, 80);
            ofFill();
            ofDrawRectangle(gabBtns[i]);
        } else {
            if (bGabAccordionOpen[i]) {
                ofSetColor(150, 150, 200, 255);
                ofFill();
            } else {
                ofSetColor(255, 150, 0, alpha); 
                if(isWireframe) { ofNoFill(); ofSetLineWidth(3); } 
                else { ofFill(); }
            }
            ofDrawRectangle(gabBtns[i]);
        }
        
        ofNoFill(); ofSetLineWidth(1); ofSetColor(255);
        ofDrawRectangle(gabBtns[i]);
        
        text += bGabAccordionOpen[i] ? "[-]" : "[+]";
        
        ofPushMatrix(); ofTranslate(gabBtns[i].x, gabBtns[i].y); ofScale(gabBtns[i].height / 30.0f, gabBtns[i].height / 30.0f);
        ofDrawBitmapString(text, 2, 20);
        ofPopMatrix();
        ofPopStyle();
    }
    
    if(mainAppPtr && mainAppPtr->roomApp) {
        ofPushStyle();
        float currentAlpha = mainAppPtr->roomApp->wallAlpha;
        string alphaStr = "100";
        if (currentAlpha <= 0.0f) alphaStr = "0";
        else if (currentAlpha <= 85.0f) alphaStr = "33";
        else if (currentAlpha <= 192.0f) alphaStr = "75";

        if (bRoomAlphaAccordionOpen) {
            ofSetColor(150, 150, 200, 255);
            ofFill();
        } else {
            ofSetColor(255, 150, 0, std::max(25.0f, currentAlpha)); // Reste visible même à 0%
            ofFill();
        }
        ofDrawRectangle(roomAlphaBtn);
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(roomAlphaBtn);
        
        ofPushMatrix();
        ofTranslate(roomAlphaBtn.x, roomAlphaBtn.y);
        ofScale(roomAlphaBtn.height / 30.0f, roomAlphaBtn.height / 30.0f);
        ofDrawBitmapString("A:" + alphaStr + (bRoomAlphaAccordionOpen ? "[-]" : "[+]"), 2, 20);
        ofPopMatrix();
        ofPopStyle();
    }

    if (mainAppPtr) {
        ofPushStyle();
        if (mainAppPtr->bDiffuseRoom) ofSetColor(50, 200, 50); else ofSetColor(200, 50, 50);
        ofFill(); ofDrawRectangle(diffuseRoomBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(diffuseRoomBtn);
        ofPushMatrix(); ofTranslate(diffuseRoomBtn.x, diffuseRoomBtn.y); ofScale(diffuseRoomBtn.height / 30.0f, diffuseRoomBtn.height / 30.0f);
        ofDrawBitmapString(mainAppPtr->bDiffuseRoom ? "DIF:ON" : "DIF:OFF", 4, 20);
        ofPopMatrix();
        
        if (mainAppPtr->bDiffuseScene2D) ofSetColor(50, 200, 50); else ofSetColor(200, 50, 50);
        ofFill(); ofDrawRectangle(diffuseScene2DBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(diffuseScene2DBtn);
        ofPushMatrix(); ofTranslate(diffuseScene2DBtn.x, diffuseScene2DBtn.y); ofScale(diffuseScene2DBtn.height / 30.0f, diffuseScene2DBtn.height / 30.0f);
        ofDrawBitmapString(mainAppPtr->bDiffuseScene2D ? "DIF:ON" : "DIF:OFF", 4, 20);
        ofPopMatrix();
        ofPopStyle();
    }
    
    for(int i=0; i<4; i++) {
        gabOptionRects[i].clear();
        if (bGabAccordionOpen[i] && mainAppPtr) {
            int currentState = 0;
            if(i == 0) currentState = mainAppPtr->gabMode;
            else if(i == 1 && mainAppPtr->roomApp) currentState = mainAppPtr->roomApp->bgMode;
            else if(i == 2 && mainAppPtr->scene2D) currentState = mainAppPtr->scene2D->bgDisplayMode;
            else if(i == 3 && mainAppPtr->scene2D) currentState = mainAppPtr->scene2D->overlayMode;

            for(size_t j=0; j<gabOptions[i].size(); j++) {
                ofRectangle optRect(gabBtns[i].x, gabBtns[i].getBottom() + j * gabBtns[i].height, gabBtns[i].width + 20, gabBtns[i].height);
                gabOptionRects[i].push_back(optRect);
                
                ofPushStyle();
                if (currentState == j) ofSetColor(200, 200, 50); else ofSetColor(80);
                ofFill(); ofDrawRectangle(optRect);
                ofNoFill(); ofSetColor(200); ofDrawRectangle(optRect);
                ofSetColor(255);
                ofPushMatrix(); ofTranslate(optRect.x, optRect.y); ofScale(optRect.height / 30.0f, optRect.height / 30.0f);
                ofDrawBitmapString(gabOptions[i][j], 4, 20);
                ofPopMatrix();
                ofPopStyle();
            }
        }
    }
    
    if (bRoomAlphaAccordionOpen && mainAppPtr && mainAppPtr->roomApp) {
        roomAlphaOptionRects.clear();
        float currentAlpha = mainAppPtr->roomApp->wallAlpha;
        for(size_t j=0; j < roomAlphaOptions.size(); j++) {
            ofRectangle optRect(roomAlphaBtn.x, roomAlphaBtn.getBottom() + j * roomAlphaBtn.height, roomAlphaBtn.width + 20, roomAlphaBtn.height);
            roomAlphaOptionRects.push_back(optRect);
            
            ofPushStyle();
            bool isSelected = false;
            if (j == 0 && currentAlpha > 192.0f) isSelected = true;
            if (j == 1 && currentAlpha > 85.0f && currentAlpha <= 192.0f) isSelected = true;
            if (j == 2 && currentAlpha > 0.0f && currentAlpha <= 85.0f) isSelected = true;
            if (j == 3 && currentAlpha <= 0.0f) isSelected = true;

            if (isSelected) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(optRect);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(optRect);
            ofSetColor(255);
            ofPushMatrix(); ofTranslate(optRect.x, optRect.y); ofScale(optRect.height / 30.0f, optRect.height / 30.0f);
            ofDrawBitmapString(roomAlphaOptions[j], 4, 20);
            ofPopMatrix();
            ofPopStyle();
        }
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

    for(int i=0; i<4; i++) {
        if(moveWinBtns[i].inside(worldM)) {
            bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
            if(isLoaded) {
                mainAppPtr->viewApps[i]->moveWindow(1300, 100); // Rapproche les fenêtres à la même position qu'au démarrage
            }
            return true;
        }
    }
    
    for(int i=0; i<4; i++) {
        if(toggleWinBtns[i].inside(worldM)) {
            bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
            if(isLoaded && mainAppPtr->viewApps[i]->myWindow) {
                auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->viewApps[i]->myWindow);
                if(glfwWin) {
                    GLFWwindow* nativeWin = glfwWin->getGLFWWindow();
                    bWinHidden[i] = !bWinHidden[i];
                    if(bWinHidden[i]) {
                        mainAppPtr->viewApps[i]->bEnabled = false; // Stoppe le rendu/mise à jour
                        glfwHideWindow(nativeWin); // Ferme physiquement la fenêtre GLFW
                    } else {
                        mainAppPtr->viewApps[i]->bEnabled = true;
                        glfwShowWindow(nativeWin);
                        glfwRestoreWindow(nativeWin); 
                    }
                }
            }
            return true;
        }
    }
    
    for(int i=0; i<4; i++) {
        if(recWinBtns[i].inside(worldM)) {
            bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
            if(isLoaded) {
                mainAppPtr->viewApps[i]->toggleRecording();
            }
            return true;
        }
    }

    if(formatBtn.inside(worldM)) {
        if(mainAppPtr) mainAppPtr->recordFormat = (mainAppPtr->recordFormat == "jpg") ? "png" : "jpg";
        return true;
    }
    
    if(qualityBtn.inside(worldM)) {
        if(mainAppPtr) {
            mainAppPtr->qualityIndex = (mainAppPtr->qualityIndex + 1) % 5;
            if(mainAppPtr->qualityIndex == 0) mainAppPtr->recordQuality = OF_IMAGE_QUALITY_BEST;
            else if(mainAppPtr->qualityIndex == 1) mainAppPtr->recordQuality = OF_IMAGE_QUALITY_HIGH;
            else if(mainAppPtr->qualityIndex == 2) mainAppPtr->recordQuality = OF_IMAGE_QUALITY_MEDIUM;
            else if(mainAppPtr->qualityIndex == 3) mainAppPtr->recordQuality = OF_IMAGE_QUALITY_LOW;
            else if(mainAppPtr->qualityIndex == 4) mainAppPtr->recordQuality = OF_IMAGE_QUALITY_WORST;
        }
        return true;
    }

    if(arrangeWinBtn.inside(worldM)) {
        if(mainAppPtr) {
            int screenW = ofGetScreenWidth();
            int screenH = ofGetScreenHeight();

            int w3 = screenW / 3;
            int h2 = screenH / 2;

            auto setWin = [&](shared_ptr<ofAppBaseWindow> win, int x, int y, int w, int h) {
                if(win) {
                    win->setWindowPosition(x, y);
                    win->setWindowShape(w, h);
                }
            };

            setWin(mainAppPtr->mainWindowPtr, 0, 0, w3, h2);
            setWin(mainAppPtr->roomWindowPtr, 0, h2, w3, h2);
            setWin(mainAppPtr->scene2DWindowPtr, w3, 0, w3, h2);
            setWin(mainAppPtr->previewWindowPtr, w3, h2, w3, h2);
            setWin(mainAppPtr->zenitWindowPtr, w3*2, 0, w3, h2);
            setWin(mainAppPtr->buttonWindowPtr, w3*2, h2, w3, h2);
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

    if (bRoomAlphaAccordionOpen) {
        for(size_t i = 0; i < roomAlphaOptionRects.size(); i++) {
            if(roomAlphaOptionRects[i].inside(worldM)) {
                if(mainAppPtr && mainAppPtr->roomApp) {
                    // Si l'alpha est mis au-dessus de 0% alors que la room etait OFF, on la rallume
                    if (i == 0) { mainAppPtr->roomApp->wallAlpha = 255.0f; if (mainAppPtr->roomApp->bgMode == 1) mainAppPtr->roomApp->bgMode = 0; }
                    else if (i == 1) { mainAppPtr->roomApp->wallAlpha = 191.0f; if (mainAppPtr->roomApp->bgMode == 1) mainAppPtr->roomApp->bgMode = 0; }
                    else if (i == 2) { mainAppPtr->roomApp->wallAlpha = 84.0f; if (mainAppPtr->roomApp->bgMode == 1) mainAppPtr->roomApp->bgMode = 0; }
                    else if (i == 3) { mainAppPtr->roomApp->wallAlpha = 0.0f; } // On ne force plus le mode à 1 (OFF) pour conserver Scene2D en mémoire
                }
                bRoomAlphaAccordionOpen = false;
                return true;
            }
        }
    }

    for(int i=0; i<4; i++) {
        if(bGabAccordionOpen[i]) {
            for(size_t j=0; j<gabOptionRects[i].size(); j++) {
                if(gabOptionRects[i][j].inside(worldM)) {
                    if(mainAppPtr) {
                        if(i == 0) mainAppPtr->gabMode = j;
                        else if(i == 1 && mainAppPtr->roomApp) {
                            if(j == 2 && mainAppPtr->scene2D && mainAppPtr->scene2D->bgDisplayMode == 2) {
                                mainAppPtr->scene2D->bgDisplayMode = 1; // Force Scene2D à "Transp" pour éviter la recursion
                            }
                            mainAppPtr->roomApp->bgMode = j;
                            mainAppPtr->roomApp->wallAlpha = (j == 1) ? 0.0f : 100.0f;
                        }
                        else if(i == 2 && mainAppPtr->scene2D) {
                            if(j == 2 && mainAppPtr->roomApp && mainAppPtr->roomApp->bgMode == 2) {
                                mainAppPtr->roomApp->bgMode = 0; // Force RoomApp à "ON" pour éviter la recursion
                                mainAppPtr->roomApp->wallAlpha = 100.0f;
                            }
                            mainAppPtr->scene2D->bgDisplayMode = j;
                        }
                        else if(i == 3 && mainAppPtr->scene2D) mainAppPtr->scene2D->overlayMode = j;
                    }
                    bGabAccordionOpen[i] = false;
                    return true;
                }
            }
        }
    }

    for(int i=0; i<4; i++) {
        if(gabBtns[i].inside(worldM)) {
            if(mainAppPtr) {
                bGabAccordionOpen[i] = !bGabAccordionOpen[i];
                for(int k=0; k<4; k++) if(k != i) bGabAccordionOpen[k] = false;
                bRoomAlphaAccordionOpen = false;
            }
            return true;
        }
    }
    
    if (roomAlphaBtn.inside(worldM)) {
        bRoomAlphaAccordionOpen = !bRoomAlphaAccordionOpen;
        for(int k=0; k<4; k++) bGabAccordionOpen[k] = false;
        return true;
    }
    
    if (diffuseRoomBtn.inside(worldM)) {
        if (mainAppPtr) mainAppPtr->bDiffuseRoom = !mainAppPtr->bDiffuseRoom;
        return true;
    }
    if (diffuseScene2DBtn.inside(worldM)) {
        if (mainAppPtr) mainAppPtr->bDiffuseScene2D = !mainAppPtr->bDiffuseScene2D;
        return true;
    }
    return false;
}

string PlaylistWindowControlsUI::getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager) {
    for(int i=0; i<4; i++) if(viewBtns[i].inside(worldM)) return tooltipManager.getTooltipText("V" + ofToString(i+1));
    for(int i=0; i<4; i++) if(moveWinBtns[i].inside(worldM)) return tooltipManager.getTooltipText("->V" + ofToString(i+1));
    for(int i=0; i<4; i++) if(toggleWinBtns[i].inside(worldM)) return tooltipManager.getTooltipText("V" + ofToString(i+1) + " WIN");
    for(int i=0; i<4; i++) if(recWinBtns[i].inside(worldM)) return tooltipManager.getTooltipText("REC V" + ofToString(i+1));
    if(formatBtn.inside(worldM)) return tooltipManager.getTooltipText("FMT_BTN");
    if(qualityBtn.inside(worldM)) return tooltipManager.getTooltipText("QUAL_BTN");
    if(arrangeWinBtn.inside(worldM)) return tooltipManager.getTooltipText("ARRANGE_WINS");
    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(worldM)) return tooltipManager.getTooltipText(wxcvbNames[i]);
        if(focusBtns[i].inside(worldM)) return tooltipManager.getTooltipText(focusNames[i]);
    }
    for(int i=0; i<4; i++) {
        if (bGabAccordionOpen[i]) {
            for (size_t j=0; j<gabOptionRects[i].size(); j++) {
                if (gabOptionRects[i][j].inside(worldM)) return "Definit le GAB " + ofToString(i) + " sur " + gabOptions[i][j];
            }
        }
        if(gabBtns[i].inside(worldM)) return tooltipManager.getTooltipText("GAB " + ofToString(i));
    }
    if (bRoomAlphaAccordionOpen) {
        for (size_t i = 0; i < roomAlphaOptionRects.size(); i++) {
            if (roomAlphaOptionRects[i].inside(worldM)) return "Definit l'opacite de la Room a " + roomAlphaOptions[i];
        }
    }
    if(roomAlphaBtn.inside(worldM)) return "Definit l'opacite des murs de la Room (GAB R)";
    if(diffuseRoomBtn.inside(worldM)) return "Active/Desactive la diffusion de la Room sur le Canvas Master";
    if(diffuseScene2DBtn.inside(worldM)) return "Active/Desactive la diffusion de la Scene 2D sur le Canvas Master";
    return "";
}

void PlaylistWindowControlsUI::saveSettings(ofJson& pt) {
    auto saveR = [&](const string& key, const ofRectangle& r) { pt[key]["x"] = r.x; pt[key]["y"] = r.y; pt[key]["w"] = r.width; pt[key]["h"] = r.height; };
    for(int i=0; i<4; i++) saveR("view_" + ofToString(i), viewBtns[i]);
    for(int i=0; i<4; i++) saveR("moveWin_" + ofToString(i), moveWinBtns[i]);
    for(int i=0; i<4; i++) saveR("toggleWin_" + ofToString(i), toggleWinBtns[i]);
    for(int i=0; i<4; i++) saveR("recWin_" + ofToString(i), recWinBtns[i]);
    saveR("formatBtn", formatBtn);
    saveR("qualityBtn", qualityBtn);
    saveR("arrangeWinBtn", arrangeWinBtn);
    for(int i=0; i<6; i++) { saveR("wxcvb_" + ofToString(i), wxcvbBtns[i]); saveR("focus_" + ofToString(i), focusBtns[i]); }
    for(int i=0; i<4; i++) saveR("gab_" + ofToString(i), gabBtns[i]);
    saveR("roomAlphaBtn", roomAlphaBtn);
    saveR("diffuseRoomBtn", diffuseRoomBtn);
    saveR("diffuseScene2DBtn", diffuseScene2DBtn);
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
    for(int i=0; i<4; i++) {
        string key = "moveWin_" + ofToString(i);
        if(i == 2 && pt.contains("moveV3")) key = "moveV3"; // Compatibilité si tu avais déjà sauvegardé le V3
        if(pt.contains(key)) loadR(key, moveWinBtns[i]);
        else { moveWinBtns[i].x = viewBtns[i].x; moveWinBtns[i].y = viewBtns[i].y - 70; }
    }
    for(int i=0; i<4; i++) {
        string key = "toggleWin_" + ofToString(i);
        if(i == 2 && pt.contains("toggleV3Win")) key = "toggleV3Win"; // Compatibilité si tu avais déjà sauvegardé le V3
        if(pt.contains(key)) loadR(key, toggleWinBtns[i]);
        else { toggleWinBtns[i].x = viewBtns[i].x; toggleWinBtns[i].y = viewBtns[i].y - 35; }
    }
    for(int i=0; i<4; i++) {
        if(pt.contains("recWin_" + ofToString(i))) loadR("recWin_" + ofToString(i), recWinBtns[i]);
        else { recWinBtns[i].x = viewBtns[i].x; recWinBtns[i].y = viewBtns[i].y - 105; }
    }
    if(pt.contains("formatBtn")) loadR("formatBtn", formatBtn); else formatBtn.set(240, viewBtns[0].y - 105, 75, 30);
    if(pt.contains("qualityBtn")) loadR("qualityBtn", qualityBtn); else qualityBtn.set(320, viewBtns[0].y - 105, 75, 30);
    if(pt.contains("arrangeWinBtn")) loadR("arrangeWinBtn", arrangeWinBtn); else arrangeWinBtn.set(400, viewBtns[0].y - 105, 100, 30);

    for(int i=0; i<6; i++) { loadR("wxcvb_" + ofToString(i), wxcvbBtns[i]); loadR("focus_" + ofToString(i), focusBtns[i]); }
    for(int i=0; i<4; i++) loadR("gab_" + ofToString(i), gabBtns[i]);
    if(pt.contains("roomAlphaBtn")) loadR("roomAlphaBtn", roomAlphaBtn); 
    else roomAlphaBtn.set(gabBtns[1].x, gabBtns[1].y + 35, 56, 30);

    if(pt.contains("diffuseRoomBtn")) loadR("diffuseRoomBtn", diffuseRoomBtn);
    else diffuseRoomBtn.set(gabBtns[1].x, gabBtns[1].y + 70, 56, 30);
    
    if(pt.contains("diffuseScene2DBtn")) loadR("diffuseScene2DBtn", diffuseScene2DBtn);
    else diffuseScene2DBtn.set(gabBtns[3].x, gabBtns[3].y + 35, 56, 30);
}

vector<ofRectangle*> PlaylistWindowControlsUI::getInteractableRects() {
    vector<ofRectangle*> rects;
    for(int i=0; i<4; i++) rects.push_back(&viewBtns[i]);
    for(int i=0; i<4; i++) rects.push_back(&moveWinBtns[i]);
    for(int i=0; i<4; i++) rects.push_back(&toggleWinBtns[i]);
    for(int i=0; i<4; i++) rects.push_back(&recWinBtns[i]);
    rects.push_back(&formatBtn);
    rects.push_back(&qualityBtn);
    rects.push_back(&arrangeWinBtn);
    for(int i=0; i<6; i++) { rects.push_back(&wxcvbBtns[i]); rects.push_back(&focusBtns[i]); }
    for(int i=0; i<4; i++) {
        rects.push_back(&gabBtns[i]);
        if (bGabAccordionOpen[i]) {
            for(auto& r : gabOptionRects[i]) rects.push_back(&r);
        }
    }
    rects.push_back(&roomAlphaBtn);
    if (bRoomAlphaAccordionOpen) {
        for(auto& r : roomAlphaOptionRects) rects.push_back(&r);
    }
    rects.push_back(&diffuseRoomBtn);
    rects.push_back(&diffuseScene2DBtn);
    return rects;
}

ofRectangle* PlaylistWindowControlsUI::findButtonAt(ofVec2f pos) {
    for(int i=0; i<4; i++) if(viewBtns[i].inside(pos)) return &viewBtns[i];
    for(int i=0; i<4; i++) if(moveWinBtns[i].inside(pos)) return &moveWinBtns[i];
    for(int i=0; i<4; i++) if(toggleWinBtns[i].inside(pos)) return &toggleWinBtns[i];
    for(int i=0; i<4; i++) if(recWinBtns[i].inside(pos)) return &recWinBtns[i];
    if(formatBtn.inside(pos)) return &formatBtn;
    if(qualityBtn.inside(pos)) return &qualityBtn;
    if(arrangeWinBtn.inside(pos)) return &arrangeWinBtn;
    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(pos)) return &wxcvbBtns[i];
        if(focusBtns[i].inside(pos)) return &focusBtns[i];
    }
    for(int i=0; i<4; i++) {
        if (bGabAccordionOpen[i]) {
            for(auto& r : gabOptionRects[i]) if(r.inside(pos)) return &r;
        }
        if(gabBtns[i].inside(pos)) return &gabBtns[i];
    }
    if (bRoomAlphaAccordionOpen) {
        for(auto& r : roomAlphaOptionRects) if(r.inside(pos)) return &r;
    }
    if (roomAlphaBtn.inside(pos)) return &roomAlphaBtn;
    if (diffuseRoomBtn.inside(pos)) return &diffuseRoomBtn;
    if (diffuseScene2DBtn.inside(pos)) return &diffuseScene2DBtn;
    return nullptr;
}