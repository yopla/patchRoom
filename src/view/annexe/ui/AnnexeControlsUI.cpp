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
    
    layerVolumBtnRect.set(150, 370, 110, 30);
    depthMapBtnRect.set(150, 410, 110, 30);
    resetDepthMapBtnRect.set(150, 450, 110, 30);
    
    rotUpBtnRect.set(185, 490, 40, 30);
    rotLeftBtnRect.set(140, 525, 40, 30);
    resetRotBtnRect.set(185, 525, 40, 30);
    rotRightBtnRect.set(230, 525, 40, 30);
    rotDownBtnRect.set(185, 560, 40, 30);
    
    patteuBtnRect.set(150, 600, 110, 30);
    patteuDropRect.set(270, 600, 60, 30);
    patteuResetBtnRect.set(150, 640, 110, 30);
    patteuIntensitySliderRect.set(150, 680, 110, 15);
    patteuHardnessSliderRect.set(150, 705, 110, 15);
    patteuBrushSizeSliderRect.set(150, 730, 110, 15);
    
    deuPatteuBtnRect.set(150, 765, 110, 30);
    deuPatteuDropFgRect.set(270, 765, 60, 30);
    deuPatteuDropBgRect.set(340, 765, 60, 30);
    deuPatteuResetFgBtnRect.set(150, 805, 110, 30);
    deuPatteuResetBgBtnRect.set(270, 805, 110, 30);
    deuPatteuIntensitySliderRect.set(150, 845, 110, 15);
    deuPatteuHardnessSliderRect.set(150, 870, 110, 15);
    deuPatteuRevealSliderRect.set(150, 895, 110, 15);
    deuPatteuBrushSizeSliderRect.set(150, 920, 110, 15);
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
        if (mainAppPtr->annexeApp->rippleController.bActive) ofSetColor(50, 180, 200);
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
    
    if (mainAppPtr && mainAppPtr->annexeApp && mainAppPtr->annexeApp->volumManager.bLayerVolumActive) ofSetColor(100, 180, 100);
    else ofSetColor(80, 120, 80);
    ofFill(); ofDrawRectangle(layerVolumBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(layerVolumBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("LAYER VOLUM", layerVolumBtnRect.x + 8, layerVolumBtnRect.y + 20);

    if (mainAppPtr && mainAppPtr->annexeApp && mainAppPtr->annexeApp->volumManager.bDepthMapActive) ofSetColor(100, 180, 100);
    else ofSetColor(80, 120, 80);
    ofFill(); ofDrawRectangle(depthMapBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(depthMapBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("DEPTH MAP", depthMapBtnRect.x + 8, depthMapBtnRect.y + 20);

    ofSetColor(150, 50, 50);
    ofFill(); ofDrawRectangle(resetDepthMapBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(resetDepthMapBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("RESET DEPTH", resetDepthMapBtnRect.x + 8, resetDepthMapBtnRect.y + 20);

    ofSetColor(60, 60, 80);
    ofFill(); ofDrawRectangle(rotUpBtnRect); ofDrawRectangle(rotDownBtnRect); ofDrawRectangle(rotLeftBtnRect); ofDrawRectangle(rotRightBtnRect); ofDrawRectangle(resetRotBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(rotUpBtnRect); ofDrawRectangle(rotDownBtnRect); ofDrawRectangle(rotLeftBtnRect); ofDrawRectangle(rotRightBtnRect); ofDrawRectangle(resetRotBtnRect);
    ofSetColor(255);
    ofDrawBitmapString("UP", rotUpBtnRect.x + 12, rotUpBtnRect.y + 20);
    ofDrawBitmapString("DWN", rotDownBtnRect.x + 8, rotDownBtnRect.y + 20);
    ofDrawBitmapString("LFT", rotLeftBtnRect.x + 8, rotLeftBtnRect.y + 20);
    ofDrawBitmapString("RGT", rotRightBtnRect.x + 8, rotRightBtnRect.y + 20);
    ofDrawBitmapString(" 0", resetRotBtnRect.x + 8, resetRotBtnRect.y + 20);
    
    // ---- PATTEU UI ----
    if (mainAppPtr && mainAppPtr->annexeApp && mainAppPtr->annexeApp->patteuLayer.bActive) ofSetColor(100, 180, 100);
    else ofSetColor(80, 120, 80);
    ofFill(); ofDrawRectangle(patteuBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(patteuBtnRect);
    ofSetColor(255); ofDrawBitmapString("LAYER PATTEU", patteuBtnRect.x + 8, patteuBtnRect.y + 20);

    ofSetColor(150, 100, 50); ofFill(); ofDrawRectangle(patteuDropRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(patteuDropRect);
    ofSetColor(255); ofDrawBitmapString("DROP", patteuDropRect.x + 12, patteuDropRect.y + 20);

    ofSetColor(180, 60, 60); ofFill(); ofDrawRectangle(patteuResetBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(patteuResetBtnRect);
    ofSetColor(255); ofDrawBitmapString("RESET PATTEU", patteuResetBtnRect.x + 8, patteuResetBtnRect.y + 20);

    if (mainAppPtr && mainAppPtr->annexeApp) {
        float intensity = mainAppPtr->annexeApp->patteuLayer.intensity;
        float hardness = mainAppPtr->annexeApp->patteuLayer.hardness;
        float pNormBrush = ofMap(mainAppPtr->annexeApp->patteuLayer.brushSize, 5.0f, 500.0f, 0.0f, 1.0f, true);
        ofSetColor(50); ofFill(); ofDrawRectangle(patteuIntensitySliderRect); ofSetColor(150, 150, 255); ofDrawRectangle(patteuIntensitySliderRect.x, patteuIntensitySliderRect.y, patteuIntensitySliderRect.width * intensity, patteuIntensitySliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(patteuIntensitySliderRect); ofDrawBitmapString("Intensite", patteuIntensitySliderRect.x + 5, patteuIntensitySliderRect.y + 12);
        ofSetColor(50); ofFill(); ofDrawRectangle(patteuHardnessSliderRect); ofSetColor(255, 150, 150); ofDrawRectangle(patteuHardnessSliderRect.x, patteuHardnessSliderRect.y, patteuHardnessSliderRect.width * hardness, patteuHardnessSliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(patteuHardnessSliderRect); ofDrawBitmapString("Durete", patteuHardnessSliderRect.x + 5, patteuHardnessSliderRect.y + 12);
        ofSetColor(50); ofFill(); ofDrawRectangle(patteuBrushSizeSliderRect); ofSetColor(200, 200, 150); ofDrawRectangle(patteuBrushSizeSliderRect.x, patteuBrushSizeSliderRect.y, patteuBrushSizeSliderRect.width * pNormBrush, patteuBrushSizeSliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(patteuBrushSizeSliderRect); ofDrawBitmapString("Taille", patteuBrushSizeSliderRect.x + 5, patteuBrushSizeSliderRect.y + 12);
    }

    // ---- DEU PATTEU UI ----
    if (mainAppPtr && mainAppPtr->annexeApp && mainAppPtr->annexeApp->deuPatteuLayer.bActive) ofSetColor(180, 100, 200);
    else ofSetColor(120, 80, 140);
    ofFill(); ofDrawRectangle(deuPatteuBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuBtnRect);
    ofSetColor(255); ofDrawBitmapString("DEU PATTEU", deuPatteuBtnRect.x + 8, deuPatteuBtnRect.y + 20);

    ofSetColor(50, 150, 100); ofFill(); ofDrawRectangle(deuPatteuDropFgRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuDropFgRect);
    ofSetColor(255); ofDrawBitmapString("FG", deuPatteuDropFgRect.x + 12, deuPatteuDropFgRect.y + 20);

    ofSetColor(50, 100, 150); ofFill(); ofDrawRectangle(deuPatteuDropBgRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuDropBgRect);
    ofSetColor(255); ofDrawBitmapString("BG", deuPatteuDropBgRect.x + 12, deuPatteuDropBgRect.y + 20);

    ofSetColor(180, 60, 60); ofFill(); ofDrawRectangle(deuPatteuResetFgBtnRect); ofDrawRectangle(deuPatteuResetBgBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuResetFgBtnRect); ofDrawRectangle(deuPatteuResetBgBtnRect);
    ofSetColor(255); ofDrawBitmapString("RESET FG", deuPatteuResetFgBtnRect.x + 15, deuPatteuResetFgBtnRect.y + 20);
    ofDrawBitmapString("RESET BG", deuPatteuResetBgBtnRect.x + 15, deuPatteuResetBgBtnRect.y + 20);

    if (mainAppPtr && mainAppPtr->annexeApp) {
        float intensity = mainAppPtr->annexeApp->deuPatteuLayer.intensity;
        float hardness = mainAppPtr->annexeApp->deuPatteuLayer.hardness;
        float reveal = mainAppPtr->annexeApp->deuPatteuLayer.revealSpeed;
        float dNormBrush = ofMap(mainAppPtr->annexeApp->deuPatteuLayer.brushSize, 5.0f, 500.0f, 0.0f, 1.0f, true);
        ofSetColor(50); ofFill(); ofDrawRectangle(deuPatteuIntensitySliderRect); ofSetColor(150, 150, 255); ofDrawRectangle(deuPatteuIntensitySliderRect.x, deuPatteuIntensitySliderRect.y, deuPatteuIntensitySliderRect.width * intensity, deuPatteuIntensitySliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuIntensitySliderRect); ofDrawBitmapString("Intensite", deuPatteuIntensitySliderRect.x + 5, deuPatteuIntensitySliderRect.y + 12);
        ofSetColor(50); ofFill(); ofDrawRectangle(deuPatteuHardnessSliderRect); ofSetColor(255, 150, 150); ofDrawRectangle(deuPatteuHardnessSliderRect.x, deuPatteuHardnessSliderRect.y, deuPatteuHardnessSliderRect.width * hardness, deuPatteuHardnessSliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuHardnessSliderRect); ofDrawBitmapString("Durete", deuPatteuHardnessSliderRect.x + 5, deuPatteuHardnessSliderRect.y + 12);
        ofSetColor(50); ofFill(); ofDrawRectangle(deuPatteuRevealSliderRect); ofSetColor(150, 255, 150); ofDrawRectangle(deuPatteuRevealSliderRect.x, deuPatteuRevealSliderRect.y, deuPatteuRevealSliderRect.width * reveal, deuPatteuRevealSliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuRevealSliderRect); ofDrawBitmapString("Reveal", deuPatteuRevealSliderRect.x + 5, deuPatteuRevealSliderRect.y + 12);
        ofSetColor(50); ofFill(); ofDrawRectangle(deuPatteuBrushSizeSliderRect); ofSetColor(200, 200, 150); ofDrawRectangle(deuPatteuBrushSizeSliderRect.x, deuPatteuBrushSizeSliderRect.y, deuPatteuBrushSizeSliderRect.width * dNormBrush, deuPatteuBrushSizeSliderRect.height); ofNoFill(); ofSetColor(255); ofDrawRectangle(deuPatteuBrushSizeSliderRect); ofDrawBitmapString("Taille", deuPatteuBrushSizeSliderRect.x + 5, deuPatteuBrushSizeSliderRect.y + 12);
    }

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
            mainAppPtr->annexeApp->rippleController.bActive = !mainAppPtr->annexeApp->rippleController.bActive;
            if (mainAppPtr->annexeApp->rippleController.bActive) {
                mainAppPtr->annexeApp->rippleController.setup(mainAppPtr->annexeApp->img);
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
    
    if (layerVolumBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->volumManager.bLayerVolumActive = !mainAppPtr->annexeApp->volumManager.bLayerVolumActive;
        }
        return true;
    }
    
    if (depthMapBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->volumManager.bDepthMapActive = !mainAppPtr->annexeApp->volumManager.bDepthMapActive;
            if (mainAppPtr->annexeApp->volumManager.bDepthMapActive) {
                mainAppPtr->annexeApp->volumManager.bLayerVolumActive = true;
            }
        }
        return true;
    }
    
    if (resetDepthMapBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) mainAppPtr->annexeApp->resetDepthMap();
        return true;
    }
    
    if (resetRotBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->volumManager.rotX = 0;
            mainAppPtr->annexeApp->volumManager.rotY = 0;
        }
        return true;
    }
    
    if (rotUpBtnRect.inside(worldM) || rotDownBtnRect.inside(worldM) || rotLeftBtnRect.inside(worldM) || rotRightBtnRect.inside(worldM)) {
        return true; // Intercepte le clic pour éviter que le canvas ne déclenche le "Drag Pan"
    }
    
    if (patteuBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) {
            mainAppPtr->annexeApp->patteuLayer.bActive = !mainAppPtr->annexeApp->patteuLayer.bActive;
        }
        return true;
    }
    if (patteuResetBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) mainAppPtr->annexeApp->patteuLayer.reset();
        return true;
    }
    if (patteuDropRect.inside(worldM) || patteuIntensitySliderRect.inside(worldM) || patteuHardnessSliderRect.inside(worldM) || patteuBrushSizeSliderRect.inside(worldM)) {
        return true; // Consommer les clics pour éviter le pan
    }
    
    if (deuPatteuBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) mainAppPtr->annexeApp->deuPatteuLayer.bActive = !mainAppPtr->annexeApp->deuPatteuLayer.bActive;
        return true;
    }
    if (deuPatteuResetFgBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) mainAppPtr->annexeApp->deuPatteuLayer.resetFg();
        return true;
    }
    if (deuPatteuResetBgBtnRect.inside(worldM)) {
        if (mainAppPtr && mainAppPtr->annexeApp) mainAppPtr->annexeApp->deuPatteuLayer.resetBg();
        return true;
    }
    if (deuPatteuDropFgRect.inside(worldM) || deuPatteuDropBgRect.inside(worldM) || 
        deuPatteuIntensitySliderRect.inside(worldM) || deuPatteuHardnessSliderRect.inside(worldM) || 
        deuPatteuRevealSliderRect.inside(worldM) || deuPatteuBrushSizeSliderRect.inside(worldM)) {
        return true;
    }
    
    return false;
}

void AnnexeControlsUI::handleContinuousActions(ofVec2f worldM, ofApp* mainAppPtr) {
    if (!mainAppPtr || !mainAppPtr->annexeApp) return;
    if (rotUpBtnRect.inside(worldM)) mainAppPtr->annexeApp->volumManager.rotX -= mainAppPtr->annexeApp->volumManager.rotSpeed;
    if (rotDownBtnRect.inside(worldM)) mainAppPtr->annexeApp->volumManager.rotX += mainAppPtr->annexeApp->volumManager.rotSpeed;
    if (rotLeftBtnRect.inside(worldM)) mainAppPtr->annexeApp->volumManager.rotY -= mainAppPtr->annexeApp->volumManager.rotSpeed;
    if (rotRightBtnRect.inside(worldM)) mainAppPtr->annexeApp->volumManager.rotY += mainAppPtr->annexeApp->volumManager.rotSpeed;
    
    if (patteuIntensitySliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->patteuLayer.intensity = ofMap(worldM.x, patteuIntensitySliderRect.x, patteuIntensitySliderRect.x + patteuIntensitySliderRect.width, 0.0f, 1.0f, true);
    }
    if (patteuHardnessSliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->patteuLayer.hardness = ofMap(worldM.x, patteuHardnessSliderRect.x, patteuHardnessSliderRect.x + patteuHardnessSliderRect.width, 0.0f, 1.0f, true);
    }
    if (patteuBrushSizeSliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->patteuLayer.brushSize = ofMap(worldM.x, patteuBrushSizeSliderRect.x, patteuBrushSizeSliderRect.x + patteuBrushSizeSliderRect.width, 5.0f, 500.0f, true);
    }
    
    if (deuPatteuIntensitySliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->deuPatteuLayer.intensity = ofMap(worldM.x, deuPatteuIntensitySliderRect.x, deuPatteuIntensitySliderRect.x + deuPatteuIntensitySliderRect.width, 0.0f, 1.0f, true);
    }
    if (deuPatteuHardnessSliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->deuPatteuLayer.hardness = ofMap(worldM.x, deuPatteuHardnessSliderRect.x, deuPatteuHardnessSliderRect.x + deuPatteuHardnessSliderRect.width, 0.0f, 1.0f, true);
    }
    if (deuPatteuRevealSliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->deuPatteuLayer.revealSpeed = ofMap(worldM.x, deuPatteuRevealSliderRect.x, deuPatteuRevealSliderRect.x + deuPatteuRevealSliderRect.width, 0.0f, 1.0f, true);
    }
    if (deuPatteuBrushSizeSliderRect.inside(worldM)) {
        mainAppPtr->annexeApp->deuPatteuLayer.brushSize = ofMap(worldM.x, deuPatteuBrushSizeSliderRect.x, deuPatteuBrushSizeSliderRect.x + deuPatteuBrushSizeSliderRect.width, 5.0f, 500.0f, true);
    }
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
    if(layerVolumBtnRect.inside(worldM)) return tooltipManager.getTooltipText("LAYER VOLUM");
    if(depthMapBtnRect.inside(worldM)) return tooltipManager.getTooltipText("DEPTH MAP");
    if(resetDepthMapBtnRect.inside(worldM)) return tooltipManager.getTooltipText("RESET DEPTH");
    if(rotUpBtnRect.inside(worldM) || rotDownBtnRect.inside(worldM) || rotLeftBtnRect.inside(worldM) || rotRightBtnRect.inside(worldM)) return tooltipManager.getTooltipText("ROTATION");
    if(resetRotBtnRect.inside(worldM)) return tooltipManager.getTooltipText("RESET ROT");
    if(patteuBtnRect.inside(worldM)) return tooltipManager.getTooltipText("PATTEU BTN");
    if(patteuDropRect.inside(worldM)) return tooltipManager.getTooltipText("PATTEU DROP");
    if(patteuResetBtnRect.inside(worldM)) return tooltipManager.getTooltipText("PATTEU RESET");
    if(patteuBrushSizeSliderRect.inside(worldM)) return tooltipManager.getTooltipText("PATTEU TAILLE");
    
    if(deuPatteuBtnRect.inside(worldM)) return tooltipManager.getTooltipText("DEUPATTEU BTN");
    if(deuPatteuDropFgRect.inside(worldM)) return tooltipManager.getTooltipText("DEUPATTEU DROP FG");
    if(deuPatteuDropBgRect.inside(worldM)) return tooltipManager.getTooltipText("DEUPATTEU DROP BG");
    if(deuPatteuResetFgBtnRect.inside(worldM)) return tooltipManager.getTooltipText("DEUPATTEU RESET FG");
    if(deuPatteuResetBgBtnRect.inside(worldM)) return tooltipManager.getTooltipText("DEUPATTEU RESET BG");
    if(deuPatteuBrushSizeSliderRect.inside(worldM)) return tooltipManager.getTooltipText("DEUPATTEU TAILLE");
    
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
    
    pt["annexeControlsUI"]["layerVolumBtn"]["x"] = layerVolumBtnRect.x; pt["annexeControlsUI"]["layerVolumBtn"]["y"] = layerVolumBtnRect.y;
    pt["annexeControlsUI"]["layerVolumBtn"]["w"] = layerVolumBtnRect.width; pt["annexeControlsUI"]["layerVolumBtn"]["h"] = layerVolumBtnRect.height;
    pt["annexeControlsUI"]["depthMapBtn"]["x"] = depthMapBtnRect.x; pt["annexeControlsUI"]["depthMapBtn"]["y"] = depthMapBtnRect.y;
    pt["annexeControlsUI"]["depthMapBtn"]["w"] = depthMapBtnRect.width; pt["annexeControlsUI"]["depthMapBtn"]["h"] = depthMapBtnRect.height;
    pt["annexeControlsUI"]["resetDepthMapBtn"]["x"] = resetDepthMapBtnRect.x; pt["annexeControlsUI"]["resetDepthMapBtn"]["y"] = resetDepthMapBtnRect.y;
    pt["annexeControlsUI"]["resetDepthMapBtn"]["w"] = resetDepthMapBtnRect.width; pt["annexeControlsUI"]["resetDepthMapBtn"]["h"] = resetDepthMapBtnRect.height;
    pt["annexeControlsUI"]["rotUpBtn"]["x"] = rotUpBtnRect.x; pt["annexeControlsUI"]["rotUpBtn"]["y"] = rotUpBtnRect.y;
    pt["annexeControlsUI"]["rotDownBtn"]["x"] = rotDownBtnRect.x; pt["annexeControlsUI"]["rotDownBtn"]["y"] = rotDownBtnRect.y;
    pt["annexeControlsUI"]["rotLeftBtn"]["x"] = rotLeftBtnRect.x; pt["annexeControlsUI"]["rotLeftBtn"]["y"] = rotLeftBtnRect.y;
    pt["annexeControlsUI"]["rotRightBtn"]["x"] = rotRightBtnRect.x; pt["annexeControlsUI"]["rotRightBtn"]["y"] = rotRightBtnRect.y;
    pt["annexeControlsUI"]["resetRotBtn"]["x"] = resetRotBtnRect.x; pt["annexeControlsUI"]["resetRotBtn"]["y"] = resetRotBtnRect.y;
    pt["annexeControlsUI"]["resetRotBtn"]["w"] = resetRotBtnRect.width; pt["annexeControlsUI"]["resetRotBtn"]["h"] = resetRotBtnRect.height;
    
    pt["annexeControlsUI"]["patteuBtn"]["x"] = patteuBtnRect.x; pt["annexeControlsUI"]["patteuBtn"]["y"] = patteuBtnRect.y;
    pt["annexeControlsUI"]["patteuDrop"]["x"] = patteuDropRect.x; pt["annexeControlsUI"]["patteuDrop"]["y"] = patteuDropRect.y;
    pt["annexeControlsUI"]["patteuResetBtn"]["x"] = patteuResetBtnRect.x; pt["annexeControlsUI"]["patteuResetBtn"]["y"] = patteuResetBtnRect.y;
    pt["annexeControlsUI"]["patteuIntensity"]["x"] = patteuIntensitySliderRect.x; pt["annexeControlsUI"]["patteuIntensity"]["y"] = patteuIntensitySliderRect.y;
    pt["annexeControlsUI"]["patteuHardness"]["x"] = patteuHardnessSliderRect.x; pt["annexeControlsUI"]["patteuHardness"]["y"] = patteuHardnessSliderRect.y;
    pt["annexeControlsUI"]["patteuBrushSize"]["x"] = patteuBrushSizeSliderRect.x; pt["annexeControlsUI"]["patteuBrushSize"]["y"] = patteuBrushSizeSliderRect.y;
    
    pt["annexeControlsUI"]["deuPatteuBtn"]["x"] = deuPatteuBtnRect.x; pt["annexeControlsUI"]["deuPatteuBtn"]["y"] = deuPatteuBtnRect.y;
    pt["annexeControlsUI"]["deuPatteuDropFg"]["x"] = deuPatteuDropFgRect.x; pt["annexeControlsUI"]["deuPatteuDropFg"]["y"] = deuPatteuDropFgRect.y;
    pt["annexeControlsUI"]["deuPatteuDropBg"]["x"] = deuPatteuDropBgRect.x; pt["annexeControlsUI"]["deuPatteuDropBg"]["y"] = deuPatteuDropBgRect.y;
    pt["annexeControlsUI"]["deuPatteuResetFg"]["x"] = deuPatteuResetFgBtnRect.x; pt["annexeControlsUI"]["deuPatteuResetFg"]["y"] = deuPatteuResetFgBtnRect.y;
    pt["annexeControlsUI"]["deuPatteuResetBg"]["x"] = deuPatteuResetBgBtnRect.x; pt["annexeControlsUI"]["deuPatteuResetBg"]["y"] = deuPatteuResetBgBtnRect.y;
    pt["annexeControlsUI"]["deuPatteuIntensity"]["x"] = deuPatteuIntensitySliderRect.x; pt["annexeControlsUI"]["deuPatteuIntensity"]["y"] = deuPatteuIntensitySliderRect.y;
    pt["annexeControlsUI"]["deuPatteuHardness"]["x"] = deuPatteuHardnessSliderRect.x; pt["annexeControlsUI"]["deuPatteuHardness"]["y"] = deuPatteuHardnessSliderRect.y;
    pt["annexeControlsUI"]["deuPatteuReveal"]["x"] = deuPatteuRevealSliderRect.x; pt["annexeControlsUI"]["deuPatteuReveal"]["y"] = deuPatteuRevealSliderRect.y;
    pt["annexeControlsUI"]["deuPatteuBrushSize"]["x"] = deuPatteuBrushSizeSliderRect.x; pt["annexeControlsUI"]["deuPatteuBrushSize"]["y"] = deuPatteuBrushSizeSliderRect.y;
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
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("layerVolumBtn")) {
        layerVolumBtnRect.x = pt["annexeControlsUI"]["layerVolumBtn"].value("x", layerVolumBtnRect.x);
        layerVolumBtnRect.y = pt["annexeControlsUI"]["layerVolumBtn"].value("y", layerVolumBtnRect.y);
        layerVolumBtnRect.width = pt["annexeControlsUI"]["layerVolumBtn"].value("w", layerVolumBtnRect.width);
        layerVolumBtnRect.height = pt["annexeControlsUI"]["layerVolumBtn"].value("h", layerVolumBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("depthMapBtn")) {
        depthMapBtnRect.x = pt["annexeControlsUI"]["depthMapBtn"].value("x", depthMapBtnRect.x);
        depthMapBtnRect.y = pt["annexeControlsUI"]["depthMapBtn"].value("y", depthMapBtnRect.y);
        depthMapBtnRect.width = pt["annexeControlsUI"]["depthMapBtn"].value("w", depthMapBtnRect.width);
        depthMapBtnRect.height = pt["annexeControlsUI"]["depthMapBtn"].value("h", depthMapBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("resetDepthMapBtn")) {
        resetDepthMapBtnRect.x = pt["annexeControlsUI"]["resetDepthMapBtn"].value("x", resetDepthMapBtnRect.x);
        resetDepthMapBtnRect.y = pt["annexeControlsUI"]["resetDepthMapBtn"].value("y", resetDepthMapBtnRect.y);
        resetDepthMapBtnRect.width = pt["annexeControlsUI"]["resetDepthMapBtn"].value("w", resetDepthMapBtnRect.width);
        resetDepthMapBtnRect.height = pt["annexeControlsUI"]["resetDepthMapBtn"].value("h", resetDepthMapBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("rotUpBtn")) {
        rotUpBtnRect.x = pt["annexeControlsUI"]["rotUpBtn"].value("x", rotUpBtnRect.x); rotUpBtnRect.y = pt["annexeControlsUI"]["rotUpBtn"].value("y", rotUpBtnRect.y);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("rotDownBtn")) {
        rotDownBtnRect.x = pt["annexeControlsUI"]["rotDownBtn"].value("x", rotDownBtnRect.x); rotDownBtnRect.y = pt["annexeControlsUI"]["rotDownBtn"].value("y", rotDownBtnRect.y);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("rotLeftBtn")) {
        rotLeftBtnRect.x = pt["annexeControlsUI"]["rotLeftBtn"].value("x", rotLeftBtnRect.x); rotLeftBtnRect.y = pt["annexeControlsUI"]["rotLeftBtn"].value("y", rotLeftBtnRect.y);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("rotRightBtn")) {
        rotRightBtnRect.x = pt["annexeControlsUI"]["rotRightBtn"].value("x", rotRightBtnRect.x); rotRightBtnRect.y = pt["annexeControlsUI"]["rotRightBtn"].value("y", rotRightBtnRect.y);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("resetRotBtn")) {
        resetRotBtnRect.x = pt["annexeControlsUI"]["resetRotBtn"].value("x", resetRotBtnRect.x);
        resetRotBtnRect.y = pt["annexeControlsUI"]["resetRotBtn"].value("y", resetRotBtnRect.y);
        resetRotBtnRect.width = pt["annexeControlsUI"]["resetRotBtn"].value("w", resetRotBtnRect.width);
        resetRotBtnRect.height = pt["annexeControlsUI"]["resetRotBtn"].value("h", resetRotBtnRect.height);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("patteuBtn")) {
        patteuBtnRect.x = pt["annexeControlsUI"]["patteuBtn"].value("x", patteuBtnRect.x); patteuBtnRect.y = pt["annexeControlsUI"]["patteuBtn"].value("y", patteuBtnRect.y);
        patteuDropRect.x = pt["annexeControlsUI"]["patteuDrop"].value("x", patteuDropRect.x); patteuDropRect.y = pt["annexeControlsUI"]["patteuDrop"].value("y", patteuDropRect.y);
        patteuResetBtnRect.x = pt["annexeControlsUI"]["patteuResetBtn"].value("x", patteuResetBtnRect.x); patteuResetBtnRect.y = pt["annexeControlsUI"]["patteuResetBtn"].value("y", patteuResetBtnRect.y);
        patteuIntensitySliderRect.x = pt["annexeControlsUI"]["patteuIntensity"].value("x", patteuIntensitySliderRect.x); patteuIntensitySliderRect.y = pt["annexeControlsUI"]["patteuIntensity"].value("y", patteuIntensitySliderRect.y);
        patteuHardnessSliderRect.x = pt["annexeControlsUI"]["patteuHardness"].value("x", patteuHardnessSliderRect.x); patteuHardnessSliderRect.y = pt["annexeControlsUI"]["patteuHardness"].value("y", patteuHardnessSliderRect.y);
        patteuBrushSizeSliderRect.x = pt["annexeControlsUI"]["patteuBrushSize"].value("x", patteuBrushSizeSliderRect.x); patteuBrushSizeSliderRect.y = pt["annexeControlsUI"]["patteuBrushSize"].value("y", patteuBrushSizeSliderRect.y);
    }
    if(pt.contains("annexeControlsUI") && pt["annexeControlsUI"].contains("deuPatteuBtn")) {
        deuPatteuBtnRect.x = pt["annexeControlsUI"]["deuPatteuBtn"].value("x", deuPatteuBtnRect.x); deuPatteuBtnRect.y = pt["annexeControlsUI"]["deuPatteuBtn"].value("y", deuPatteuBtnRect.y);
        deuPatteuDropFgRect.x = pt["annexeControlsUI"]["deuPatteuDropFg"].value("x", deuPatteuDropFgRect.x); deuPatteuDropFgRect.y = pt["annexeControlsUI"]["deuPatteuDropFg"].value("y", deuPatteuDropFgRect.y);
        deuPatteuDropBgRect.x = pt["annexeControlsUI"]["deuPatteuDropBg"].value("x", deuPatteuDropBgRect.x); deuPatteuDropBgRect.y = pt["annexeControlsUI"]["deuPatteuDropBg"].value("y", deuPatteuDropBgRect.y);
        deuPatteuResetFgBtnRect.x = pt["annexeControlsUI"]["deuPatteuResetFg"].value("x", deuPatteuResetFgBtnRect.x); deuPatteuResetFgBtnRect.y = pt["annexeControlsUI"]["deuPatteuResetFg"].value("y", deuPatteuResetFgBtnRect.y);
        deuPatteuResetBgBtnRect.x = pt["annexeControlsUI"]["deuPatteuResetBg"].value("x", deuPatteuResetBgBtnRect.x); deuPatteuResetBgBtnRect.y = pt["annexeControlsUI"]["deuPatteuResetBg"].value("y", deuPatteuResetBgBtnRect.y);
        deuPatteuIntensitySliderRect.x = pt["annexeControlsUI"]["deuPatteuIntensity"].value("x", deuPatteuIntensitySliderRect.x); deuPatteuIntensitySliderRect.y = pt["annexeControlsUI"]["deuPatteuIntensity"].value("y", deuPatteuIntensitySliderRect.y);
        deuPatteuHardnessSliderRect.x = pt["annexeControlsUI"]["deuPatteuHardness"].value("x", deuPatteuHardnessSliderRect.x); deuPatteuHardnessSliderRect.y = pt["annexeControlsUI"]["deuPatteuHardness"].value("y", deuPatteuHardnessSliderRect.y);
        deuPatteuRevealSliderRect.x = pt["annexeControlsUI"]["deuPatteuReveal"].value("x", deuPatteuRevealSliderRect.x); deuPatteuRevealSliderRect.y = pt["annexeControlsUI"]["deuPatteuReveal"].value("y", deuPatteuRevealSliderRect.y);
        deuPatteuBrushSizeSliderRect.x = pt["annexeControlsUI"]["deuPatteuBrushSize"].value("x", deuPatteuBrushSizeSliderRect.x); deuPatteuBrushSizeSliderRect.y = pt["annexeControlsUI"]["deuPatteuBrushSize"].value("y", deuPatteuBrushSizeSliderRect.y);
    }
}

vector<ofRectangle*> AnnexeControlsUI::getInteractableRects() { return { &soloAnnexeBtnRect, &saveFrameBtnRect, &genAIBtnRect, &genSAMBtnRect, &genDepthAnythingBtnRect, &rippleBtnRect, &recordVideoBtnRect, &samControlBtnRect, &layerVolumBtnRect, &depthMapBtnRect, &resetDepthMapBtnRect, &rotUpBtnRect, &rotDownBtnRect, &rotLeftBtnRect, &rotRightBtnRect, &resetRotBtnRect, &patteuBtnRect, &patteuDropRect, &patteuResetBtnRect, &patteuIntensitySliderRect, &patteuHardnessSliderRect, &patteuBrushSizeSliderRect, &deuPatteuBtnRect, &deuPatteuDropFgRect, &deuPatteuDropBgRect, &deuPatteuResetFgBtnRect, &deuPatteuResetBgBtnRect, &deuPatteuIntensitySliderRect, &deuPatteuHardnessSliderRect, &deuPatteuRevealSliderRect, &deuPatteuBrushSizeSliderRect }; }
ofRectangle* AnnexeControlsUI::findButtonAt(ofVec2f pos) { 
    if (soloAnnexeBtnRect.inside(pos)) return &soloAnnexeBtnRect; 
    if (saveFrameBtnRect.inside(pos)) return &saveFrameBtnRect; 
    if (genAIBtnRect.inside(pos)) return &genAIBtnRect; 
    if (genSAMBtnRect.inside(pos)) return &genSAMBtnRect;
    if (genDepthAnythingBtnRect.inside(pos)) return &genDepthAnythingBtnRect;
    if (rippleBtnRect.inside(pos)) return &rippleBtnRect;
    if (recordVideoBtnRect.inside(pos)) return &recordVideoBtnRect;
    if (samControlBtnRect.inside(pos)) return &samControlBtnRect;
    if (layerVolumBtnRect.inside(pos)) return &layerVolumBtnRect;
    if (depthMapBtnRect.inside(pos)) return &depthMapBtnRect;
    if (resetDepthMapBtnRect.inside(pos)) return &resetDepthMapBtnRect;
    if (rotUpBtnRect.inside(pos)) return &rotUpBtnRect;
    if (rotDownBtnRect.inside(pos)) return &rotDownBtnRect;
    if (rotLeftBtnRect.inside(pos)) return &rotLeftBtnRect;
    if (rotRightBtnRect.inside(pos)) return &rotRightBtnRect;
    if (resetRotBtnRect.inside(pos)) return &resetRotBtnRect;
    if (patteuBtnRect.inside(pos)) return &patteuBtnRect;
    if (patteuDropRect.inside(pos)) return &patteuDropRect;
    if (patteuResetBtnRect.inside(pos)) return &patteuResetBtnRect;
    if (patteuIntensitySliderRect.inside(pos)) return &patteuIntensitySliderRect;
    if (patteuHardnessSliderRect.inside(pos)) return &patteuHardnessSliderRect;
    if (patteuBrushSizeSliderRect.inside(pos)) return &patteuBrushSizeSliderRect;
    if (deuPatteuBtnRect.inside(pos)) return &deuPatteuBtnRect;
    if (deuPatteuDropFgRect.inside(pos)) return &deuPatteuDropFgRect;
    if (deuPatteuDropBgRect.inside(pos)) return &deuPatteuDropBgRect;
    if (deuPatteuResetFgBtnRect.inside(pos)) return &deuPatteuResetFgBtnRect;
    if (deuPatteuResetBgBtnRect.inside(pos)) return &deuPatteuResetBgBtnRect;
    if (deuPatteuIntensitySliderRect.inside(pos)) return &deuPatteuIntensitySliderRect;
    if (deuPatteuHardnessSliderRect.inside(pos)) return &deuPatteuHardnessSliderRect;
    if (deuPatteuRevealSliderRect.inside(pos)) return &deuPatteuRevealSliderRect;
    if (deuPatteuBrushSizeSliderRect.inside(pos)) return &deuPatteuBrushSizeSliderRect;
    return nullptr; 
}