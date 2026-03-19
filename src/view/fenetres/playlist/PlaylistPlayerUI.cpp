#include "PlaylistPlayerUI.h"

void PlaylistPlayerUI::setup() {
    updateLayout(100, 1200); // Position par défaut
}

void PlaylistPlayerUI::updateLayout(float startX, float startY) {
    loopButtonRect.y = startY;
    toggleButtonRect.y = startY + 40;
    simButtonRect.y = startY + 80;
    doubleSpeedBtnRect.y = startY + 120;
    muteBtnRect.y = startY + 160;
    crop106BtnRect.y = startY + 200;
    useDiskImagesBtnRect.y = startY + 240;
    pauseAccordionBtn.y = startY + 280;
    
    loopButtonRect.x = startX;
    toggleButtonRect.x = startX;
    simButtonRect.x = startX;
    doubleSpeedBtnRect.x = startX;
    muteBtnRect.x = startX;
    crop106BtnRect.x = startX;
    useDiskImagesBtnRect.x = startX;
    pauseAccordionBtn.x = startX;
    
    videoInfoBox.x = startX + 200;
    videoInfoBox.y = startY;
    
    pauseOptionRects.clear();
    for(int i=0; i<pauseOptions.size(); i++) {
        pauseOptionRects.push_back(ofRectangle(startX, pauseAccordionBtn.y + 30 + i * 30, 180, 30));
    }

    fadeOptionRects.clear();
    for(int i=0; i<fadeOptions.size(); i++) {
        fadeOptionRects.push_back(ofRectangle(startX, 0, 180, 30));
    }
}

void PlaylistPlayerUI::update() {
    if(pauseOptionRects.size() == pauseOptions.size()) {
        for(size_t i=0; i<pauseOptions.size(); i++) {
            pauseOptionRects[i].x = pauseAccordionBtn.x;
            pauseOptionRects[i].y = pauseAccordionBtn.y + pauseAccordionBtn.height + i * pauseAccordionBtn.height;
            pauseOptionRects[i].width = pauseAccordionBtn.width;
            pauseOptionRects[i].height = pauseAccordionBtn.height;
        }
    }
    
    infinitePauseBtnRect.x = pauseAccordionBtn.x;
    infinitePauseBtnRect.width = pauseAccordionBtn.width;
    infinitePauseBtnRect.height = pauseAccordionBtn.height;
    if(bPauseAccordionOpen) {
        infinitePauseBtnRect.y = pauseAccordionBtn.y + pauseAccordionBtn.height + pauseOptions.size() * pauseAccordionBtn.height + 10;
    } else {
        infinitePauseBtnRect.y = pauseAccordionBtn.y + pauseAccordionBtn.height + 10;
    }

    fadeAccordionBtn.x = pauseAccordionBtn.x;
    fadeAccordionBtn.width = pauseAccordionBtn.width;
    fadeAccordionBtn.height = pauseAccordionBtn.height;
    fadeAccordionBtn.y = infinitePauseBtnRect.getBottom() + 10;

    if(fadeOptionRects.size() == fadeOptions.size()) {
        for(size_t i=0; i<fadeOptions.size(); i++) {
            fadeOptionRects[i].x = fadeAccordionBtn.x;
            fadeOptionRects[i].y = fadeAccordionBtn.y + fadeAccordionBtn.height + i * fadeAccordionBtn.height;
            fadeOptionRects[i].width = fadeAccordionBtn.width;
            fadeOptionRects[i].height = fadeAccordionBtn.height;
        }
    }
}

void PlaylistPlayerUI::draw(Scene360VideoPlayer* player, const PlaylistNodeGraph& nodeGraph) {
    if (!player) return;
    
    ofPushStyle();
    if (player->isLoopMode()) ofSetColor(255, 150, 0); else ofSetColor(100);
    ofFill(); ofDrawRectangle(loopButtonRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(loopButtonRect.x, loopButtonRect.y); ofScale(loopButtonRect.height/30.0f, loopButtonRect.height/30.0f);
    ofDrawBitmapString("LOOP MODE: " + string(player->isLoopMode() ? "ON" : "OFF"), 15, 20);
    ofPopMatrix();

    if (player->isActive()) ofSetColor(0, 200, 50); else ofSetColor(100);
    ofFill(); ofDrawRectangle(toggleButtonRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(toggleButtonRect.x, toggleButtonRect.y); ofScale(toggleButtonRect.height/30.0f, toggleButtonRect.height/30.0f);
    ofDrawBitmapString("GO PLAYLIST [8]: " + string(player->isActive() ? "ON" : "OFF"), 10, 20);
    ofPopMatrix();

    if (player->isSimulating32Videos()) ofSetColor(200, 0, 200); else ofSetColor(100);
    ofFill(); ofDrawRectangle(simButtonRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(simButtonRect.x, simButtonRect.y); ofScale(simButtonRect.height/30.0f, simButtonRect.height/30.0f);
    ofDrawBitmapString("SIMULATE 32 VID: " + string(player->isSimulating32Videos() ? "ON" : "OFF"), 10, 20);
    ofPopMatrix();
    
    if (player->isUserSelectedNext()) {
        int upcomingIdx = player->getUpcomingVideoIndex();
        auto& vids = player->getVideos();
        if (!player->getPlannedPath().empty()) {
            int backIdx = player->getPlannedPath().back();
            if (backIdx >= 0 && backIdx < vids.size()) {
                ofDrawBitmapStringHighlight("CHEMIN PLANIFIE vers : " + vids[backIdx].endFrame, simButtonRect.getRight() + 20, simButtonRect.y + 20, ofColor(50, 150, 255), ofColor(0));
            }
        } else if (upcomingIdx >= 0 && upcomingIdx < vids.size()) {
            ofDrawBitmapStringHighlight("FORCAGE MANUEL vers : " + vids[upcomingIdx].endFrame, simButtonRect.getRight() + 20, simButtonRect.y + 20, ofColor(50, 200, 50), ofColor(0));
        }
    }
    
    if (player->isDoubleSpeed()) ofSetColor(200, 100, 50); else ofSetColor(100);
    ofFill(); ofDrawRectangle(doubleSpeedBtnRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(doubleSpeedBtnRect.x, doubleSpeedBtnRect.y); ofScale(doubleSpeedBtnRect.height/30.0f, doubleSpeedBtnRect.height/30.0f);
    ofDrawBitmapString("SPEED x2: " + string(player->isDoubleSpeed() ? "ON" : "OFF"), 10, 20);
    ofPopMatrix();
    
    if (player->isMuted()) ofSetColor(200, 50, 50); else ofSetColor(100);
    ofFill(); ofDrawRectangle(muteBtnRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(muteBtnRect.x, muteBtnRect.y); ofScale(muteBtnRect.height/30.0f, muteBtnRect.height/30.0f);
    ofDrawBitmapString("MUTE: " + string(player->isMuted() ? "ON" : "OFF"), 10, 20);
    ofPopMatrix();

    if (player->isCrop106()) ofSetColor(100, 150, 200); else ofSetColor(100);
    ofFill(); ofDrawRectangle(crop106BtnRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(crop106BtnRect.x, crop106BtnRect.y); ofScale(crop106BtnRect.height/30.0f, crop106BtnRect.height/30.0f);
    ofDrawBitmapString("CROP 106%: " + string(player->isCrop106() ? "ON" : "OFF"), 10, 20);
    ofPopMatrix();

    if (player->isUsingDiskPauseImages()) ofSetColor(100, 200, 150); else ofSetColor(100);
    ofFill(); ofDrawRectangle(useDiskImagesBtnRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(useDiskImagesBtnRect.x, useDiskImagesBtnRect.y); ofScale(useDiskImagesBtnRect.height/30.0f, useDiskImagesBtnRect.height/30.0f);
    ofDrawBitmapString("USE DISK IMGS: " + string(player->isUsingDiskPauseImages() ? "ON" : "OFF"), 5, 20);
    ofPopMatrix();

    if (bPauseAccordionOpen) ofSetColor(150, 150, 200); else ofSetColor(100);
    ofFill(); ofDrawRectangle(pauseAccordionBtn);
    ofSetColor(255);
    string loopWarning = player->isLoopMode() ? " (Ignore)" : "";
    ofPushMatrix(); ofTranslate(pauseAccordionBtn.x, pauseAccordionBtn.y); ofScale(pauseAccordionBtn.height/30.0f, pauseAccordionBtn.height/30.0f);
    ofDrawBitmapString("PAUSE: " + ofToString(player->pauseDurationFrames) + "f" + loopWarning + (bPauseAccordionOpen ? " [-]" : " [+]"), 5, 20);
    ofPopMatrix();

    if (bPauseAccordionOpen) {
        for(int i=0; i<pauseOptions.size(); i++) {
            if (player->pauseDurationFrames == pauseOptions[i]) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(pauseOptionRects[i]);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(pauseOptionRects[i]);
            ofSetColor(255);
            ofPushMatrix(); ofTranslate(pauseOptionRects[i].x, pauseOptionRects[i].y); ofScale(pauseOptionRects[i].height/30.0f, pauseOptionRects[i].height/30.0f);
            ofDrawBitmapString(ofToString(pauseOptions[i]) + " frames", 20, 20);
            ofPopMatrix();
        }
    }

    if (player->isInfinitePause()) ofSetColor(200, 50, 50); else ofSetColor(100);
    ofFill(); ofDrawRectangle(infinitePauseBtnRect);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(infinitePauseBtnRect.x, infinitePauseBtnRect.y); ofScale(infinitePauseBtnRect.height/30.0f, infinitePauseBtnRect.height/30.0f);
    ofDrawBitmapString("HOLD LAST FRAME: " + string(player->isInfinitePause() ? "ON" : "OFF"), 10, 20);
    ofPopMatrix();

    if (bFadeAccordionOpen) ofSetColor(150, 150, 200); else ofSetColor(100);
    ofFill(); ofDrawRectangle(fadeAccordionBtn);
    ofSetColor(255);
    ofPushMatrix(); ofTranslate(fadeAccordionBtn.x, fadeAccordionBtn.y); ofScale(fadeAccordionBtn.height/30.0f, fadeAccordionBtn.height/30.0f);
    ofDrawBitmapString("FADE: " + ofToString(player->fadeDurationFrames) + "f" + (bFadeAccordionOpen ? " [-]" : " [+]"), 5, 20);
    ofPopMatrix();

    if (bFadeAccordionOpen) {
        for(int i=0; i<fadeOptions.size(); i++) {
            if (player->fadeDurationFrames == fadeOptions[i]) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(fadeOptionRects[i]);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(fadeOptionRects[i]);
            ofSetColor(255);
            ofPushMatrix(); ofTranslate(fadeOptionRects[i].x, fadeOptionRects[i].y); ofScale(fadeOptionRects[i].height/30.0f, fadeOptionRects[i].height/30.0f);
            ofDrawBitmapString(ofToString(fadeOptions[i]) + " frames", 20, 20);
            ofPopMatrix();
        }
    }
    ofPopStyle();

    ofPushStyle();
    ofSetColor(20, 20, 20, 180);
    ofFill();
    ofDrawRectangle(videoInfoBox);
    ofNoFill();
    ofSetColor(150);
    ofDrawRectangle(videoInfoBox);

    int curIdx = player->getCurrentVideoIndex();
    auto& videos = player->getVideos();
    if(curIdx >= 0 && curIdx < videos.size()) {
        string fileName = ofFilePath::getFileName(videos[curIdx].path);
        string status = player->isPaused() ? "PAUSE" : "LECTURE";
        ofSetColor(0, 255, 0);
        ofDrawBitmapString(status + ": " + videos[curIdx].startFrame + " -> " + videos[curIdx].endFrame, videoInfoBox.x + 10, videoInfoBox.y + 20);
        ofSetColor(255);
        ofDrawBitmapString("Fichier: " + fileName, videoInfoBox.x + 10, videoInfoBox.y + 40);
    }
    
    if(!nodeGraph.deadEnds.empty()) {
        string warningMsg = "CUL-DE-SAC: " + nodeGraph.deadEnds[0];
        if (nodeGraph.deadEnds.size() > 1) warningMsg += ", ...";
        ofDrawBitmapStringHighlight(warningMsg, videoInfoBox.x + 10, videoInfoBox.y + 60, ofColor(200, 0, 0), ofColor(255));
    }
    ofPopStyle();
}

bool PlaylistPlayerUI::mousePressed(ofVec2f worldM, Scene360VideoPlayer* player, bool* bDrawScene360VideoPtr) {
    if (!player) return false;
    
    if (loopButtonRect.inside(worldM)) { player->toggleLoopMode(); return true; }
    if (toggleButtonRect.inside(worldM)) { 
        player->toggle(); 
        if(bDrawScene360VideoPtr) *bDrawScene360VideoPtr = player->isActive();
        return true; 
    }
    if (simButtonRect.inside(worldM)) { player->toggleSimulate32Videos(); return true; }
    if (doubleSpeedBtnRect.inside(worldM)) { player->toggleDoubleSpeed(); return true; }
    if (muteBtnRect.inside(worldM)) { player->toggleMute(); return true; }
    if (crop106BtnRect.inside(worldM)) { player->toggleCrop106(); return true; }
    if (useDiskImagesBtnRect.inside(worldM)) { player->toggleUseDiskPauseImages(); return true; }
    if (infinitePauseBtnRect.inside(worldM)) { player->toggleInfinitePause(); return true; }
    if (pauseAccordionBtn.inside(worldM)) { bPauseAccordionOpen = !bPauseAccordionOpen; return true; }
    if (fadeAccordionBtn.inside(worldM)) { bFadeAccordionOpen = !bFadeAccordionOpen; return true; }
    
    if (bPauseAccordionOpen) {
        for(int i=0; i<pauseOptionRects.size(); i++) {
            if (pauseOptionRects[i].inside(worldM)) {
                player->pauseDurationFrames = pauseOptions[i];
                bPauseAccordionOpen = false;
                return true;
            }
        }
    }
    if (bFadeAccordionOpen) {
        for(int i=0; i<fadeOptionRects.size(); i++) {
            if (fadeOptionRects[i].inside(worldM)) {
                player->fadeDurationFrames = fadeOptions[i];
                bFadeAccordionOpen = false;
                return true;
            }
        }
    }
    return false;
}

string PlaylistPlayerUI::getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager) {
    if(loopButtonRect.inside(worldM)) return tooltipManager.getTooltipText("LOOP");
    if(toggleButtonRect.inside(worldM)) return tooltipManager.getTooltipText("PLAY");
    if(simButtonRect.inside(worldM)) return tooltipManager.getTooltipText("SIMU");
    if(pauseAccordionBtn.inside(worldM)) return tooltipManager.getTooltipText("PAUSE");
    if(doubleSpeedBtnRect.inside(worldM)) return tooltipManager.getTooltipText("SPEED_X2");
    if(muteBtnRect.inside(worldM)) return tooltipManager.getTooltipText("MUTE");
    if(crop106BtnRect.inside(worldM)) return tooltipManager.getTooltipText("106CROP");
    if(useDiskImagesBtnRect.inside(worldM)) return tooltipManager.getTooltipText("USE_DISK_IMGS");
    if(infinitePauseBtnRect.inside(worldM)) return tooltipManager.getTooltipText("HOLD_FRAME");
    if(fadeAccordionBtn.inside(worldM)) return tooltipManager.getTooltipText("FADE");
    if(videoInfoBox.inside(worldM)) return tooltipManager.getTooltipText("VIDEO_INFO");
    
    if (bPauseAccordionOpen) {
        for(size_t i=0; i<pauseOptionRects.size(); i++) {
            if (pauseOptionRects[i].inside(worldM)) return "Arret de " + ofToString(pauseOptions[i]) + " frames avant la prochaine video.";
        }
    }
    if (bFadeAccordionOpen) {
        for(size_t i=0; i<fadeOptionRects.size(); i++) {
            if (fadeOptionRects[i].inside(worldM)) return "Fondu de " + ofToString(fadeOptions[i]) + " frames vers l'image de pause.";
        }
    }
    return "";
}

void PlaylistPlayerUI::saveSettings(ofJson& pt) {
    auto saveR = [&](const string& key, const ofRectangle& r) {
        pt[key]["x"] = r.x; pt[key]["y"] = r.y; pt[key]["w"] = r.width; pt[key]["h"] = r.height;
    };
    saveR("loop", loopButtonRect);
    saveR("toggle", toggleButtonRect);
    saveR("sim", simButtonRect);
    saveR("speedX2", doubleSpeedBtnRect);
    saveR("mute", muteBtnRect);
    saveR("crop106", crop106BtnRect);
    saveR("useDiskImages", useDiskImagesBtnRect);
    saveR("videoInfo", videoInfoBox);
    saveR("pause", pauseAccordionBtn);
    // On ne sauvegarde pas infinitePauseBtnRect ni pauseOptionRects car ils sont fixés dynamiquement à l'accordéon dans update()
}

void PlaylistPlayerUI::loadSettings(const ofJson& pt) {
    auto loadR = [&](const string& key, ofRectangle& r) {
        if(pt.contains(key)) {
            r.x = pt[key].value("x", r.x); r.y = pt[key].value("y", r.y);
            if(pt[key].contains("w")) r.width = pt[key].value("w", r.width);
            if(pt[key].contains("h")) r.height = pt[key].value("h", r.height);
        }
    };
    loadR("loop", loopButtonRect);
    loadR("toggle", toggleButtonRect);
    loadR("sim", simButtonRect);
    loadR("speedX2", doubleSpeedBtnRect);
    loadR("mute", muteBtnRect);
    loadR("crop106", crop106BtnRect);
    loadR("useDiskImages", useDiskImagesBtnRect);
    loadR("videoInfo", videoInfoBox);
    loadR("pause", pauseAccordionBtn);
    
    // Si le bouton n'existe pas dans le JSON ou a une taille nulle (nouvel ajout), on l'aligne sur "CROP 106%"
    if (!pt.contains("useDiskImages") || useDiskImagesBtnRect.width == 0) {
        useDiskImagesBtnRect.x = crop106BtnRect.x;
        useDiskImagesBtnRect.y = crop106BtnRect.getBottom() + 15;
        useDiskImagesBtnRect.width = crop106BtnRect.width > 0 ? crop106BtnRect.width : 180;
        useDiskImagesBtnRect.height = crop106BtnRect.height > 0 ? crop106BtnRect.height : 30;
    }
}

vector<ofRectangle*> PlaylistPlayerUI::getInteractableRects() {
    vector<ofRectangle*> rects;
    rects.push_back(&loopButtonRect);
    rects.push_back(&toggleButtonRect);
    rects.push_back(&simButtonRect);
    rects.push_back(&pauseAccordionBtn);
    if (bPauseAccordionOpen) {
        for(auto& r : pauseOptionRects) rects.push_back(&r);
    }
    rects.push_back(&doubleSpeedBtnRect);
    rects.push_back(&muteBtnRect);
    rects.push_back(&crop106BtnRect);
    rects.push_back(&useDiskImagesBtnRect);
    rects.push_back(&videoInfoBox);
    rects.push_back(&infinitePauseBtnRect);
    rects.push_back(&fadeAccordionBtn);
    if (bFadeAccordionOpen) {
        for(auto& r : fadeOptionRects) rects.push_back(&r);
    }
    return rects;
}

ofRectangle* PlaylistPlayerUI::findButtonAt(ofVec2f pos) {
    if(loopButtonRect.inside(pos)) return &loopButtonRect;
    if(toggleButtonRect.inside(pos)) return &toggleButtonRect;
    if(simButtonRect.inside(pos)) return &simButtonRect;
    if(pauseAccordionBtn.inside(pos)) return &pauseAccordionBtn;
    if (bPauseAccordionOpen) {
        for(auto& r : pauseOptionRects) if(r.inside(pos)) return &r;
    }
    if(doubleSpeedBtnRect.inside(pos)) return &doubleSpeedBtnRect;
    if(muteBtnRect.inside(pos)) return &muteBtnRect;
    if(crop106BtnRect.inside(pos)) return &crop106BtnRect;
    if(useDiskImagesBtnRect.inside(pos)) return &useDiskImagesBtnRect;
    if(videoInfoBox.inside(pos)) return &videoInfoBox;
    if(infinitePauseBtnRect.inside(pos)) return &infinitePauseBtnRect;
    if(fadeAccordionBtn.inside(pos)) return &fadeAccordionBtn;
    if (bFadeAccordionOpen) {
        for(auto& r : fadeOptionRects) if(r.inside(pos)) return &r;
    }
    return nullptr;
}