#include "ImageGraphPlayer.h"
#include <queue>

void ImageGraphPlayer::setup(AtmosphereSystem* atm) {
    atmosphere = atm;
}

bool ImageGraphPlayer::ccw(ofVec2f A, ofVec2f B, ofVec2f C) {
    return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
}

bool ImageGraphPlayer::lineIntersectsLine(ofVec2f A, ofVec2f B, ofVec2f C, ofVec2f D) {
    return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
}

void ImageGraphPlayer::generateGraph(const ofRectangle& bnds) {
    if (nodes.empty()) return;
    bounds = bnds;
    
    for (size_t i = 0; i < nodes.size(); i++) {
        bool placed = false;
        for (int tries = 0; i == 0 || tries < 200; tries++) {
            ofVec2f candidate(ofRandom(bounds.x + 20, bounds.getRight() - 20),
                              ofRandom(bounds.y + 60, bounds.getBottom() - 20));
            bool tooClose = false;
            for (size_t j = 0; j < i; j++) {
                if (candidate.distance(nodes[j].pos) < 60) {
                    tooClose = true;
                    break;
                }
            }
            if (!tooClose || tries == 199) {
                nodes[i].pos = candidate;
                break;
            }
        }
    }
    
    vector<bool> inTree(nodes.size(), false);
    inTree[0] = true;
    for (size_t step = 1; step < nodes.size(); step++) {
        float minDist = FLT_MAX;
        int bestA = -1, bestB = -1;
        for (size_t i = 0; i < nodes.size(); i++) {
            if (inTree[i]) {
                for (size_t j = 0; j < nodes.size(); j++) {
                    if (!inTree[j]) {
                        float d = nodes[i].pos.distanceSquared(nodes[j].pos);
                        if (d < minDist) {
                            minDist = d;
                            bestA = i;
                            bestB = j;
                        }
                    }
                }
            }
        }
        if (bestA != -1 && bestB != -1) {
            inTree[bestB] = true;
            nodes[bestA].edges.push_back(bestB);
            nodes[bestB].edges.push_back(bestA);
        }
    }
    
    for (size_t i = 0; i < nodes.size(); i++) {
        vector<pair<float, int>> dists;
        for (size_t j = 0; j < nodes.size(); j++) {
            if (i != j) dists.push_back({nodes[i].pos.distanceSquared(nodes[j].pos), (int)j});
        }
        std::sort(dists.begin(), dists.end());
        for (int k = 0; k < std::min(3, (int)dists.size()); k++) {
            int j = dists[k].second;
            if (std::find(nodes[i].edges.begin(), nodes[i].edges.end(), j) == nodes[i].edges.end()) {
                bool crosses = false;
                for (size_t a = 0; a < nodes.size(); a++) {
                    for (int b : nodes[a].edges) {
                        if (a < b && a != i && a != j && b != i && b != j) {
                            if (lineIntersectsLine(nodes[i].pos, nodes[j].pos, nodes[a].pos, nodes[b].pos)) {
                                crosses = true; break;
                            }
                        }
                    }
                    if (crosses) break;
                }
                if (!crosses) {
                    nodes[i].edges.push_back(j);
                    nodes[j].edges.push_back(i);
                }
            }
        }
    }
}

void ImageGraphPlayer::loadFolder(const string& folderPath, const ofRectangle& bnds) {
    ofDirectory dir(folderPath);
    if (!dir.exists()) return;
    
    dir.allowExt("png"); dir.allowExt("jpg"); dir.allowExt("jpeg");
    dir.allowExt("PNG"); dir.allowExt("JPG"); dir.allowExt("JPEG");
    dir.listDir();
    
    if (dir.size() == 0) return;
    
    currentFolderPath = folderPath;
    nodes.clear();
    plannedPath.clear();
    currentIndex = -1;
    nextIndex = -1;
    state = 0;
    
    int maxChars = std::max(8, (int)(bnds.width / 25.0f)); // Limite la taille en fonction du cadre
    for (int i = 0; i < dir.size(); i++) {
        ImageGraphNode n;
        n.path = dir.getPath(i);
        string baseName = ofFilePath::getBaseName(dir.getName(i)); // Sans l'extension
        if (baseName.length() > maxChars) {
            int partLen = (maxChars - 2) / 2;
            n.name = baseName.substr(0, partLen + (maxChars % 2)) + ".." + baseName.substr(baseName.length() - partLen);
        } else {
            n.name = baseName;
        }
        nodes.push_back(n);
    }
    
    generateGraph(bnds);
}

void ImageGraphPlayer::update() {
    if (!isPlaying || nodes.empty() || !atmosphere) return;
    
    if (currentIndex == -1) {
        currentIndex = 0;
        currentImg.load(nodes[currentIndex].path);
        updateAtmosphere(currentImg, 1.0f);
        state = 0;
        stateTimer = pauseDuration;
    }
    
    if (state == 0) {
        stateTimer--;
        if (stateTimer <= 0) {
            if (!plannedPath.empty()) {
                nextIndex = plannedPath.front();
                plannedPath.erase(plannedPath.begin());
            } else {
                if (!nodes[currentIndex].edges.empty()) {
                    nextIndex = nodes[currentIndex].edges[ofRandom(nodes[currentIndex].edges.size())];
                } else {
                    nextIndex = (currentIndex + 1) % nodes.size();
                }
            }
            nextImg.load(nodes[nextIndex].path);
            state = 1;
            stateTimer = fadeDuration;
        }
    } else if (state == 1) {
        stateTimer--;
        float alpha = 1.0f - (float)stateTimer / max(1, fadeDuration);
        
        if (!crossfadeFbo.isAllocated() || crossfadeFbo.getWidth() != currentImg.getWidth() || crossfadeFbo.getHeight() != currentImg.getHeight()) {
            ofFbo::Settings s;
            s.width = currentImg.getWidth() > 0 ? currentImg.getWidth() : 1920;
            s.height = currentImg.getHeight() > 0 ? currentImg.getHeight() : 1080;
            s.internalformat = GL_RGB;
            crossfadeFbo.allocate(s);
        }
        
        crossfadeFbo.begin();
        ofClear(0);
        ofSetColor(255);
        if (currentImg.isAllocated()) currentImg.draw(0, 0, crossfadeFbo.getWidth(), crossfadeFbo.getHeight());
        if (nextImg.isAllocated() && alpha > 0.0f) {
            ofEnableAlphaBlending();
            ofSetColor(255, 255 * alpha);
            nextImg.draw(0, 0, crossfadeFbo.getWidth(), crossfadeFbo.getHeight());
            ofDisableAlphaBlending();
        }
        crossfadeFbo.end();
        
        updateAtmosphereFbo(crossfadeFbo);
        
        if (stateTimer <= 0) {
            currentIndex = nextIndex;
            currentImg = nextImg;
            nextIndex = -1;
            state = 0;
            stateTimer = pauseDuration;
            updateAtmosphere(currentImg, 1.0f);
        }
    }
}

void ImageGraphPlayer::updateAtmosphere(ofImage& img, float alpha) {
    if (!atmosphere || !img.isAllocated()) return;
    if (!atmosphere->lastFrameFbo.isAllocated() || atmosphere->lastFrameFbo.getWidth() != img.getWidth() || atmosphere->lastFrameFbo.getHeight() != img.getHeight()) {
        ofFbo::Settings s;
        s.width = img.getWidth();
        s.height = img.getHeight();
        s.internalformat = GL_RGB;
        atmosphere->lastFrameFbo.allocate(s);
    }
    atmosphere->lastFrameFbo.begin();
    ofClear(0);
    ofSetColor(255);
    img.draw(0, 0);
    atmosphere->lastFrameFbo.end();
    
    atmosphere->bShowLastFrame = true;
    atmosphere->bShow360 = true;
    atmosphere->bShowSphere = false;
    atmosphere->bShowDiscoBall = false;
}

void ImageGraphPlayer::updateAtmosphereFbo(ofFbo& fbo) {
    if (!atmosphere) return;
    if (!atmosphere->lastFrameFbo.isAllocated() || atmosphere->lastFrameFbo.getWidth() != fbo.getWidth() || atmosphere->lastFrameFbo.getHeight() != fbo.getHeight()) {
        ofFbo::Settings s;
        s.width = fbo.getWidth();
        s.height = fbo.getHeight();
        s.internalformat = GL_RGB;
        atmosphere->lastFrameFbo.allocate(s);
    }
    atmosphere->lastFrameFbo.begin();
    ofClear(0);
    ofSetColor(255);
    fbo.draw(0, 0);
    atmosphere->lastFrameFbo.end();
    atmosphere->bShowLastFrame = true;
    atmosphere->bShow360 = true;
}

void ImageGraphPlayer::draw(const ofRectangle& dropZone) {
    // Si le cadre a bouge ou a ete redimensionne en mode EDIT, on actualise la position des noeuds
    if (bounds.width > 0 && bounds.height > 0 && 
        (std::abs(bounds.x - dropZone.x) > 0.1f || std::abs(bounds.y - dropZone.y) > 0.1f || 
         std::abs(bounds.width - dropZone.width) > 0.1f || std::abs(bounds.height - dropZone.height) > 0.1f)) {
        
        float scaleX = dropZone.width / bounds.width;
        float scaleY = dropZone.height / bounds.height;
        int maxChars = std::max(8, (int)(dropZone.width / 25.0f));
        
        for (auto& node : nodes) {
            node.pos.x = dropZone.x + (node.pos.x - bounds.x) * scaleX;
            node.pos.y = dropZone.y + (node.pos.y - bounds.y) * scaleY;
            
            string baseName = ofFilePath::getBaseName(node.path);
            if (baseName.length() > maxChars) {
                int partLen = (maxChars - 2) / 2;
                node.name = baseName.substr(0, partLen + (maxChars % 2)) + ".." + baseName.substr(baseName.length() - partLen);
            } else {
                node.name = baseName;
            }
        }
    }

    bounds = dropZone;
    ofPushStyle();
    ofSetColor(255, 255, 255, 15);
    ofFill();
    ofDrawRectangle(dropZone);
    ofSetColor(255, 150, 100, 200);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(dropZone);
    
    if (nodes.empty()) {
        ofSetColor(255);
        ofDrawBitmapStringHighlight("Glissez dossier images ici", dropZone.x + 20, dropZone.y + dropZone.height/2, ofColor(0, 150), ofColor(255));
        ofPopStyle();
        return;
    }
    
    playBtn.set(dropZone.x + 10, dropZone.y + 10, 80, 25);
    pauseBtn.set(dropZone.x + 100, dropZone.y + 10, 140, 25);
    fadeBtn.set(dropZone.x + 250, dropZone.y + 10, 130, 25);
    
    pauseOptionRects.resize(pauseOptions.size());
    for(size_t i=0; i<pauseOptions.size(); i++) {
        pauseOptionRects[i].set(pauseBtn.x, pauseBtn.getBottom() + i * 25, pauseBtn.width, 25);
    }
    
    fadeOptionRects.resize(fadeOptions.size());
    for(size_t i=0; i<fadeOptions.size(); i++) {
        fadeOptionRects[i].set(fadeBtn.x, fadeBtn.getBottom() + i * 25, fadeBtn.width, 25);
    }

    ofFill();
    ofSetColor(isPlaying ? ofColor(50, 200, 50) : ofColor(100));
    ofDrawRectangle(playBtn);
    ofSetColor(100, 100, 150);
    ofDrawRectangle(pauseBtn);
    ofSetColor(150, 100, 100);
    ofDrawRectangle(fadeBtn);
    
    ofSetColor(255);
    ofDrawBitmapString(isPlaying ? "PLAYING" : "PAUSED", playBtn.x + 8, playBtn.y + 16);
    ofDrawBitmapString("PAUSE: " + ofToString(pauseDuration) + (bPauseAccordionOpen ? " [-]" : " [+]"), pauseBtn.x + 8, pauseBtn.y + 16);
    ofDrawBitmapString("FADE: " + ofToString(fadeDuration) + (bFadeAccordionOpen ? " [-]" : " [+]"), fadeBtn.x + 8, fadeBtn.y + 16);
    
    ofSetLineWidth(1.5);
    for (size_t i = 0; i < nodes.size(); i++) {
        for (int neighbor : nodes[i].edges) {
            if (i < neighbor) {
                ofSetColor(150, 200, 255, 100);
                ofDrawLine(nodes[i].pos, nodes[neighbor].pos);
            }
        }
    }
    
    if (!plannedPath.empty() && currentIndex != -1) {
        ofSetColor(50, 255, 50, 200);
        ofSetLineWidth(3);
        int prev = (state == 1 && nextIndex != -1) ? nextIndex : currentIndex;
        int pathLength = plannedPath.size();
        int pathIndex = 0;
        for (int step : plannedPath) {
            ofVec2f p1 = nodes[prev].pos;
            ofVec2f p2 = nodes[step].pos;
            ofDrawLine(p1, p2);
            ofVec2f textPos = p1 + (p2 - p1) * 0.5f;
            ofDrawBitmapStringHighlight(ofToString(pathLength - pathIndex), textPos.x - 4, textPos.y + 4, ofColor(0, 150), ofColor(255));
            prev = step;
            pathIndex++;
        }
    }
    
    if (state == 1 && currentIndex != -1 && nextIndex != -1) {
        ofSetColor(255, 255, 0, 255); // Jaune pour le chemin actif en cours de fondu
        ofSetLineWidth(4);
        ofVec2f p1 = nodes[currentIndex].pos;
        ofVec2f p2 = nodes[nextIndex].pos;
        ofDrawLine(p1, p2);

        float arrowPosPct = 1.0f - (float)stateTimer / std::max(1, fadeDuration);
        ofVec2f dir = p2 - p1;
        ofVec2f nDir = dir.getNormalized();
        ofVec2f mid = p1 + dir * arrowPosPct;
        ofVec2f perp(-nDir.y, nDir.x);
        
        ofFill();
        ofDrawTriangle(mid + nDir * 15, mid + perp * 10 - nDir * 6, mid - perp * 10 - nDir * 6);
    }
    
    for (size_t i = 0; i < nodes.size(); i++) {
        bool isNext = (!plannedPath.empty() && plannedPath.front() == i) || (state == 1 && nextIndex == i);
        if (i == currentIndex) ofSetColor(255, 50, 50);
        else if (isNext) ofSetColor(50, 255, 50);
        else ofSetColor(220);
        
        ofFill();
        ofDrawCircle(nodes[i].pos, (i == currentIndex || isNext) ? 12 : 8);
        ofSetColor(255);
        ofDrawBitmapStringHighlight(nodes[i].name, nodes[i].pos.x + 15, nodes[i].pos.y + 4, ofColor(0, 150), ofColor(255));
    }

    // On dessine les menus déroulants par-dessus le graphe
    if (bPauseAccordionOpen) {
        for(size_t i=0; i<pauseOptions.size(); i++) {
            if (pauseDuration == pauseOptions[i]) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(pauseOptionRects[i]);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(pauseOptionRects[i]);
            ofSetColor(255);
            ofDrawBitmapString(ofToString(pauseOptions[i]) + " frames", pauseOptionRects[i].x + 10, pauseOptionRects[i].y + 16);
        }
    }
    if (bFadeAccordionOpen) {
        for(size_t i=0; i<fadeOptions.size(); i++) {
            if (fadeDuration == fadeOptions[i]) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(fadeOptionRects[i]);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(fadeOptionRects[i]);
            ofSetColor(255);
            ofDrawBitmapString(ofToString(fadeOptions[i]) + " frames", fadeOptionRects[i].x + 10, fadeOptionRects[i].y + 16);
        }
    }

    // --- Affichage du chemin du dossier (croppe si trop long) ---
    if(!currentFolderPath.empty()) {
        float maxW = dropZone.width - 20; // Marge de 10px de chaque cote
        string displayText = currentFolderPath;
        
        if(displayText.length() * 8.0f > maxW) { // Une lettre fait environ 8 pixels de large
            string fileName = ofFilePath::getFileName(currentFolderPath);
            int charsAllowed = std::max(0, (int)(maxW / 8.0f) - (int)fileName.length() - 4); // -4 pour ".../"
            
            if(charsAllowed > 0) displayText = currentFolderPath.substr(0, charsAllowed) + ".../" + fileName;
            else displayText = ".../" + fileName;
        }
        ofDrawBitmapStringHighlight(displayText, dropZone.x + 10, dropZone.getBottom() + 20, ofColor(0, 200), ofColor(200, 220, 255));
    }

    ofPopStyle();
}

void ImageGraphPlayer::planPath(int targetIndex) {
    if (currentIndex == -1 || targetIndex == -1 || currentIndex == targetIndex) return;
    
    std::queue<int> frontier;
    frontier.push(currentIndex);
    map<int, int> cameFrom;
    cameFrom[currentIndex] = currentIndex;
    
    bool found = false;
    while (!frontier.empty()) {
        int current = frontier.front();
        frontier.pop();
        
        if (current == targetIndex) {
            found = true;
            break;
        }
        
        for (int next : nodes[current].edges) {
            if (cameFrom.find(next) == cameFrom.end()) {
                frontier.push(next);
                cameFrom[next] = current;
            }
        }
    }
    
    if (found) {
        plannedPath.clear();
        int current = targetIndex;
        while (current != currentIndex) {
            plannedPath.push_back(current);
            current = cameFrom[current];
        }
        std::reverse(plannedPath.begin(), plannedPath.end());
    }
}

bool ImageGraphPlayer::mousePressed(ofVec2f worldM) {
    if (nodes.empty()) return false;
    
    if (bPauseAccordionOpen) {
        for (size_t i = 0; i < pauseOptionRects.size(); i++) {
            if (pauseOptionRects[i].inside(worldM)) {
                pauseDuration = pauseOptions[i];
                bPauseAccordionOpen = false;
                return true;
            }
        }
    }
    if (bFadeAccordionOpen) {
        for (size_t i = 0; i < fadeOptionRects.size(); i++) {
            if (fadeOptionRects[i].inside(worldM)) {
                fadeDuration = fadeOptions[i];
                bFadeAccordionOpen = false;
                return true;
            }
        }
    }

    if (playBtn.inside(worldM)) {
        isPlaying = !isPlaying;
        return true;
    }
    if (pauseBtn.inside(worldM)) {
        bPauseAccordionOpen = !bPauseAccordionOpen;
        bFadeAccordionOpen = false;
        return true;
    }
    if (fadeBtn.inside(worldM)) {
        bFadeAccordionOpen = !bFadeAccordionOpen;
        bPauseAccordionOpen = false;
        return true;
    }
    
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].pos.distance(worldM) < 15) {
            if (!isPlaying) {
                currentIndex = i;
                currentImg.load(nodes[currentIndex].path);
                updateAtmosphere(currentImg, 1.0f);
                plannedPath.clear();
                nextIndex = -1;
                state = 0;
                stateTimer = pauseDuration;
            } else {
                planPath(i);
            }
            return true;
        }
    }
    return false;
}

void ImageGraphPlayer::saveSettings(ofJson& pt) {
    pt["imageGraphUI"]["pauseDuration"] = pauseDuration;
    pt["imageGraphUI"]["fadeDuration"] = fadeDuration;
    pt["imageGraphUI"]["folderPath"] = currentFolderPath;
}

void ImageGraphPlayer::loadSettings(const ofJson& pt, const ofRectangle& bnds) {
    if(pt.contains("imageGraphUI")) {
        pauseDuration = pt["imageGraphUI"].value("pauseDuration", pauseDuration);
        fadeDuration = pt["imageGraphUI"].value("fadeDuration", fadeDuration);
        string fPath = pt["imageGraphUI"].value("folderPath", "");
        if (!fPath.empty()) {
            loadFolder(fPath, bnds);
            isPlaying = false; // Met en pause après le chargement initial
        }
    }
}

vector<ofRectangle*> ImageGraphPlayer::getInteractableRects() {
    vector<ofRectangle*> rects;
    rects.push_back(&playBtn);
    rects.push_back(&pauseBtn);
    if (bPauseAccordionOpen) {
        for(auto& r : pauseOptionRects) rects.push_back(&r);
    }
    rects.push_back(&fadeBtn);
    if (bFadeAccordionOpen) {
        for(auto& r : fadeOptionRects) rects.push_back(&r);
    }
    return rects;
}

ofRectangle* ImageGraphPlayer::findButtonAt(ofVec2f pos) {
    if (bPauseAccordionOpen) {
        for(auto& r : pauseOptionRects) if(r.inside(pos)) return &r;
    }
    if (bFadeAccordionOpen) {
        for(auto& r : fadeOptionRects) if(r.inside(pos)) return &r;
    }
    if (playBtn.inside(pos)) return &playBtn;
    if (pauseBtn.inside(pos)) return &pauseBtn;
    if (fadeBtn.inside(pos)) return &fadeBtn;
    return nullptr;
}