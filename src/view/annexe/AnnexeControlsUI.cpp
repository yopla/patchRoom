#include "AnnexeControlsUI.h"
#include "ofApp.h"
#include "RoomApp.h"
#include "Scene2D_SIDE.h"
#include "Scene2DZenit.h"
#include "RoomPreview.h"
#include "ButtonApp.h"
#include "AnnexeApp.h"

void AnnexeControlsUI::setup() {
    // Position par défaut dans l'espace "monde" (qui sera zoomable/pannable)
    soloAnnexeBtnRect.set(150, 50, 110, 30);
    saveFrameBtnRect.set(150, 90, 110, 30);
    genAIBtnRect.set(150, 130, 110, 30);
    genSAMBtnRect.set(150, 170, 110, 30);
    rippleBtnRect.set(150, 210, 110, 30);
    recordVideoBtnRect.set(150, 250, 110, 30);
    samControlBtnRect.set(150, 290, 110, 30);
    genDepthAnythingBtnRect.set(150, 330, 110, 30);
}

void AnnexeControlsUI::draw(ofApp* mainAppPtr) {
    ofPushStyle();
    if (bIsSoloActive) ofSetColor(200, 50, 50); else ofSetColor(60, 60, 80);
    ofFill(); ofDrawRectangle(soloAnnexeBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(soloAnnexeBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("SOLO ANNEXE", soloAnnexeBtnRect.x + 8, soloAnnexeBtnRect.y + 20);

    ofSetColor(60, 120, 80);
    ofFill(); ofDrawRectangle(saveFrameBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(saveFrameBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("SAVE FRAME", saveFrameBtnRect.x + 8, saveFrameBtnRect.y + 20);

    ofSetColor(120, 60, 180);
    ofFill(); ofDrawRectangle(genAIBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(genAIBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("GEN AI", genAIBtnRect.x + 8, genAIBtnRect.y + 20);

    ofSetColor(120, 120, 220);
    ofFill(); ofDrawRectangle(genSAMBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(genSAMBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("GEN SAM", genSAMBtnRect.x + 8, genSAMBtnRect.y + 20);

    ofSetColor(140, 80, 200);
    ofFill(); ofDrawRectangle(genDepthAnythingBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(genDepthAnythingBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("DEPTH ANY", genDepthAnythingBtnRect.x + 8, genDepthAnythingBtnRect.y + 20);

    if (mainAppPtr && mainAppPtr->annexeApp) {
        if (mainAppPtr->annexeApp->bRippleEffect) ofSetColor(50, 180, 200);
        else ofSetColor(60, 90, 100);
    } else {
        ofSetColor(60, 90, 100);
    }
    ofFill(); ofDrawRectangle(rippleBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(rippleBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("RIPPLE FX", rippleBtnRect.x + 8, rippleBtnRect.y + 20);

    if (mainAppPtr && mainAppPtr->annexeApp && mainAppPtr->annexeApp->bIsRecording) {
        // Blinking red when recording
        if (sin(ofGetElapsedTimef() * 15.0f) > 0) {
            ofSetColor(255, 50, 50);
        } else {
            ofSetColor(150, 0, 0);
        }
    } else {
        ofSetColor(180, 60, 60);
    }
    ofFill(); ofDrawRectangle(recordVideoBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(recordVideoBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("REC VIDEO", recordVideoBtnRect.x + 8, recordVideoBtnRect.y + 20);

    if (mainAppPtr && mainAppPtr->annexeApp) {
        if (mainAppPtr->annexeApp->bSamControlActive) ofSetColor(100, 180, 100);
        else ofSetColor(80, 120, 80);
    } else {
        ofSetColor(80, 120, 80);
    }
    ofFill(); ofDrawRectangle(samControlBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(samControlBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("SAM CONTROL", samControlBtnRect.x + 8, samControlBtnRect.y + 20);

    ofPopStyle();
}

bool AnnexeControlsUI::mousePressed(ofVec2f worldM, ofApp* mainAppPtr) {
    if (soloAnnexeBtnRect.inside(worldM)) {
        if (!mainAppPtr) return true;

        bIsSoloActive = !bIsSoloActive;

        if (bIsSoloActive) {
            // 1. Sauvegarde des états actuels
            saved_bDrawMain = mainAppPtr->bDrawMain;
            saved_bDrawRoom = mainAppPtr->bDrawRoom;
            saved_bDrawZenit = mainAppPtr->bDrawZenit;
            saved_bDrawScene2D = mainAppPtr->bDrawScene2D;
            saved_bDrawButtons = mainAppPtr->bDrawButtons;
            if (mainAppPtr->roomPreviewApp) {
                saved_bPreviewPaused = mainAppPtr->roomPreviewApp->bPaused;
            } else {
                saved_bPreviewPaused = true;
            }

            // 2. Solo : on coupe tout le reste
            mainAppPtr->bDrawMain = false;
            mainAppPtr->bDrawRoom = false; if (mainAppPtr->roomApp) mainAppPtr->roomApp->setEnabled(false);
            mainAppPtr->bDrawZenit = false; if (mainAppPtr->sceneZenit) mainAppPtr->sceneZenit->setEnabled(false);
            mainAppPtr->bDrawScene2D = false; if (mainAppPtr->scene2D) mainAppPtr->scene2D->setEnabled(false);
            mainAppPtr->bDrawButtons = false; if (mainAppPtr->buttonApp) mainAppPtr->buttonApp->setEnabled(false);
            if (mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->setPaused(true);

        } else {
            // 3. Restauration des états
            mainAppPtr->bDrawMain = saved_bDrawMain;
            mainAppPtr->bDrawRoom = saved_bDrawRoom; if (mainAppPtr->roomApp) mainAppPtr->roomApp->setEnabled(saved_bDrawRoom);
            mainAppPtr->bDrawZenit = saved_bDrawZenit; if (mainAppPtr->sceneZenit) mainAppPtr->sceneZenit->setEnabled(saved_bDrawZenit);
            mainAppPtr->bDrawScene2D = saved_bDrawScene2D; if (mainAppPtr->scene2D) mainAppPtr->scene2D->setEnabled(saved_bDrawScene2D);
            mainAppPtr->bDrawButtons = saved_bDrawButtons; if (mainAppPtr->buttonApp) mainAppPtr->buttonApp->setEnabled(saved_bDrawButtons);
            if (mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->setPaused(saved_bPreviewPaused);
        }
        return true;
    }

    if (saveFrameBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->saveHighResFrame();
        }
        return true;
    }

    if (genAIBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->generateDepthMapAI();
        }
        return true;
    }

    if (genSAMBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->generateSAMCollider();
        }
        return true;
    }

    if (genDepthAnythingBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->generateDepthMapDepthAnything();
        }
        return true;
    }

    if (rippleBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->bRippleEffect = !mainAppPtr->annexeApp->bRippleEffect;
            if (mainAppPtr->annexeApp->bRippleEffect) {
                mainAppPtr->annexeApp->setupRipple();
            }
        }
        return true;
    }

    if (recordVideoBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->toggleRecording();
        }
        return true;
    }

    if (samControlBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->toggleSamControl();
        }
        return true;
    }
    return false;
}

string AnnexeControlsUI::getTooltip(ofVec2f worldM, AnnexeTooltipManager& tooltipManager) {
    if(soloAnnexeBtnRect.inside(worldM)) return tooltipManager.getTooltipText("SOLO ANNEXE");
    if(saveFrameBtnRect.inside(worldM)) return tooltipManager.getTooltipText("SAVE FRAME");
    if(genAIBtnRect.inside(worldM)) return tooltipManager.getTooltipText("GEN AI");
    if(genSAMBtnRect.inside(worldM)) return tooltipManager.getTooltipText("GEN SAM");
    if(genDepthAnythingBtnRect.inside(worldM)) return tooltipManager.getTooltipText("DEPTH ANY");
    if(rippleBtnRect.inside(worldM)) return tooltipManager.getTooltipText("RIPPLE FX");
    if(recordVideoBtnRect.inside(worldM)) return tooltipManager.getTooltipText("RECORD VIDEO");
    if(samControlBtnRect.inside(worldM)) return tooltipManager.getTooltipText("SAM CONTROL");
    return "";
}

void AnnexeControlsUI::saveSettings(ofJson& pt) {
    pt["annexeControlsUI"]["soloBtn"]["x"] = soloAnnexeBtnRect.x; pt["annexeControlsUI"]["soloBtn"]["y"] = soloAnnexeBtnRect.y;
    pt["annexeControlsUI"]["soloBtn"]["w"] = soloAnnexeBtnRect.width; pt["annexeControlsUI"]["soloBtn"]["h"] = soloAnnexeBtnRect.height;
    pt["annexeControlsUI"]["saveFrameBtn"]["x"] = saveFrameBtnRect.x; pt["annexeControlsUI"]["saveFrameBtn"]["y"] = saveFrameBtnRect.y;
    pt["annexeControlsUI"]["saveFrameBtn"]["w"] = saveFrameBtnRect.width; pt["annexeControlsUI"]["saveFrameBtn"]["h"] = saveFrameBtnRect.height;
    pt["annexeControlsUI"]["genAIBtn"]["x"] = genAIBtnRect.x; pt["annexeControlsUI"]["genAIBtn"]["y"] = genAIBtnRect.y;
    pt["annexeControlsUI"]["genAIBtn"]["w"] = genAIBtnRect.width; pt["annexeControlsUI"]["genAIBtn"]["h"] = genAIBtnRect.height;
    pt["annexeControlsUI"]["genSAMBtn"]["x"] = genSAMBtnRect.x; pt["annexeControlsUI"]["genSAMBtn"]["y"] = genSAMBtnRect.y;
    pt["annexeControlsUI"]["genSAMBtn"]["w"] = genSAMBtnRect.width; pt["annexeControlsUI"]["genSAMBtn"]["h"] = genSAMBtnRect.height;
    pt["annexeControlsUI"]["genDepthAnythingBtn"]["x"] = genDepthAnythingBtnRect.x; pt["annexeControlsUI"]["genDepthAnythingBtn"]["y"] = genDepthAnythingBtnRect.y;
    pt["annexeControlsUI"]["genDepthAnythingBtn"]["w"] = genDepthAnythingBtnRect.width; pt["annexeControlsUI"]["genDepthAnythingBtn"]["h"] = genDepthAnythingBtnRect.height;
    pt["annexeControlsUI"]["rippleBtn"]["x"] = rippleBtnRect.x; pt["annexeControlsUI"]["rippleBtn"]["y"] = rippleBtnRect.y;
    pt["annexeControlsUI"]["rippleBtn"]["w"] = rippleBtnRect.width; pt["annexeControlsUI"]["rippleBtn"]["h"] = rippleBtnRect.height;
    pt["annexeControlsUI"]["recordVideoBtn"]["x"] = recordVideoBtnRect.x; pt["annexeControlsUI"]["recordVideoBtn"]["y"] = recordVideoBtnRect.y;
    pt["annexeControlsUI"]["recordVideoBtn"]["w"] = recordVideoBtnRect.width; pt["annexeControlsUI"]["recordVideoBtn"]["h"] = recordVideoBtnRect.height;
    pt["annexeControlsUI"]["samControlBtn"]["x"] = samControlBtnRect.x; pt["annexeControlsUI"]["samControlBtn"]["y"] = samControlBtnRect.y;
    pt["annexeControlsUI"]["samControlBtn"]["w"] = samControlBtnRect.width; pt["annexeControlsUI"]["samControlBtn"]["h"] = samControlBtnRect.height;
}

void AnnexeControlsUI::loadSettings(const ofJson& pt) {
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("soloBtn")) {
        soloAnnexeBtnRect.x = pt["annexeControlsUI"]["soloBtn"].value("x", soloAnnexeBtnRect.x);
        soloAnnexeBtnRect.y = pt["annexeControlsUI"]["soloBtn"].value("y", soloAnnexeBtnRect.y);
        soloAnnexeBtnRect.width = pt["annexeControlsUI"]["soloBtn"].value("w", soloAnnexeBtnRect.width);
        soloAnnexeBtnRect.height = pt["annexeControlsUI"]["soloBtn"].value("h", soloAnnexeBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("saveFrameBtn")) {
        saveFrameBtnRect.x = pt["annexeControlsUI"]["saveFrameBtn"].value("x", saveFrameBtnRect.x);
        saveFrameBtnRect.y = pt["annexeControlsUI"]["saveFrameBtn"].value("y", saveFrameBtnRect.y);
        saveFrameBtnRect.width = pt["annexeControlsUI"]["saveFrameBtn"].value("w", saveFrameBtnRect.width);
        saveFrameBtnRect.height = pt["annexeControlsUI"]["saveFrameBtn"].value("h", saveFrameBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("genAIBtn")) {
        genAIBtnRect.x = pt["annexeControlsUI"]["genAIBtn"].value("x", genAIBtnRect.x);
        genAIBtnRect.y = pt["annexeControlsUI"]["genAIBtn"].value("y", genAIBtnRect.y);
        genAIBtnRect.width = pt["annexeControlsUI"]["genAIBtn"].value("w", genAIBtnRect.width);
        genAIBtnRect.height = pt["annexeControlsUI"]["genAIBtn"].value("h", genAIBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("genSAMBtn")) {
        genSAMBtnRect.x = pt["annexeControlsUI"]["genSAMBtn"].value("x", genSAMBtnRect.x);
        genSAMBtnRect.y = pt["annexeControlsUI"]["genSAMBtn"].value("y", genSAMBtnRect.y);
        genSAMBtnRect.width = pt["annexeControlsUI"]["genSAMBtn"].value("w", genSAMBtnRect.width);
        genSAMBtnRect.height = pt["annexeControlsUI"]["genSAMBtn"].value("h", genSAMBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("genDepthAnythingBtn")) {
        genDepthAnythingBtnRect.x = pt["annexeControlsUI"]["genDepthAnythingBtn"].value("x", genDepthAnythingBtnRect.x);
        genDepthAnythingBtnRect.y = pt["annexeControlsUI"]["genDepthAnythingBtn"].value("y", genDepthAnythingBtnRect.y);
        genDepthAnythingBtnRect.width = pt["annexeControlsUI"]["genDepthAnythingBtn"].value("w", genDepthAnythingBtnRect.width);
        genDepthAnythingBtnRect.height = pt["annexeControlsUI"]["genDepthAnythingBtn"].value("h", genDepthAnythingBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("rippleBtn")) {
        rippleBtnRect.x = pt["annexeControlsUI"]["rippleBtn"].value("x", rippleBtnRect.x);
        rippleBtnRect.y = pt["annexeControlsUI"]["rippleBtn"].value("y", rippleBtnRect.y);
        rippleBtnRect.width = pt["annexeControlsUI"]["rippleBtn"].value("w", rippleBtnRect.width);
        rippleBtnRect.height = pt["annexeControlsUI"]["rippleBtn"].value("h", rippleBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("recordVideoBtn")) {
        recordVideoBtnRect.x = pt["annexeControlsUI"]["recordVideoBtn"].value("x", recordVideoBtnRect.x);
        recordVideoBtnRect.y = pt["annexeControlsUI"]["recordVideoBtn"].value("y", recordVideoBtnRect.y);
        recordVideoBtnRect.width = pt["annexeControlsUI"]["recordVideoBtn"].value("w", recordVideoBtnRect.width);
        recordVideoBtnRect.height = pt["annexeControlsUI"]["recordVideoBtn"].value("h", recordVideoBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("samControlBtn")) {
        samControlBtnRect.x = pt["annexeControlsUI"]["samControlBtn"].value("x", samControlBtnRect.x);
        samControlBtnRect.y = pt["annexeControlsUI"]["samControlBtn"].value("y", samControlBtnRect.y);
        samControlBtnRect.width = pt["annexeControlsUI"]["samControlBtn"].value("w", samControlBtnRect.width);
        samControlBtnRect.height = pt["annexeControlsUI"]["samControlBtn"].value("h", samControlBtnRect.height);
    }
}

vector<ofRectangle*> AnnexeControlsUI::getInteractableRects() { return { &soloAnnexeBtnRect, &saveFrameBtnRect, &genAIBtnRect, &genSAMBtnRect, &genDepthAnythingBtnRect, &rippleBtnRect, &recordVideoBtnRect, &samControlBtnRect }; }
ofRectangle* AnnexeControlsUI::findButtonAt(ofVec2f pos) { 
    if (soloAnnexeBtnRect.inside(pos)) return &soloAnnexeBtnRect; 
    if (saveFrameBtnRect.inside(pos)) return &saveFrameBtnRect; 
    if (genAIBtnRect.inside(pos)) return &genAIBtnRect; 
    if (genSAMBtnRect.inside(pos)) return &genSAMBtnRect;
    if (genDepthAnythingBtnRect.inside(pos)) return &genDepthAnythingBtnRect;
    if (rippleBtnRect.inside(pos)) return &rippleBtnRect;
    if (recordVideoBtnRect.inside(pos)) return &recordVideoBtnRect;
    if (samControlBtnRect.inside(pos)) return &samControlBtnRect;
    return nullptr; 
}