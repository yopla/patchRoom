#include "PlaylistSequenceUI.h"
#include "PlaylistVisualizerApp.h"
#include "ofApp.h"
#include "RoomApp.h"
#include "Scene2D_SIDE.h"
#include "RoomPreview.h"
#include "AnnexeApp.h"
#include "AnnexePlayerApp.h"
#include "ofAppGLFWWindow.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

void PlaylistSequence::save() {
    ofJson pt;
    pt["pan_x"] = pan.x;
    pt["pan_y"] = pan.y;
    pt["zoom"] = zoom;
    
    pt["buttons"] = ofJson::array();
    for(auto& b : buttons) {
        ofJson bj;
        bj["name"] = b.originalName;
        bj["x"] = b.rect.x;
        bj["y"] = b.rect.y;
        bj["w"] = b.rect.width;
        bj["h"] = b.rect.height;
        bj["stateIdx"] = b.currentStateIdx;
        pt["buttons"].push_back(bj);
    }
    
    pt["notes"] = ofJson::array();
    for(auto& n : notes) {
        ofJson nj;
        nj["text"] = n->text;
        nj["x"] = n->rect.x;
        nj["y"] = n->rect.y;
        pt["notes"].push_back(nj);
    }
    
    pt["frames"] = ofJson::array();
    for(auto& f : frames) {
        ofJson fj;
        fj["x"] = f->rect.x; fj["y"] = f->rect.y;
        fj["w"] = f->rect.width; fj["h"] = f->rect.height;
        pt["frames"].push_back(fj);
    }
    
    viewsUI.saveSettings(pt);
    ofSavePrettyJson(filename, pt);
    ofLogNotice("PlaylistSequence") << "Saved sequence to " << filename;
}

void PlaylistSequence::load() {
    ofFile file(filename);
    if(!file.exists()) return;
    
    ofJson pt = ofLoadJson(file.path());
    pan.x = pt.value("pan_x", 250.0f);
    pan.y = pt.value("pan_y", 0.0f);
    zoom = pt.value("zoom", 1.0f);
    
    buttons.clear();
    if(pt.contains("buttons")) {
        for(auto& bj : pt["buttons"]) {
            SeqButton b;
            b.originalName = bj.value("name", "");
            b.rect.set(bj.value("x", 0.0f), bj.value("y", 0.0f), bj.value("w", 150.0f), bj.value("h", 40.0f));
            b.labelRect.set(b.rect.x, b.rect.getBottom(), b.rect.width, 20);
            b.currentStateIdx = bj.value("stateIdx", 0);
            buttons.push_back(b);
        }
    }
    
    notes.clear();
    if(pt.contains("notes")) {
        for(auto& nj : pt["notes"]) {
            auto note = std::make_shared<PlaylistTextNote>(ofVec2f(nj.value("x", 0.0f), nj.value("y", 0.0f)));
            note->text = nj.value("text", "");
            note->updateSize();
            notes.push_back(note);
        }
    }
    
    frames.clear();
    if(pt.contains("frames")) {
        for(auto& fj : pt["frames"]) {
            auto frame = std::make_shared<PlaylistVisualFrame>(ofVec2f(0, 0));
            frame->rect.set(fj.value("x", 0.0f), fj.value("y", 0.0f), fj.value("w", 300.0f), fj.value("h", 200.0f));
            frames.push_back(frame);
        }
    }
    
    viewsUI.loadSettings(pt);
}

void PlaylistSequenceUI::setup() {
    // Création initiale de deux séquences
    PlaylistSequence s1; s1.name = "Seq 1"; s1.filename = "seq1.json"; s1.load();
    PlaylistSequence s2; s2.name = "Seq 2"; s2.filename = "seq2.json"; s2.load();
    sequences.push_back(s1);
    sequences.push_back(s2);
    
    topBtnRects.push_back(ofRectangle(400, 10, 80, 25));
    topBtnRects.push_back(ofRectangle(490, 10, 80, 25));
    
    exitBtnRect.set(10, 40, 100, 30);
    editBtnRect.set(10, 80, 100, 30);
    saveBtnRect.set(10, 120, 100, 30);
    addNoteBtnRect.set(10, 160, 100, 30);
    addFrameBtnRect.set(10, 200, 100, 30);
    vuesBtnRect.set(10, 240, 100, 30);
}

void PlaylistSequenceUI::update(PlaylistVisualizerApp* app) {
    // Les boutons on besoin d'avoir leurs states peuplés au cas où le code a changé
    if(activeSeqIdx != -1) {
        for(auto& b : sequences[activeSeqIdx].buttons) {
            if(b.states.empty()) b.states = getButtonStates(b.originalName, app);
        }
    }
}

void PlaylistSequenceUI::drawTopButtons() {
    ofPushStyle();
    for(size_t i=0; i<sequences.size(); i++) {
        if (i == activeSeqIdx) {
            ofSetColor(50, 200, 50); // Mettre en evidence la sequence active
        } else {
            ofSetColor(80);
        }
        ofFill(); ofDrawRectangle(topBtnRects[i]);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(topBtnRects[i]);
        ofSetColor(255);
        ofDrawBitmapString(sequences[i].name, topBtnRects[i].x + 15, topBtnRects[i].y + 17);
    }
    ofPopStyle();
}

void PlaylistSequenceUI::drawContextMenu() {
    if(!bContextMenuOpen) return;
    ofPushStyle();
    for(size_t i=0; i<contextMenuRects.size(); i++) {
        ofSetColor(40, 40, 40, 240);
        ofFill(); ofDrawRectangle(contextMenuRects[i]);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(contextMenuRects[i]);
        ofSetColor(255);
        ofDrawBitmapString("Ajouter a " + sequences[i].name, contextMenuRects[i].x + 10, contextMenuRects[i].y + 17);
    }
    ofPopStyle();
}

void PlaylistSequenceUI::openContextMenu(ofVec2f screenPos, std::string btnName) {
    bContextMenuOpen = true;
    contextMenuPos = screenPos;
    contextMenuTarget = btnName;
    contextMenuRects.clear();
    for(size_t i=0; i<sequences.size(); i++) {
        contextMenuRects.push_back(ofRectangle(screenPos.x, screenPos.y + i*25, 150, 25));
    }
}

void PlaylistSequenceUI::draw(PlaylistVisualizerApp* app) {
    if(activeSeqIdx == -1) return;
    
    PlaylistSequence& seq = sequences[activeSeqIdx];
    
    ofPushStyle();
    ofSetColor(15, 20, 30, 230); // Fond légèrement transparent
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    ofPopStyle();
    
    ofPushMatrix();
    ofTranslate(seq.pan);
    ofScale(seq.zoom, seq.zoom);
    
    // Cadres
    for(auto& frame : seq.frames) {
        bool isSelected = (bEditMode && std::find(selectedItems.begin(), selectedItems.end(), frame.get()) != selectedItems.end());
        frame->draw(isSelected);
    }
    
    // Boutons Fantômes
    ofPushStyle();
    for(auto& b : seq.buttons) {
        bool isSelected = (bEditMode && std::find(selectedItems.begin(), selectedItems.end(), &b) != selectedItems.end());
        
        // Bouton principal
        if(isSelected) ofSetColor(255, 200, 0); else ofSetColor(100, 150, 200);
        ofFill(); ofDrawRectangle(b.rect);
        ofNoFill(); ofSetColor(40); ofDrawRectangle(b.rect);
        
        ofSetColor(255);
        std::string label = b.originalName;
        if(label.length() > 18) label = label.substr(0, 15) + "...";
        ofDrawBitmapString(label, b.rect.x + 5, b.rect.y + 25);
        
        // Label d'état en dessous
        ofSetColor(50, 50, 50);
        ofFill(); ofDrawRectangle(b.labelRect);
        ofNoFill(); ofSetColor(30); ofDrawRectangle(b.labelRect);
        
        ofSetColor(255, 200, 100);
        std::string stateTxt = (b.states.size() > b.currentStateIdx) ? b.states[b.currentStateIdx] : "TRIGGER";
        ofDrawBitmapString("Etat: " + stateTxt, b.labelRect.x + 5, b.labelRect.y + 15);
        
        if (isSelected) {
            ofFill();
            ofSetColor(255, 200, 0);
            ofDrawRectangle(b.labelRect.getRight() - 10, b.labelRect.getBottom() - 10, 10, 10);
        }
    }
    ofPopStyle();
    
    // Notes
    for(auto& note : seq.notes) note->draw();
    
    // Rendu Edit Mode Selection
    if (bEditMode && bIsSelecting) {
        ofPushStyle();
        float x1 = std::min(selectionStart.x, selectionEnd.x);
        float y1 = std::min(selectionStart.y, selectionEnd.y);
        float x2 = std::max(selectionStart.x, selectionEnd.x);
        float y2 = std::max(selectionStart.y, selectionEnd.y);
        ofRectangle selRect(x1, y1, x2 - x1, y2 - y1);
        ofFill(); ofSetColor(100, 150, 255, 50); ofDrawRectangle(selRect);
        ofNoFill(); ofSetColor(100, 150, 255, 200); ofDrawRectangle(selRect);
        ofPopStyle();
    }
    
    ofPopMatrix();
    
    // HUD FIXE SEQUENCE
    ofPushStyle();
    ofDrawBitmapStringHighlight("SEQUENCE : " + seq.name, 10, 20);
    
    if(isSpacePressed || bIsDraggingPan) {
        ofDrawBitmapStringHighlight("ESPACE / CLIC DANS LE VIDE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20, ofColor(50, 200, 50), ofColor(0));
    } else {
        ofDrawBitmapStringHighlight("ESPACE / CLIC DANS LE VIDE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20);
    }
    
    ofSetColor(200, 50, 50); ofFill(); ofDrawRectangle(exitBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(exitBtnRect);
    ofDrawBitmapString("<- RETOUR", exitBtnRect.x + 10, exitBtnRect.y + 20);
    
    if(bEditMode) ofSetColor(50, 200, 50); else ofSetColor(80);
    ofFill(); ofDrawRectangle(editBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(editBtnRect);
    ofDrawBitmapString("EDIT", editBtnRect.x + 10, editBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(saveBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(saveBtnRect);
    ofDrawBitmapString("SAVE JSON", saveBtnRect.x + 10, saveBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(addNoteBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(addNoteBtnRect);
    ofDrawBitmapString("+ NOTE", addNoteBtnRect.x + 10, addNoteBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(addFrameBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(addFrameBtnRect);
    ofDrawBitmapString("+ FRAME", addFrameBtnRect.x + 10, addFrameBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(vuesBtnRect); ofNoFill(); ofSetColor(255); ofDrawRectangle(vuesBtnRect);
    ofDrawBitmapString("VUES", vuesBtnRect.x + 10, vuesBtnRect.y + 20);
    ofPopStyle();
    
    seq.viewsUI.draw();
    
    // Dessin du menu contextuel interne (Dupliquer/Supprimer)
    if (bSeqContextMenuOpen) {
        ofPushStyle();
        ofSetColor(40, 40, 40, 240);
        ofFill(); ofDrawRectangle(seqDuplicateRect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(seqDuplicateRect);
        ofSetColor(255); ofDrawBitmapString("Dupliquer", seqDuplicateRect.x + 10, seqDuplicateRect.y + 17);
        
        ofSetColor(150, 40, 40, 240);
        ofFill(); ofDrawRectangle(seqDeleteRect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(seqDeleteRect);
        ofSetColor(255); ofDrawBitmapString("Supprimer", seqDeleteRect.x + 10, seqDeleteRect.y + 17);
        ofPopStyle();
    }
}

bool PlaylistSequenceUI::mousePressed(int x, int y, int button, PlaylistVisualizerApp* app) {
    // Menu Contextuel global
    if(bContextMenuOpen) {
        for(size_t i=0; i<contextMenuRects.size(); i++) {
            if(contextMenuRects[i].inside(x, y)) {
                SeqButton sb;
                sb.originalName = contextMenuTarget;
                sb.rect.set(250, 250, 150, 40);
                sb.labelRect.set(sb.rect.x, sb.rect.getBottom(), 150, 20);
                sb.states = getButtonStates(contextMenuTarget, app);
                sequences[i].buttons.push_back(sb);
                ofLogNotice("PlaylistSequenceUI") << "Ajoute " << contextMenuTarget << " a " << sequences[i].name;
                bContextMenuOpen = false;
                return true;
            }
        }
        bContextMenuOpen = false; // Ferme si clic ailleur
    }
    
    // Clics sur les boutons du haut (Seq 1, Seq 2...)
    for(size_t i=0; i<topBtnRects.size(); i++) {
        if(topBtnRects[i].inside(x, y)) {
            if (activeSeqIdx == (int)i) { // Si on clique sur le bouton de la séquence déjà active
                activeSeqIdx = -1; // On la ferme
            } else {
                activeSeqIdx = i;
                bEditMode = false;
            }
            return true;
        }
    }

    if(activeSeqIdx == -1) {
        return false;
    }
    
    // --- LOGIQUE INTERNE A LA SEQUENCE ---
    PlaylistSequence& seq = sequences[activeSeqIdx];
    
    // Gestion du clic sur le Menu Contextuel interne (Dupliquer/Supprimer)
    if (bSeqContextMenuOpen) {
        if (seqDuplicateRect.inside(x, y)) {
            if (seqContextType == SeqItemType::BUTTON && seqContextIndex < seq.buttons.size()) {
                SeqButton copy = seq.buttons[seqContextIndex];
                copy.rect.x += 20; copy.rect.y += 20;
                copy.labelRect.x += 20; copy.labelRect.y += 20;
                seq.buttons.push_back(copy);
            } else if (seqContextType == SeqItemType::NOTE && seqContextIndex < seq.notes.size()) {
                auto& n = seq.notes[seqContextIndex];
                auto copy = std::make_shared<PlaylistTextNote>(ofVec2f(n->rect.x + 20, n->rect.y + 20));
                copy->text = n->text;
                copy->textColor = n->textColor;
                copy->updateSize();
                seq.notes.push_back(copy);
            } else if (seqContextType == SeqItemType::FRAME && seqContextIndex < seq.frames.size()) {
                auto& f = seq.frames[seqContextIndex];
                auto copy = std::make_shared<PlaylistVisualFrame>(ofVec2f(0, 0));
                copy->rect = f->rect;
                copy->rect.x += 20; copy->rect.y += 20;
                seq.frames.push_back(copy);
            }
            bSeqContextMenuOpen = false;
            return true;
        } else if (seqDeleteRect.inside(x, y)) {
            if (seqContextType == SeqItemType::BUTTON && seqContextIndex < seq.buttons.size()) {
                seq.buttons.erase(seq.buttons.begin() + seqContextIndex);
            } else if (seqContextType == SeqItemType::NOTE && seqContextIndex < seq.notes.size()) {
                seq.notes.erase(seq.notes.begin() + seqContextIndex);
            } else if (seqContextType == SeqItemType::FRAME && seqContextIndex < seq.frames.size()) {
                seq.frames.erase(seq.frames.begin() + seqContextIndex);
            }
            selectedItems.clear(); // Sécurité
            bSeqContextMenuOpen = false;
            return true;
        }
        bSeqContextMenuOpen = false; // Ferme le menu si on clique ailleurs
        return true; 
    }

    // Clic Droit : Ouverture du menu contextuel interne
    if (button == 2) {
        ofVec2f worldM = ofVec2f((x - seq.pan.x) / seq.zoom, (y - seq.pan.y) / seq.zoom);
        seqContextType = SeqItemType::NONE;
        seqContextIndex = -1;
        
        for(size_t i=0; i<seq.buttons.size(); i++) {
            if(seq.buttons[i].rect.inside(worldM) || seq.buttons[i].labelRect.inside(worldM)) {
                seqContextType = SeqItemType::BUTTON; seqContextIndex = i; break;
            }
        }
        if(seqContextType == SeqItemType::NONE) {
            for(size_t i=0; i<seq.notes.size(); i++) {
                if(seq.notes[i]->rect.inside(worldM)) {
                    seqContextType = SeqItemType::NOTE; seqContextIndex = i; break;
                }
            }
        }
        if(seqContextType == SeqItemType::NONE) {
            for(size_t i=0; i<seq.frames.size(); i++) {
                if(seq.frames[i]->isEdgeHit(worldM)) { // On utilise isEdgeHit pour correspondre à l'édition
                    seqContextType = SeqItemType::FRAME; seqContextIndex = i; break;
                }
            }
        }
        
        if (seqContextType != SeqItemType::NONE) {
            bSeqContextMenuOpen = true;
            seqDuplicateRect.set(x, y, 120, 25);
            seqDeleteRect.set(x, y + 25, 120, 25);
        }
        return true;
    }

    if (seq.viewsUI.mousePressed(x, y, seq.pan, seq.zoom)) return true;
    
    if(exitBtnRect.inside(x, y)) { activeSeqIdx = -1; return true; }
    if(editBtnRect.inside(x, y)) { bEditMode = !bEditMode; selectedItems.clear(); return true; }
    if(saveBtnRect.inside(x, y)) { seq.save(); return true; }
    if(addNoteBtnRect.inside(x, y)) {
        auto note = std::make_shared<PlaylistTextNote>(ofVec2f((ofGetWidth()/2 - seq.pan.x)/seq.zoom, (ofGetHeight()/2 - seq.pan.y)/seq.zoom));
        note->bIsEditing = true;
        seq.notes.push_back(note);
        editingNote = note;
        return true;
    }
    if(addFrameBtnRect.inside(x, y)) {
        auto frame = std::make_shared<PlaylistVisualFrame>(ofVec2f((ofGetWidth()/2 - seq.pan.x)/seq.zoom, (ofGetHeight()/2 - seq.pan.y)/seq.zoom));
        seq.frames.push_back(frame);
        return true;
    }
    if(vuesBtnRect.inside(x, y)) {
        seq.viewsUI.toggle(seq.pan, seq.zoom, vuesBtnRect.getRight(), vuesBtnRect.y);
        return true;
    }
    
    ofVec2f worldM = ofVec2f((x - seq.pan.x) / seq.zoom, (y - seq.pan.y) / seq.zoom);
    
    if (editingNote && !editingNote->rect.inside(worldM)) {
        editingNote->bIsEditing = false;
        editingNote = nullptr;
    }
    
    // Recherche d'un élément cliqué
    void* clickedItem = nullptr;
    for(auto& b : seq.buttons) { if(b.rect.inside(worldM) || b.labelRect.inside(worldM)) { clickedItem = &b; break; } }
    if(!clickedItem) for(auto& n : seq.notes) { if(n->rect.inside(worldM)) { clickedItem = n.get(); break; } }
    if(!clickedItem) for(auto& f : seq.frames) { if(f->isEdgeHit(worldM)) { clickedItem = f.get(); break; } }
    
    // Mode Edit via Cmd + Clic dans le vide
    bool isCmdPressed = ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_SUPER) || ofGetKeyPressed(OF_KEY_CONTROL);
    if (!clickedItem && isCmdPressed) {
        bEditMode = !bEditMode; selectedItems.clear(); bIsSelecting = false; bIsDraggingGroup = false; return true;
    }
    
    if(bEditMode) {
        for (auto& frame : seq.frames) {
            if (std::find(selectedItems.begin(), selectedItems.end(), frame.get()) != selectedItems.end()) {
                if (frame->isResizeHit(worldM)) {
                    resizingFrame = frame; return true;
                }
            }
        }
        
        for (void* item : selectedItems) {
            for (auto& b : seq.buttons) {
                if (&b == item) {
                    ofRectangle handle(b.labelRect.getRight() - 10, b.labelRect.getBottom() - 10, 20, 20);
                    if (handle.inside(worldM)) {
                        resizingButton = &b; return true;
                    }
                }
            }
        }
        if(clickedItem) {
            if(std::find(selectedItems.begin(), selectedItems.end(), clickedItem) == selectedItems.end()) {
                selectedItems.clear(); selectedItems.push_back(clickedItem);
            }
            dragOffsets.clear();
            for(void* item : selectedItems) {
                ofRectangle* r = nullptr;
                for(auto& b : seq.buttons) if(&b == item) r = &b.rect;
                for(auto& n : seq.notes) if(n.get() == item) r = &n->rect;
                for(auto& f : seq.frames) if(f.get() == item) r = &f->rect;
                if(r) dragOffsets.push_back(worldM - ofVec2f(r->x, r->y));
            }
            bIsDraggingGroup = true;
        } else {
            selectedItems.clear();
            bIsSelecting = true;
            selectionStart = selectionEnd = worldM;
        }
        return true;
    }
    
    // Mode normal : Interactions
    for(auto& n : seq.notes) {
        if(n->rect.inside(worldM)) { n->bIsEditing = true; editingNote = n; return true; }
    }
    
    for(auto& b : seq.buttons) {
        if(b.rect.inside(worldM)) {
            std::string stateToApply = (b.states.size() > b.currentStateIdx) ? b.states[b.currentStateIdx] : "TRIGGER";
            applyButtonState(b.originalName, stateToApply, app);
            return true;
        }
        if(b.labelRect.inside(worldM)) {
            if(!b.states.empty()) b.currentStateIdx = (b.currentStateIdx + 1) % b.states.size();
            return true;
        }
    }
    
    bIsDraggingPan = true;
    lastMouse.set(x, y); // Repare le bug de saut de vue
    return true;
}

void PlaylistSequenceUI::mouseDragged(int x, int y, int button) {
    if(activeSeqIdx == -1) return;
    if(bSeqContextMenuOpen) return;
    PlaylistSequence& seq = sequences[activeSeqIdx];
    
    if(isSpacePressed || bIsDraggingPan) {
        ofVec2f cur(x, y);
        seq.pan += (cur - lastMouse);
        lastMouse = cur;
        return;
    }
    
    if(bEditMode) {
        ofVec2f worldM = ofVec2f((x - seq.pan.x) / seq.zoom, (y - seq.pan.y) / seq.zoom);
        if(resizingFrame) {
            resizingFrame->rect.width = std::max(20.0f, worldM.x - resizingFrame->rect.x);
            resizingFrame->rect.height = std::max(20.0f, worldM.y - resizingFrame->rect.y);
            return;
        }
        if(resizingButton) {
            float newW = worldM.x - resizingButton->rect.x;
            float newH = worldM.y - resizingButton->rect.y;
            resizingButton->rect.width = std::max(50.0f, newW);
            resizingButton->rect.height = std::max(20.0f, newH - 20.0f);
            resizingButton->labelRect.width = resizingButton->rect.width;
            resizingButton->labelRect.y = resizingButton->rect.getBottom();
            return;
        }
        if(bIsDraggingGroup && selectedItems.size() == dragOffsets.size()) {
            for(size_t i=0; i<selectedItems.size(); i++) {
                void* item = selectedItems[i];
                ofVec2f newPos = worldM - dragOffsets[i];
                for(auto& b : seq.buttons) { 
                    if(&b == item) { b.rect.x = newPos.x; b.rect.y = newPos.y; b.labelRect.x = newPos.x; b.labelRect.y = b.rect.getBottom(); }
                }
                for(auto& n : seq.notes) if(n.get() == item) { n->rect.x = newPos.x; n->rect.y = newPos.y; }
                for(auto& f : seq.frames) if(f.get() == item) { f->rect.x = newPos.x; f->rect.y = newPos.y; }
            }
        } else if(bIsSelecting) {
            selectionEnd = worldM;
            float x1 = std::min(selectionStart.x, selectionEnd.x);
            float y1 = std::min(selectionStart.y, selectionEnd.y);
            float x2 = std::max(selectionStart.x, selectionEnd.x);
            float y2 = std::max(selectionStart.y, selectionEnd.y);
            ofRectangle selRect(x1, y1, x2 - x1, y2 - y1);
            
            selectedItems.clear();
            for(auto& b : seq.buttons) {
                if(selRect.intersects(b.rect) || selRect.intersects(b.labelRect)) {
                    selectedItems.push_back(&b);
                }
            }
            for(auto& n : seq.notes) {
                if(selRect.intersects(n->rect)) selectedItems.push_back(n.get());
            }
            for(auto& f : seq.frames) {
                if(selRect.x <= f->rect.x && selRect.y <= f->rect.y && 
                   selRect.getRight() >= f->rect.getRight() && selRect.getBottom() >= f->rect.getBottom()) {
                    selectedItems.push_back(f.get());
                }
            }
        }
    }
    lastMouse.set(x, y);
}

void PlaylistSequenceUI::mouseReleased(int x, int y, int button) {
    bIsDraggingGroup = false;
    bIsSelecting = false;
    resizingFrame = nullptr;
    resizingButton = nullptr;
    bIsDraggingPan = false;
}

void PlaylistSequenceUI::mouseScrolled(int x, int y, float scrollY) {
    if(activeSeqIdx == -1 || scrollY == 0) return;
    if(bSeqContextMenuOpen) { bSeqContextMenuOpen = false; return; }
    PlaylistSequence& seq = sequences[activeSeqIdx];
    
    ofVec2f worldM = ofVec2f((x - seq.pan.x) / seq.zoom, (y - seq.pan.y) / seq.zoom);
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    seq.zoom = ofClamp(seq.zoom * zoomFactor, 0.01f, 50.0f);
    seq.pan.x = x - worldM.x * seq.zoom;
    seq.pan.y = y - worldM.y * seq.zoom;
}

void PlaylistSequenceUI::keyPressed(int key, PlaylistVisualizerApp* app) {
    if (bContextMenuOpen && key == OF_KEY_ESC) { bContextMenuOpen = false; return; }
    if(activeSeqIdx == -1) return;
    if(bSeqContextMenuOpen && key == OF_KEY_ESC) { bSeqContextMenuOpen = false; return; }
    PlaylistSequence& seq = sequences[activeSeqIdx];
    
    if (seq.viewsUI.isVisible() && seq.viewsUI.keyPressed(key, seq.pan, seq.zoom)) return;
    
    if (editingNote) {
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if (!editingNote->text.empty()) { editingNote->text.pop_back(); editingNote->updateSize(); }
        } else if (key == OF_KEY_RETURN) { editingNote->text += '\n'; editingNote->updateSize();
        } else if (key == OF_KEY_ESC) { editingNote->bIsEditing = false; editingNote = nullptr;
        } else if (key >= 32 && key <= 126) { editingNote->text += (char)key; editingNote->updateSize(); }
        return;
    }
    
    if (key == 'r' || key == 'R') { seq.pan.set(250, 0); seq.zoom = 1.0f; }
    
    if(bEditMode && !selectedItems.empty() && (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL)) {
        seq.buttons.erase(std::remove_if(seq.buttons.begin(), seq.buttons.end(), [&](SeqButton& b){
            return std::find(selectedItems.begin(), selectedItems.end(), &b) != selectedItems.end();
        }), seq.buttons.end());
        seq.notes.erase(std::remove_if(seq.notes.begin(), seq.notes.end(), [&](std::shared_ptr<PlaylistTextNote>& n){
            return std::find(selectedItems.begin(), selectedItems.end(), n.get()) != selectedItems.end();
        }), seq.notes.end());
        seq.frames.erase(std::remove_if(seq.frames.begin(), seq.frames.end(), [&](std::shared_ptr<PlaylistVisualFrame>& f){
            return std::find(selectedItems.begin(), selectedItems.end(), f.get()) != selectedItems.end();
        }), seq.frames.end());
        selectedItems.clear();
    }
}

std::vector<std::string> PlaylistSequenceUI::getButtonStates(std::string btnName, PlaylistVisualizerApp* app) {
    if (btnName == "GAB 0") return {"100%", "75%", "33%", "10%", "OFF"};
    if (btnName == "GAB 1") return {"ON", "OFF", "Scene2D", "Zenit"};
    if (btnName == "GAB 2" || btnName == "GAB 4") return {"Opaque", "Transp", "FBOs", "Rien"};
    if (btnName == "GAB 3") return {"OFF", "33%", "75%", "100%"};
    if (btnName == "Room Alpha") return {"100%", "75%", "33%", "0%"};
    if (btnName == "LAY:ZENIT") return {"SOL", "TOP", "JAR", "JAR+C"};
    if (btnName == "FMT") return {"PNG", "JPG"};
    if (btnName == "Q") return {"BEST", "HIGH", "MED", "LOW", "WORST"};
    
    if (app) {
        for(auto& t : app->controlsUI.roomToggles) if(t.name == btnName) return {"ON", "OFF", "TOGGLE"};
        for(auto& t : app->controlsUI.layerToggles) if(t.name == btnName) return {"ON", "OFF", "TOGGLE"};
    }
    
    if (btnName == "LOOP MODE" || btnName == "GO PLAYLIST" || btnName == "SIMULATE 32 VID" || 
        btnName == "SPEED x2" || btnName == "MUTE" || btnName == "CROP 106%" || 
        btnName == "USE DISK IMGS" || btnName == "HOLD LAST FRAME" || 
        btnName == "FOCUS ANNEXE" || btnName == "ANNEXE ON/OFF" || 
        btnName == "DIF:ROOM" || btnName == "DIF:SCENE2D" ||
            btnName == "AutoS Pause" || btnName == "GOLM Pause" || btnName == "Jupy Pause" || btnName == "Play Tuyau" || btnName == "Pause Carref") return {"ON", "OFF", "TOGGLE"};
        
    if (btnName.substr(0, 1) == "V" && btnName.find(" WIN") == std::string::npos && btnName.find(":") == std::string::npos) return {"ON", "OFF", "TOGGLE"};
    if (btnName.find(" WIN") != std::string::npos) return {"ON", "OFF", "TOGGLE"};
    if (btnName == "MAIN" || btnName == "W:ROOM" || btnName == "X:ZENI" || btnName == "C:SCEN" || btnName == "V:PREV" || btnName == "B:BTNS") {
        return {"ON", "OFF", "TOGGLE"};
    }
    
    return {"TRIGGER"};
}

void PlaylistSequenceUI::applyButtonState(std::string btnName, std::string state, PlaylistVisualizerApp* app) {
    if(!app || !app->mainAppPtr) return;
    ofApp* main = app->mainAppPtr;
    
    if (btnName == "GAB 0") {
        if (state == "100%") main->gabMode = 0; else if (state == "75%") main->gabMode = 1; else if (state == "33%") main->gabMode = 2; else if (state == "10%") main->gabMode = 3; else if (state == "OFF") main->gabMode = 4;
    }
    else if (btnName == "GAB 1" && main->roomApp) {
        if (state == "ON") { main->roomApp->bgMode = 0; main->roomApp->wallAlpha = 100.0f; }
        else if (state == "OFF") { main->roomApp->bgMode = 1; main->roomApp->wallAlpha = 0.0f; }
        else if (state == "Scene2D") { main->roomApp->bgMode = 2; main->roomApp->wallAlpha = 100.0f; }
        else if (state == "Zenit") { main->roomApp->bgMode = 3; main->roomApp->wallAlpha = 100.0f; }
    }
    else if (btnName == "GAB 2" && main->scene2D) {
        if (state == "Opaque") main->scene2D->bgDisplayMode = 0; else if (state == "Transp") main->scene2D->bgDisplayMode = 1; else if (state == "FBOs") main->scene2D->bgDisplayMode = 2; else if (state == "Rien") main->scene2D->bgDisplayMode = 3;
    }
    else if (btnName == "GAB 3" && main->scene2D) {
        if (state == "OFF") main->scene2D->overlayMode = 0; else if (state == "33%") main->scene2D->overlayMode = 1; else if (state == "75%") main->scene2D->overlayMode = 2; else if (state == "100%") main->scene2D->overlayMode = 3;
    }
    else if (btnName == "GAB 4" && main->sceneZenit) {
        if (state == "Opaque") main->sceneZenit->bgDisplayMode = 0; else if (state == "Transp") main->sceneZenit->bgDisplayMode = 1; else if (state == "FBOs") main->sceneZenit->bgDisplayMode = 2; else if (state == "Rien") main->sceneZenit->bgDisplayMode = 3;
    }
    else if (btnName == "DIF:ROOM") { 
        if (state == "ON") main->bDiffuseRoom = true; else if (state == "OFF") main->bDiffuseRoom = false; else main->bDiffuseRoom = !main->bDiffuseRoom;
    }
    else if (btnName == "DIF:SCENE2D") { 
        if (state == "ON") main->bDiffuseScene2D = true; else if (state == "OFF") main->bDiffuseScene2D = false; else main->bDiffuseScene2D = !main->bDiffuseScene2D;
    }
    else if (btnName == "LAY:ZENIT" && main->sceneZenit) {
        if (state == "SOL") main->sceneZenit->layoutMode = 0; else if (state == "TOP") main->sceneZenit->layoutMode = 1; else if (state == "JAR") main->sceneZenit->layoutMode = 2; else if (state == "JAR+C") main->sceneZenit->layoutMode = 3;
    }
    else if (btnName == "FMT") {
        if (state == "PNG") main->recordFormat = "png";
        else if (state == "JPG") main->recordFormat = "jpg";
        else app->windowControlsUI.mousePressed(app->windowControlsUI.formatBtn.getCenter(), main);
    }
    else if (btnName == "Q") {
        if (state == "BEST") main->qualityIndex = 1;
        else if (state == "HIGH") main->qualityIndex = 0;
        else if (state == "MED") main->qualityIndex = 2;
        else if (state == "LOW") main->qualityIndex = 3;
        else if (state == "WORST") main->qualityIndex = 4;
        
        if (main->qualityIndex == 0) main->recordQuality = OF_IMAGE_QUALITY_HIGH;
        else if(main->qualityIndex == 1) main->recordQuality = OF_IMAGE_QUALITY_BEST;
        else if(main->qualityIndex == 2) main->recordQuality = OF_IMAGE_QUALITY_MEDIUM;
        else if(main->qualityIndex == 3) main->recordQuality = OF_IMAGE_QUALITY_LOW;
        else if(main->qualityIndex == 4) main->recordQuality = OF_IMAGE_QUALITY_WORST;
    }
    else if (btnName == "Room Alpha" && main->roomApp) {
        if (state == "100%") { main->roomApp->wallAlpha = 255.0f; if (main->roomApp->bgMode == 1) main->roomApp->bgMode = 0; }
        else if (state == "75%") { main->roomApp->wallAlpha = 191.0f; if (main->roomApp->bgMode == 1) main->roomApp->bgMode = 0; }
        else if (state == "33%") { main->roomApp->wallAlpha = 84.0f; if (main->roomApp->bgMode == 1) main->roomApp->bgMode = 0; }
        else if (state == "0%") { main->roomApp->wallAlpha = 0.0f; }
    }
    else if (btnName == "LOOP MODE" && app->player) {
        bool cur = app->player->isLoopMode();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleLoopMode();
    }
    else if (btnName == "GO PLAYLIST" && app->player) {
        bool cur = app->player->isActive();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") {
            app->player->toggle();
            if (app->bDrawScene360VideoPtr) *(app->bDrawScene360VideoPtr) = app->player->isActive();
        }
    }
    else if (btnName == "SIMULATE 32 VID" && app->player) {
        bool cur = app->player->isSimulating32Videos();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleSimulate32Videos();
    }
    else if (btnName == "SPEED x2" && app->player) {
        bool cur = app->player->isDoubleSpeed();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleDoubleSpeed();
    }
    else if (btnName == "MUTE" && app->player) {
        bool cur = app->player->isMuted();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleMute();
    }
    else if (btnName == "CROP 106%" && app->player) {
        bool cur = app->player->isCrop106();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleCrop106();
    }
    else if (btnName == "USE DISK IMGS" && app->player) {
        bool cur = app->player->isUsingDiskPauseImages();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleUseDiskPauseImages();
    }
    else if (btnName == "HOLD LAST FRAME" && app->player) {
        bool cur = app->player->isInfinitePause();
        if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") app->player->toggleInfinitePause();
    }
        else if (btnName == "Play Tuyau" && main->roomApp) {
            bool cur = main->roomApp->tuyau.bIsPlaying;
            if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") main->roomApp->tuyau.bIsPlaying = !cur;
        }
        else if (btnName == "Pause Carref" && main->roomApp) {
            bool cur = main->roomApp->tuyau.bPauseAtJunction;
            if ((state == "ON" && !cur) || (state == "OFF" && cur) || state == "TOGGLE") main->roomApp->tuyau.bPauseAtJunction = !cur;
        }
    else if (btnName == "FOCUS ANNEXE") {
        if (main->annexeApp) {
            auto focusWin = [](std::shared_ptr<ofAppBaseWindow> win) {
                if(win) {
                    auto glfwWin = std::dynamic_pointer_cast<ofAppGLFWWindow>(win);
                    if(glfwWin) {
                        glfwShowWindow(glfwWin->getGLFWWindow());
                        glfwRestoreWindow(glfwWin->getGLFWWindow());
                        glfwFocusWindow(glfwWin->getGLFWWindow());
                    }
                }
            };
            focusWin(main->annexeWindowPtr);
            focusWin(main->annexePlayerWindowPtr);
        }
    }
    else if (btnName == "ANNEXE ON/OFF") {
        if ((state == "ON" && app->bAnnexesHidden) || (state == "OFF" && !app->bAnnexesHidden) || state == "TOGGLE") {
            app->bAnnexesHidden = !app->bAnnexesHidden;
            if (main->annexeApp) main->annexeApp->setEnabled(!app->bAnnexesHidden);
            if (main->annexePlayerApp) main->annexePlayerApp->setEnabled(!app->bAnnexesHidden);
        }
    }
    else {
        bool handled = false;
        for(auto& t : app->controlsUI.roomToggles) {
            if (t.name == btnName) {
                bool curState = t.getState();
                if ((state == "ON" && !curState) || (state == "OFF" && curState) || state == "TOGGLE") t.toggle();
                handled = true; break;
            }
        }
        if (!handled) {
            for(auto& t : app->controlsUI.layerToggles) {
                if (t.name == btnName) {
                    bool curState = *(t.valuePtr);
                    if ((state == "ON" && !curState) || (state == "OFF" && curState) || state == "TOGGLE") {
                        app->controlsUI.mousePressed(t.rect.getCenter(), app->scene2D, main->roomApp.get());
                    }
                    handled = true; break;
                }
            }
        }
        if (!handled) {
            for(int i=0; i<4; i++) {
                if (btnName == "V" + ofToString(i+1)) {
                    bool curState = !app->windowControlsUI.viewHidden[i];
                    if ((state == "ON" && !curState) || (state == "OFF" && curState) || state == "TOGGLE") app->windowControlsUI.mousePressed(app->windowControlsUI.viewBtns[i].getCenter(), main);
                    handled = true; break;
                }
                if (btnName == "V" + ofToString(i+1) + " WIN") {
                    bool curState = !app->windowControlsUI.bWinHidden[i];
                    if ((state == "ON" && !curState) || (state == "OFF" && curState) || state == "TOGGLE") app->windowControlsUI.mousePressed(app->windowControlsUI.toggleWinBtns[i].getCenter(), main);
                    handled = true; break;
                }
            }
        }
        if (!handled) {
            for(int i=0; i<6; i++) {
                if (btnName == app->windowControlsUI.wxcvbNames[i]) {
                    bool isOn = false;
                    if(i == 0) isOn = main->bDrawMain;
                    else if(i == 1) isOn = main->bDrawRoom;
                    else if(i == 2) isOn = main->bDrawZenit;
                    else if(i == 3) isOn = main->bDrawScene2D;
                    else if(i == 4 && main->roomPreviewApp) isOn = !main->roomPreviewApp->bPaused;
                    else if(i == 5) isOn = main->bDrawButtons;
                    
                    if ((state == "ON" && !isOn) || (state == "OFF" && isOn) || state == "TOGGLE") {
                        app->windowControlsUI.mousePressed(app->windowControlsUI.wxcvbBtns[i].getCenter(), main);
                    }
                    handled = true; break;
                }
            }
        }
        
        if (handled) return; // Si ca a ete gere plus haut, on arrete la
        
        for (auto& btn : app->getAllSearchableButtons()) {
            if (btn.name == btnName && btn.rect != nullptr) {
                ofVec2f btnPos = btn.rect->getCenter();
                if (app->windowControlsUI.mousePressed(btnPos, main)) break;
                if (app->controlsUI.mousePressed(btnPos, app->scene2D, main->roomApp.get())) break;
                if (app->playerUI.mousePressed(btnPos, app->player, app->bDrawScene360VideoPtr)) break;
                if (app->geminiUI.mousePressed(btnPos, main)) break;
                if (btnName == "CREER ANNEXE") {
                    main->createAnnexeWindows();
                }
                break;
            }
        }
    }
}