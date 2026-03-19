#include "PlaylistTextureUI.h"
#include "RoomApp.h"

void PlaylistTextureUI::update() {
    textureToggleBtn.set(textureDropZone.x, textureDropZone.y - 40, textureDropZone.width, 30);
}

void PlaylistTextureUI::draw() {
    ofPushStyle();
    
    // --- DESSIN DE LA ZONE DES TEXTURES / VIDEOS ---
    ofSetColor(255, 255, 255, 15);
    ofFill();
    ofDrawRectangle(textureDropZone);
    ofSetColor(100, 200, 255, 200);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(textureDropZone);

    ofSetColor(255);
    if (textureFiles.empty()) {
        string msg = "Glisser dossier\ntextures/videos ici";
        ofDrawBitmapStringHighlight(msg, textureDropZone.x + 20, textureDropZone.y + textureDropZone.height/2, ofColor(0,150), ofColor(255));
    } else {
        int maxDisplay = std::max(1, (int)((textureDropZone.height - 40) / 20));
        
        if (textureScrollOffset > (int)textureFiles.size() - maxDisplay) textureScrollOffset = std::max(0, (int)textureFiles.size() - maxDisplay);
        if (textureScrollOffset < 0) textureScrollOffset = 0;

        int maxChars = std::max(5, (int)((textureDropZone.width - 25) / 8.0f));
        int endIndex = std::min((int)textureFiles.size(), textureScrollOffset + maxDisplay);
        for(int i = textureScrollOffset; i < endIndex; i++) {
            if (i == currentTextureIndex) ofSetColor(50, 255, 50);
            else ofSetColor(200);
            string fName = ofFilePath::getFileName(textureFiles[i]);
            if (fName.length() > maxChars) fName = fName.substr(0, maxChars - 3) + "...";
            ofDrawBitmapString(fName, textureDropZone.x + 10, textureDropZone.y + 30 + (i - textureScrollOffset) * 20);
        }
        
        if ((int)textureFiles.size() > maxDisplay) {
            float scrollBarWidth = 6;
            float viewRatio = (float)maxDisplay / textureFiles.size();
            float scrollBarHeight = std::max(20.0f, textureDropZone.height * viewRatio);
            float maxScrollOffset = textureFiles.size() - maxDisplay;
            float scrollPos = (textureDropZone.height - scrollBarHeight) * ((float)textureScrollOffset / maxScrollOffset);
            
            ofSetColor(150, 150, 150, 180);
            ofFill();
            ofDrawRectangle(textureDropZone.getRight() - scrollBarWidth - 4, textureDropZone.y + scrollPos, scrollBarWidth, scrollBarHeight);
        }
    }
    
    // Bouton ON/OFF
    if (bTextureControlOn) ofSetColor(50, 200, 50); else ofSetColor(100);
    ofFill(); ofDrawRectangle(textureToggleBtn);
    ofNoFill(); ofSetColor(200); ofDrawRectangle(textureToggleBtn);
    ofSetColor(255); ofDrawBitmapString("CONTROLE FLECHES : " + string(bTextureControlOn ? "ON" : "OFF"), textureToggleBtn.x + 10, textureToggleBtn.y + 20);
    
    ofPopStyle();
}

void PlaylistTextureUI::loadFolder(const string& path, RoomApp* roomApp) {
    ofDirectory dir(path);
    if (!dir.exists()) return;
    
    currentFolderPath = path;
    
    dir.allowExt("png"); dir.allowExt("jpg"); dir.allowExt("jpeg");
    dir.allowExt("mp4"); dir.allowExt("mov");
    dir.allowExt("PNG"); dir.allowExt("JPG"); dir.allowExt("JPEG");
    dir.allowExt("MP4"); dir.allowExt("MOV");
    dir.listDir();
    dir.sort();
    textureFiles.clear();
    for(int i=0; i<dir.size(); i++) textureFiles.push_back(dir.getPath(i));
    
    if(!textureFiles.empty()) {
        currentTextureIndex = 0;
        textureScrollOffset = 0;
        bTextureControlOn = true;
        if(roomApp) roomApp->atmosphere.loadTexture(textureFiles[currentTextureIndex]);
    }
    ofLogNotice("PlaylistTextureUI") << "Dossier textures charge : " << textureFiles.size() << " fichiers.";
}

bool PlaylistTextureUI::handleFolderDrop(const string& path, ofVec2f dropPos, RoomApp* roomApp) {
    if(textureDropZone.inside(dropPos)) {
        loadFolder(path, roomApp);
        return true;
    }
    return false;
}

bool PlaylistTextureUI::mousePressed(ofVec2f worldM, RoomApp* roomApp) {
    if(textureToggleBtn.inside(worldM)) {
        bTextureControlOn = !bTextureControlOn;
        return true;
    }
    if(textureDropZone.inside(worldM) && !textureFiles.empty()) {
        float localY = worldM.y - (textureDropZone.y + 15);
        if(localY >= 0) {
            int maxDisplay = std::max(1, (int)((textureDropZone.height - 40) / 20));
            int clickedIdx = textureScrollOffset + (int)(localY / 20);
            if(clickedIdx >= 0 && clickedIdx < textureFiles.size() && clickedIdx < textureScrollOffset + maxDisplay) {
                currentTextureIndex = clickedIdx;
                if(roomApp) roomApp->atmosphere.loadTexture(textureFiles[currentTextureIndex]);
                
                float currentTime = ofGetElapsedTimeMillis();
                if(clickedIdx == lastClickedIndex && (currentTime - lastClickTime) < 400) {
                    string absPath = ofToDataPath(textureFiles[currentTextureIndex], true);
                    ofSystem("open -R \"" + absPath + "\"");
                }
                lastClickedIndex = clickedIdx;
                lastClickTime = currentTime;
            }
        }
        return true;
    }
    return false;
}

bool PlaylistTextureUI::mouseScrolled(ofVec2f worldM, float scrollY) {
    if(textureDropZone.inside(worldM) && !textureFiles.empty()) {
        int maxDisplay = std::max(1, (int)((textureDropZone.height - 40) / 20));
        if ((int)textureFiles.size() > maxDisplay) {
            textureScrollOffset -= (scrollY > 0) ? 3 : -3;
            if (textureScrollOffset > (int)textureFiles.size() - maxDisplay) textureScrollOffset = textureFiles.size() - maxDisplay;
            if (textureScrollOffset < 0) textureScrollOffset = 0;
        }
        return true; 
    }
    return false;
}

bool PlaylistTextureUI::keyPressed(int key, RoomApp* roomApp) {
    if (bTextureControlOn && !textureFiles.empty() && (key == OF_KEY_LEFT || key == OF_KEY_RIGHT || key == OF_KEY_UP || key == OF_KEY_DOWN)) {
        if (key == OF_KEY_LEFT || key == OF_KEY_UP) {
            currentTextureIndex = (currentTextureIndex - 1 + textureFiles.size()) % textureFiles.size();
        } else if (key == OF_KEY_RIGHT || key == OF_KEY_DOWN) {
            currentTextureIndex = (currentTextureIndex + 1) % textureFiles.size();
        }
        
        int maxDisplay = std::max(1, (int)((textureDropZone.height - 40) / 20));
        if (currentTextureIndex < textureScrollOffset) {
            textureScrollOffset = currentTextureIndex;
        } else if (currentTextureIndex >= textureScrollOffset + maxDisplay) {
            textureScrollOffset = currentTextureIndex - maxDisplay + 1;
        }
        
        if(roomApp) roomApp->atmosphere.loadTexture(textureFiles[currentTextureIndex]);
        return true;
    }
    return false;
}

void PlaylistTextureUI::saveSettings(ofJson& pt) {
    pt["textureUI"]["x"] = textureDropZone.x;
    pt["textureUI"]["y"] = textureDropZone.y;
    pt["textureUI"]["w"] = textureDropZone.width;
    pt["textureUI"]["h"] = textureDropZone.height;
    pt["textureUI"]["bControlOn"] = bTextureControlOn;
    pt["textureUI"]["folderPath"] = currentFolderPath;
}

void PlaylistTextureUI::loadSettings(const ofJson& pt) {
    if(pt.contains("textureUI")) {
        textureDropZone.x = pt["textureUI"].value("x", textureDropZone.x);
        textureDropZone.y = pt["textureUI"].value("y", textureDropZone.y);
        textureDropZone.width = pt["textureUI"].value("w", textureDropZone.width);
        textureDropZone.height = pt["textureUI"].value("h", textureDropZone.height);
        bTextureControlOn = pt["textureUI"].value("bControlOn", bTextureControlOn);
        currentFolderPath = pt["textureUI"].value("folderPath", currentFolderPath);
    }
}

vector<ofRectangle*> PlaylistTextureUI::getInteractableRects() {
    vector<ofRectangle*> rects;
    rects.push_back(&textureDropZone);
    return rects;
}

ofRectangle* PlaylistTextureUI::findButtonAt(ofVec2f pos) {
    if(textureToggleBtn.inside(pos)) return &textureToggleBtn;
    if(textureDropZone.inside(pos)) return &textureDropZone;
    return nullptr;
}