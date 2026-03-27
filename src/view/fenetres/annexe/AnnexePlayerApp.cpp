#include "AnnexePlayerApp.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

shared_ptr<ofAppBaseWindow> AnnexePlayerApp::getAppWindow(int index) {
    if(!mainAppPtr) return nullptr;
    switch(index) {
        case 0: return mainAppPtr->mainWindowPtr;
        case 1: return mainAppPtr->roomWindowPtr;
        case 2: return mainAppPtr->zenitWindowPtr;
        case 3: return mainAppPtr->scene2DWindowPtr;
        case 4: return mainAppPtr->previewWindowPtr;
        case 5: return mainAppPtr->buttonWindowPtr;
        case 6: return mainAppPtr->annexeWindowPtr;
        case 7: return mainAppPtr->annexePlayerWindowPtr;
    }
    return nullptr;
}

void AnnexePlayerApp::setup() {
    ofSetBackgroundColor(25);
    zoom = 1.0f;
    pan.set(0, 0);
    isSpacePressed = false;
    
    tooltipManager.setup();
    searchBar.setup();
    
    editBtnRect.set(10, 40, 60, 30);
    saveBtnRect.set(10, 80, 60, 30);
    loadBtnRect.set(10, 120, 60, 30);
    addNoteBtnRect.set(10, 160, 60, 30);
    addFrameBtnRect.set(10, 200, 60, 30);
    searchBtnRect.set(10, 240, 60, 30);
    focusAnnexeBtnRect.set(10, 280, 100, 30);
    
    for(int i=0; i<5; i++) {
        cameraPresetBtns[i].set(250 + i * 40, 5, 30, 30);
        presetPans[i].set(0, 0);
        presetZooms[i] = 1.0f;
        
        windowPresetBtns[i].set(480 + i * 40, 5, 30, 30);
        for(int w=0; w<8; w++) windowPresets[i][w].set(0,0,0,0);
        windowPresetSaved[i] = false;
    }
    
    controlsUI.setup();
    loadButtonPositions();
}

void AnnexePlayerApp::update() {}

void AnnexePlayerApp::draw() {
    ofBackground(25);

    ofPushMatrix();
    ofTranslate(pan);
    ofScale(zoom, zoom);

    for(auto& frame : visualFrames) {
        bool isSelected = (bEditMode && std::find(selectedRects.begin(), selectedRects.end(), &frame->rect) != selectedRects.end());
        frame->draw(isSelected);
    }
    
    for(auto& note : textNotes) {
        note->draw();
    }
    
    controlsUI.draw(mainAppPtr);

    if (bEditMode) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(255, 200, 0, 255);
        ofSetLineWidth(2);
        for(auto* r : selectedRects) {
            ofDrawRectangle(r->x - 2, r->y - 2, r->width + 4, r->height + 4);
            ofFill();
            ofDrawRectangle(r->getRight() - 6, r->getBottom() - 6, 10, 10);
            ofNoFill();
        }
        if (bIsSelecting) {
            float x1 = std::min(selectionStart.x, selectionEnd.x);
            float y1 = std::min(selectionStart.y, selectionEnd.y);
            float x2 = max(selectionStart.x, selectionEnd.x);
            float y2 = max(selectionStart.y, selectionEnd.y);
            ofRectangle selRect(x1, y1, x2 - x1, y2 - y1);
            
            ofFill(); ofSetColor(100, 150, 255, 50); ofDrawRectangle(selRect);
            ofNoFill(); ofSetColor(100, 150, 255, 200); ofSetLineWidth(1); ofDrawRectangle(selRect);
        }
        ofPopStyle();
    }
    ofPopMatrix();

    ofPushStyle();
    if(bEditMode) ofSetColor(50, 200, 50); else ofSetColor(30, 30, 30);
    ofFill(); ofDrawRectangle(editBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(editBtnRect);
    ofDrawBitmapString("EDIT", editBtnRect.x + 12, editBtnRect.y + 20);
    
    bool showSaveFeedback = (ofGetElapsedTimef() - saveFeedbackTimer < 1.0f);
    if(showSaveFeedback) ofSetColor(50, 200, 50); else ofSetColor(80);
    ofFill(); ofDrawRectangle(saveBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(saveBtnRect);
    ofDrawBitmapString(showSaveFeedback ? "SAVED" : "SAVE", saveBtnRect.x + 8, saveBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(loadBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(loadBtnRect);
    ofDrawBitmapString("LOAD", loadBtnRect.x + 12, loadBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(addNoteBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(addNoteBtnRect);
    ofDrawBitmapString("+ NOTE", addNoteBtnRect.x + 5, addNoteBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(addFrameBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(addFrameBtnRect);
    ofDrawBitmapString("+ FRAME", addFrameBtnRect.x + 3, addFrameBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(searchBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(searchBtnRect);
    ofDrawBitmapString("SEARCH", searchBtnRect.x + 6, searchBtnRect.y + 20);

    ofSetColor(50, 150, 200);
    ofFill(); ofDrawRectangle(focusAnnexeBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(focusAnnexeBtnRect);
    ofDrawBitmapString("FOCUS ANNEXE", focusAnnexeBtnRect.x + 6, focusAnnexeBtnRect.y + 20);
    
    for(int i=0; i<5; i++) {
        ofSetColor(80);
        ofFill(); ofDrawRectangle(cameraPresetBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(cameraPresetBtns[i]);
        ofDrawBitmapString(ofToString(i+1), cameraPresetBtns[i].x + 11, cameraPresetBtns[i].y + 20);
        if(cameraPresetBtns[i].inside(ofGetMouseX(), ofGetMouseY())) hoveredTooltip = "Vue " + ofToString(i+1) + " (Shift+clic: save)";
    }
    
    for(int i=0; i<5; i++) {
        ofSetColor(windowPresetSaved[i] ? ofColor(150, 100, 200) : ofColor(80));
        ofFill(); ofDrawRectangle(windowPresetBtns[i]);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(windowPresetBtns[i]);
        ofDrawBitmapString("A" + ofToString(i+1), windowPresetBtns[i].x + 8, windowPresetBtns[i].y + 20);
        if(windowPresetBtns[i].inside(ofGetMouseX(), ofGetMouseY())) hoveredTooltip = "Fenetres A" + ofToString(i+1) + " (Shift+clic: save)";
    }
    ofPopStyle();

    ofSetColor(255);
    ofDrawBitmapStringHighlight("ANNEXE PLAYER", 10, 20);

    // --- DETECTION DES TOOLTIPS (Au survol) ---
    ofVec2f screenM(ofGetMouseX(), ofGetMouseY());
    ofVec2f worldM = getTransformedMouse(screenM.x, screenM.y);
    
    string controlsTip = controlsUI.getTooltip(worldM, tooltipManager);
    if(!controlsTip.empty()) hoveredTooltip = controlsTip;
    
    if(editBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("EDIT");
    if(saveBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("SAVE");
    if(loadBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("LOAD");
    if(addNoteBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("+ NOTE");
    if(addFrameBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("+ FRAME");
    if(searchBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("SEARCH");
    if(focusAnnexeBtnRect.inside(screenM)) hoveredTooltip = tooltipManager.getTooltipText("FOCUS ANNEXE");

    if(!hoveredTooltip.empty()) {
        tooltipManager.drawTooltip(hoveredTooltip, ofGetMouseX(), ofGetMouseY());
        hoveredTooltip = ""; // Reset
    }
    
    searchBar.draw();
}

void AnnexePlayerApp::mousePressed(int x, int y, int button) {
    lastMouse.set(x, y);
    if (searchBar.mousePressed(x, y, pan, zoom)) return;
    if (isSpacePressed) return;
    
    bool isShiftPressed = ofGetKeyPressed(OF_KEY_SHIFT);
    for(int i=0; i<5; i++) {
        if(cameraPresetBtns[i].inside(x, y)) {
            if(isShiftPressed) { presetPans[i] = pan; presetZooms[i] = zoom; }
            else { pan = presetPans[i]; zoom = presetZooms[i]; }
            return;
        }
        if(windowPresetBtns[i].inside(x, y)) {
            if(isShiftPressed) {
                for(int w=0; w<8; w++) {
                    auto win = getAppWindow(w);
                    if(win) windowPresets[i][w].set(win->getWindowPosition().x, win->getWindowPosition().y, win->getWindowSize().x, win->getWindowSize().y);
                }
                windowPresetSaved[i] = true; saveUndoState();
            } else if (windowPresetSaved[i]) {
                for(int w=0; w<8; w++) {
                    auto win = getAppWindow(w);
                    if(win && windowPresets[i][w].width > 50) {
                        win->setWindowPosition(windowPresets[i][w].x, windowPresets[i][w].y);
                        win->setWindowShape(windowPresets[i][w].width, windowPresets[i][w].height);
                    }
                }
            }
            return;
        }
    }

    if (editBtnRect.inside(x, y)) { bEditMode = !bEditMode; selectedRects.clear(); bIsSelecting = false; bIsDraggingGroup = false; return; }
    if (saveBtnRect.inside(x, y)) { saveButtonPositions(); return; }
    if (loadBtnRect.inside(x, y)) { loadButtonPositions(); return; }
    if (searchBtnRect.inside(x, y)) { searchBar.toggle(getAllSearchableButtons(), pan, zoom); return; }
    
    if (focusAnnexeBtnRect.inside(x, y)) {
        if (mainAppPtr && mainAppPtr->annexeWindowPtr) {
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->annexeWindowPtr);
            if (glfwWin) {
                glfwShowWindow(glfwWin->getGLFWWindow());
                glfwRestoreWindow(glfwWin->getGLFWWindow());
                glfwFocusWindow(glfwWin->getGLFWWindow());
            }
        }
        return;
    }

    if (addNoteBtnRect.inside(x, y)) {
        if (editingNote) editingNote->bIsEditing = false;
        saveUndoState();
        ofVec2f center = getTransformedMouse(ofGetWidth()/2, ofGetHeight()/2);
        auto newNote = make_shared<PlaylistTextNote>(center);
        newNote->bIsEditing = true;
        textNotes.push_back(newNote);
        editingNote = newNote;
        return;
    }
    if (addFrameBtnRect.inside(x, y)) {
        saveUndoState();
        ofVec2f center = getTransformedMouse(ofGetWidth()/2, ofGetHeight()/2);
        visualFrames.push_back(make_shared<PlaylistVisualFrame>(center));
        return;
    }

    ofVec2f worldM = getTransformedMouse(x, y);
    if (editingNote && !editingNote->rect.inside(worldM)) {
        editingNote->bIsEditing = false;
        editingNote = nullptr;
    }
    
    ofRectangle* clickedRect = findButtonAt(worldM);
    bool isCmdPressed = ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_SUPER) || ofGetKeyPressed(OF_KEY_CONTROL);
    if (!clickedRect && isCmdPressed) {
        bEditMode = !bEditMode;
        selectedRects.clear();
        bIsSelecting = false; bIsDraggingGroup = false;
        return;
    }

    if (bEditMode) {
        for (auto& frame : visualFrames) {
            if (std::find(selectedRects.begin(), selectedRects.end(), &frame->rect) != selectedRects.end() && frame->isResizeHit(worldM)) {
                saveUndoState(); resizingFrame = frame; return;
            }
        }
        for (auto* r : selectedRects) {
            ofRectangle handle(r->getRight() - 10, r->getBottom() - 10, 20, 20);
            if (handle.inside(worldM)) { saveUndoState(); resizingRect = r; return; }
        }
        if (clickedRect) {
            bool alreadySelected = false;
            for (auto* r : selectedRects) if (r == clickedRect) { alreadySelected = true; break; }
            if (!alreadySelected) { selectedRects.clear(); selectedRects.push_back(clickedRect); }
            dragOffsets.clear();
            for (auto* r : selectedRects) dragOffsets.push_back(worldM - ofVec2f(r->x, r->y));
            saveUndoState(); bIsDraggingGroup = true;
        } else {
            selectedRects.clear(); bIsSelecting = true; selectionStart = worldM; selectionEnd = worldM;
        }
        return;
    }
    
    for(auto& note : textNotes) {
        if (note->rect.inside(worldM)) {
            if (editingNote) editingNote->bIsEditing = false;
            note->bIsEditing = true;
            editingNote = note;
            return;
        }
    }

    if (controlsUI.mousePressed(worldM, mainAppPtr)) return;

    bIsDraggingPan = true;
}

void AnnexePlayerApp::mouseDragged(int x, int y, int button) {
    if(isSpacePressed || bIsDraggingPan) {
        ofVec2f currentMouse(x, y);
        pan += (currentMouse - lastMouse);
        lastMouse = currentMouse;
        return;
    }
    if (bEditMode) {
        ofVec2f worldM = getTransformedMouse(x, y);
        if (resizingFrame) { resizingFrame->rect.width = std::max(20.0f, worldM.x - resizingFrame->rect.x); resizingFrame->rect.height = std::max(20.0f, worldM.y - resizingFrame->rect.y); return; }
        if (resizingRect) { resizingRect->width = std::max(10.0f, worldM.x - resizingRect->x); resizingRect->height = std::max(10.0f, worldM.y - resizingRect->y); return; }
        if (bIsDraggingGroup && selectedRects.size() == dragOffsets.size()) {
            for(size_t i=0; i<selectedRects.size(); i++) { selectedRects[i]->x = worldM.x - dragOffsets[i].x; selectedRects[i]->y = worldM.y - dragOffsets[i].y; }
        } else if (bIsSelecting) {
            selectionEnd = worldM;
            float x1 = std::min(selectionStart.x, selectionEnd.x); float y1 = std::min(selectionStart.y, selectionEnd.y);
            float x2 = max(selectionStart.x, selectionEnd.x); float y2 = max(selectionStart.y, selectionEnd.y);
            ofRectangle selRect(x1, y1, x2 - x1, y2 - y1);
            selectedRects.clear();
            for(auto* r : getAllInteractableRects()) {
                bool isFrame = false;
                for(auto& f : visualFrames) if (&f->rect == r) { isFrame = true; break; }
                if (isFrame && selRect.x <= r->x && selRect.y <= r->y && selRect.getRight() >= r->getRight() && selRect.getBottom() >= r->getBottom()) selectedRects.push_back(r);
                else if (!isFrame && selRect.intersects(*r)) selectedRects.push_back(r);
            }
        }
        return;
    }
}

void AnnexePlayerApp::mouseReleased(int x, int y, int button) {
    if (bEditMode) { bIsDraggingGroup = false; bIsSelecting = false; resizingFrame = nullptr; resizingRect = nullptr; }
    bIsDraggingPan = false;
}

void AnnexePlayerApp::mouseMoved(int x, int y) { searchBar.mouseMoved(x, y, pan, zoom); }
void AnnexePlayerApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (searchBar.isVisible() || scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse(x, y);
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    zoom = ofClamp(zoom * zoomFactor, 0.01f, 50.0f);
    pan.x = x - worldM.x * zoom; pan.y = y - worldM.y * zoom;
}

void AnnexePlayerApp::keyPressed(int key) {
    if (searchBar.keyPressed(key, pan, zoom)) return;
    if (editingNote) {
        if ((ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) && (key == 'v' || key == 'V' || key == 22)) { 
            editingNote->text += ofGetWindowPtr()->getClipboardString(); editingNote->updateSize(); return;
        }
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) { if (!editingNote->text.empty()) { editingNote->text.pop_back(); editingNote->updateSize(); } }
        else if (key == OF_KEY_RETURN) { editingNote->text += '\n'; editingNote->updateSize(); }
        else if (key == OF_KEY_ESC) { editingNote->bIsEditing = false; editingNote = nullptr; }
        else if (key >= 32 && key <= 126) { editingNote->text += (char)key; editingNote->updateSize(); }
        return;
    }
    
    if (ofGetKeyPressed(OF_KEY_SHIFT) && (key == 'f' || key == 'F')) { searchBar.toggle(getAllSearchableButtons(), pan, zoom); return; }
    if (key >= '1' && key <= '5') { int idx = key - '1'; pan = presetPans[idx]; zoom = presetZooms[idx]; return; }
    if (ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) {
        if (bEditMode && (key == 'z' || key == 'Z' || key == 26)) { undo(); return; }
        if (key == 's' || key == 'S' || key == 19) { saveButtonPositions(); return; }
    }
    if (bEditMode && !selectedRects.empty() && (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL)) {
        saveUndoState();
        textNotes.erase(std::remove_if(textNotes.begin(), textNotes.end(), [&](const shared_ptr<PlaylistTextNote>& n) { return std::find(selectedRects.begin(), selectedRects.end(), &n->rect) != selectedRects.end(); }), textNotes.end());
        visualFrames.erase(std::remove_if(visualFrames.begin(), visualFrames.end(), [&](const shared_ptr<PlaylistVisualFrame>& f) { return std::find(selectedRects.begin(), selectedRects.end(), &f->rect) != selectedRects.end(); }), visualFrames.end());
        selectedRects.clear(); return;
    }
    if (key == ' ') isSpacePressed = true;
    if (key == 'r' || key == 'R') {
        zoom = 1.0f;
        pan.set(0, 0);
    }
}
void AnnexePlayerApp::keyReleased(int key) { if (key == ' ') isSpacePressed = false; }

ofVec2f AnnexePlayerApp::getTransformedMouse(int x, int y) { return ofVec2f((x - pan.x) / zoom, (y - pan.y) / zoom); }
ofRectangle* AnnexePlayerApp::findButtonAt(ofVec2f pos) {
    if(auto* r = controlsUI.findButtonAt(pos)) return r;
    for(auto& note : textNotes) if(note->rect.inside(pos)) return &note->rect;
    for(auto& frame : visualFrames) if(frame->isEdgeHit(pos)) return &frame->rect;
    return nullptr;
}

vector<ofRectangle*> AnnexePlayerApp::getAllInteractableRects() {
    vector<ofRectangle*> rects;
    for(auto* r : controlsUI.getInteractableRects()) rects.push_back(r);
    for(auto& note : textNotes) rects.push_back(&note->rect);
    for(auto& frame : visualFrames) rects.push_back(&frame->rect);
    return rects;
}
vector<SearchableButton> AnnexePlayerApp::getAllSearchableButtons() {
    vector<SearchableButton> res;
    res.push_back({"Focus Annexe Win", &focusAnnexeBtnRect});
    res.push_back({"Solo Annexe", &controlsUI.soloAnnexeBtnRect});
    for(auto& note : textNotes) {
        string preview = note->text; ofStringReplace(preview, "\n", " ");
        if (preview.length() > 20) preview = preview.substr(0, 17) + "...";
        if (preview.empty()) preview = "Note vide";
        res.push_back({"Note: " + preview, &note->rect});
    }
    return res;
}

ofJson AnnexePlayerApp::serializeState() {
    ofJson pt;
    pt["view"]["zoom"] = zoom; pt["view"]["pan_x"] = pan.x; pt["view"]["pan_y"] = pan.y;
    pt["notes"] = ofJson::array();
    for(auto& note : textNotes) { ofJson n; n["text"] = note->text; n["x"] = note->rect.x; n["y"] = note->rect.y; pt["notes"].push_back(n); }
    pt["frames"] = ofJson::array();
    for(auto& frame : visualFrames) { ofJson f; f["x"] = frame->rect.x; f["y"] = frame->rect.y; f["w"] = frame->rect.width; f["h"] = frame->rect.height; pt["frames"].push_back(f); }
    controlsUI.saveSettings(pt);
    return pt;
}
void AnnexePlayerApp::deserializeState(const ofJson& pt) {
    if(pt.contains("view")) { zoom = pt["view"].value("zoom", zoom); pan.x = pt["view"].value("pan_x", pan.x); pan.y = pt["view"].value("pan_y", pan.y); }
    textNotes.clear(); editingNote = nullptr;
    controlsUI.loadSettings(pt);
    if(pt.contains("notes")) { for(auto& n : pt["notes"]) { auto note = make_shared<PlaylistTextNote>(ofVec2f(n.value("x", 0.0f), n.value("y", 0.0f))); note->text = n.value("text", ""); note->updateSize(); textNotes.push_back(note); } }
    visualFrames.clear(); resizingFrame = nullptr;
    if(pt.contains("frames")) { for(auto& f : pt["frames"]) { auto frame = make_shared<PlaylistVisualFrame>(ofVec2f(0, 0)); frame->rect.x = f.value("x", 0.0f); frame->rect.y = f.value("y", 0.0f); frame->rect.width = f.value("w", 300.0f); frame->rect.height = f.value("h", 200.0f); visualFrames.push_back(frame); } }
}

void AnnexePlayerApp::saveUndoState() {
    ofJson currentState = serializeState();
    if (!undoStack.empty() && undoStack.back() == currentState) return; 
    undoStack.push_back(currentState);
    if(undoStack.size() > 5) undoStack.pop_front();
}
void AnnexePlayerApp::undo() {
    if(undoStack.empty()) return;
    ofJson pt = undoStack.back(); undoStack.pop_back();
    selectedRects.clear(); resizingFrame = nullptr; resizingRect = nullptr; bIsDraggingGroup = false; bIsSelecting = false; editingNote = nullptr;
    deserializeState(pt);
}

void AnnexePlayerApp::saveButtonPositions() {
    ofJson pt = serializeState();
    pt["window"]["x"] = ofGetWindowPositionX(); pt["window"]["y"] = ofGetWindowPositionY(); pt["window"]["w"] = ofGetWindowWidth(); pt["window"]["h"] = ofGetWindowHeight();
    ofSavePrettyJson("annexe_btn_positions.json", pt);
    saveFeedbackTimer = ofGetElapsedTimef();
}
void AnnexePlayerApp::loadButtonPositions() {
    ofFile file("annexe_btn_positions.json");
    if(!file.exists()) return;
    ofJson pt = ofLoadJson(file.path());
    if(pt.contains("window")) {
        int ww = pt["window"].value("w", ofGetWindowWidth()); int wh = pt["window"].value("h", ofGetWindowHeight());
        if (ww > 50 && wh > 50) { ofSetWindowPosition(pt["window"].value("x", ofGetWindowPositionX()), pt["window"].value("y", ofGetWindowPositionY())); ofSetWindowShape(ww, wh); }
    }
    deserializeState(pt);
}