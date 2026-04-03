#include "PlaylistVisualizerApp.h"
#include <algorithm>
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "ofApp.h"
#include "AnnexeApp.h"
#include "AnnexePlayerApp.h"
#include "ofAppGLFWWindow.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "PlaylistWinPosUI.h"

shared_ptr<ofAppBaseWindow> PlaylistVisualizerApp::getAppWindow(int index) {
    if(!mainAppPtr) return nullptr;
    switch(index) {
        case 0: return mainAppPtr->mainWindowPtr;
        case 1: return mainAppPtr->roomWindowPtr;
        case 2: return mainAppPtr->zenitWindowPtr;
        case 3: return mainAppPtr->scene2DWindowPtr;
        case 4: return mainAppPtr->previewWindowPtr;
        case 5: return mainAppPtr->buttonWindowPtr;
        case 6: return mainAppPtr->playlistWindowPtr;
    }
    return nullptr;
}

void PlaylistVisualizerApp::setup() {
    ofSetBackgroundColor(25);
    playerUI.setup();
    
    windowControlsUI.setup();

    zoom = 1.0f;
    pan.set(250, 0); // Ajusté pour le nouvel agencement
    isSpacePressed = false;
    
    tooltipManager.setup();
    
    editBtnRect.set(10, 40, 60, 30);
    saveBtnRect.set(10, 80, 60, 30);
    loadBtnRect.set(10, 120, 60, 30);
    addNoteBtnRect.set(10, 160, 60, 30);
    addFrameBtnRect.set(10, 200, 60, 30);
    searchBtnRect.set(10, 240, 60, 30);
    histBtnRect.set(10, 280, 60, 30);
    vuesBtnRect.set(10, 320, 60, 30);
    posWinBtnRect.set(10, 360, 60, 30);
    
    // Init Gemini UI
    geminiUI.setup();
    
    searchBar.setup();
    historyUI.setup();
    viewsUI.setup();
    winPosUI.setup();
    
    // Positions par défaut des Drop Zones
    float boxSize = 406.0f;
    diagramDropZone.set(375.0f, 1438.0f, boxSize, boxSize);
    textureUI.textureDropZone.set(849.0f, 1423.0f, boxSize, boxSize);
    imageGraphDropZone.set(1300.0f, 1423.0f, boxSize, boxSize);
    
    sequenceUI.setup();
    
    loadButtonPositions();
}

void PlaylistVisualizerApp::update() {
    bool bJustSetup = false;
    if(scene2D && controlsUI.layerToggles.empty()) {
        controlsUI.setupLayerToggles(scene2D);
        bJustSetup = true;
    }
    if(roomApp && controlsUI.roomToggles.empty()) {
        controlsUI.setupRoomToggles(roomApp, mainAppPtr);
        bJustSetup = true;
    }
    if(roomApp && controlsUI.roomActionBtns.empty()) {
        controlsUI.setupRoomActionBtns(roomApp);
        bJustSetup = true;
    }
    if(controlsUI.globalActionBtns.empty()) {
        controlsUI.setupGlobalActionBtns(mainAppPtr);
        bJustSetup = true;
    }
    if (bJustSetup) loadButtonPositions();

    if (roomApp && imageGraphPlayer.atmosphere == nullptr) {
        imageGraphPlayer.setup(&roomApp->atmosphere);
    }

    if (player && !pendingVideoFolder.empty()) {
        // Evite de relancer la playlist et de couper la vidéo si c'est déjà le bon dossier
        if (player->getFolderPath() != pendingVideoFolder) {
            player->startPlaylist(pendingVideoFolder, false); // Ne démarre pas automatiquement au chargement JSON
        }
        pendingVideoFolder = "";
    }
    
    if (roomApp && !textureUI.currentFolderPath.empty() && textureUI.textureFiles.empty()) {
        textureUI.loadFolder(textureUI.currentFolderPath, nullptr); // Ne charge pas la 1ere image dans la room au demarrage
    }

    // --- MISE A JOUR DYNAMIQUE DE L'ACCORDEON (ASCENSEUR) ---
    playerUI.update();
    geminiUI.update(mainAppPtr);
    textureUI.update(roomApp);
    
    imageGraphPlayer.update();

    sequenceUI.update(this);

    // Pression continue pour les boutons d'action qui le supportent (rotation, etc.)
    if (bEnabled && ofGetMousePressed(0) && !isSpacePressed && !bIsDraggingPan && !bEditMode) {
        ofVec2f worldM = getTransformedMouse(ofGetMouseX(), ofGetMouseY());
        controlsUI.handleContinuousActions(worldM);
    }

    if(!player) return;

    float rx = std::max(10.0f, (diagramDropZone.width - 150.0f) / 2.0f);
    float ry = std::max(10.0f, (diagramDropZone.height - 150.0f) / 2.0f);
    nodeGraph.update(player, diagramDropZone.getCenter().x, diagramDropZone.getCenter().y, rx, ry);
}

void PlaylistVisualizerApp::dragEvent(ofDragInfo dragInfo) {
    if(!bEnabled) return;
    
    ofVec2f dropPos = getTransformedMouse(dragInfo.position.x, dragInfo.position.y);
    
    if(dragInfo.files.size() > 0) {
        string path = dragInfo.files[0];
        ofFile file(path);
        if(file.isDirectory()) {
            if(diagramDropZone.inside(dropPos)) {
                if (player) {
                    if(player->isSimulating32Videos()) {
                        player->toggleSimulate32Videos();
                    }
                    player->startPlaylist(path);
                    if(bDrawScene360VideoPtr) *bDrawScene360VideoPtr = true; // Lance l'affichage dans la room
                    ofLogNotice("PlaylistVisualizerApp") << "Nouvelle playlist chargee via drag&drop : " << path;
                }
            } else if(imageGraphDropZone.inside(dropPos)) {
                imageGraphPlayer.loadFolder(path, imageGraphDropZone);
                if (roomApp) roomApp->bDrawAtmosphere = true;
                ofLogNotice("PlaylistVisualizerApp") << "Dossier d'images charge : " << path;
            } else {
                textureUI.handleFolderDrop(path, dropPos, roomApp);
            }
        }
    }
}

void PlaylistVisualizerApp::draw() {
    if(!bEnabled) {
        ofBackground(0);
        return;
    }

    ofBackground(25);
    
    ofPushMatrix();
    ofTranslate(pan);
    ofScale(zoom, zoom);

    // DESSIN DES CADRES EN PREMIER (Pour qu'ils soient en arrière-plan)
    for(auto& frame : visualFrames) {
        bool isSelected = (bEditMode && std::find(selectedRects.begin(), selectedRects.end(), &frame->rect) != selectedRects.end());
        frame->draw(isSelected);
    }

    controlsUI.draw(mainAppPtr);

    // --- DESSIN DES BOUTONS DE LECTURE (TOUJOURS VISIBLES) ---
    playerUI.draw(player, nodeGraph);

    // --- CALCUL ET DESSIN DE LA ZONE DU DIAGRAMME ---
    nodeGraph.draw(player, diagramDropZone);
    
    imageGraphPlayer.draw(imageGraphDropZone);
    
    textureUI.draw();
    
    for(auto& note : textNotes) {
        note->draw();
    }
    
    // Dessiné en dernier pour que les accordéons (GAB) s'affichent par-dessus les autres éléments
    windowControlsUI.draw(mainAppPtr);

    // --- DETECTION DES TOOLTIPS (Au survol) ---
    ofVec2f worldM = getTransformedMouse(ofGetMouseX(), ofGetMouseY());
    string hoveredTooltip = "";

    string winTip = windowControlsUI.getTooltip(worldM, tooltipManager);
    if(!winTip.empty()) hoveredTooltip = winTip;

    string controlsTip = controlsUI.getTooltip(worldM, tooltipManager);
    if (!controlsTip.empty()) hoveredTooltip = controlsTip;
    
    string playerTip = playerUI.getTooltip(worldM, tooltipManager);
    if(!playerTip.empty()) hoveredTooltip = playerTip;
    
    // --- GEMINI UI TOOLTIPS ---
    string geminiTip = geminiUI.getTooltip(worldM, tooltipManager);
    if(!geminiTip.empty()) hoveredTooltip = geminiTip;

    string graphTip = nodeGraph.getTooltip(worldM);
    if (!graphTip.empty()) hoveredTooltip = graphTip;
    
    // --- GEMINI UI ---
    geminiUI.draw();
    
    if (bEditMode) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(255, 200, 0, 255); // Jaune pour les éléments sélectionnés
        ofSetLineWidth(2);
        for(auto* r : selectedRects) {
            ofDrawRectangle(r->x - 2, r->y - 2, r->width + 4, r->height + 4);
            // Dessin de la petite poignée en bas à droite
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
            
            ofFill();
            ofSetColor(100, 150, 255, 50); // Fond bleu transparent
            ofDrawRectangle(selRect);
            ofNoFill();
            ofSetColor(100, 150, 255, 200); // Bordure bleue
            ofSetLineWidth(1);
            ofDrawRectangle(selRect);
        }
        ofPopStyle();
    }
    
    ofPopMatrix();

    // HUD FIXE (Non affecté par le zoom/pan)
    ofPushStyle();
    ofVec2f hudWorldM = getTransformedMouse(ofGetMouseX(), ofGetMouseY());
    ofDrawBitmapStringHighlight("World Mouse: X=" + ofToString(hudWorldM.x, 1) + " Y=" + ofToString(hudWorldM.y, 1), 10, ofGetHeight() - 45, ofColor(30, 30, 30, 200), ofColor(200, 255, 200));

    if(isSpacePressed || bIsDraggingPan) {
        ofDrawBitmapStringHighlight("ESPACE / CLIC DANS LE VIDE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20, ofColor(50, 200, 50), ofColor(0));
    } else {
        ofSetColor(255);
        ofDrawBitmapStringHighlight("ESPACE / CLIC DANS LE VIDE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20);
    }

    ofSetColor(255);
    ofDrawBitmapStringHighlight("PLAYLIST VISUALIZER [N]", 10, 20);
    ofPopStyle();
    
    // DESSIN DES BOUTONS EDIT / SAVE / LOAD (HUD Fixe)
    ofPushStyle();
    if(bEditMode) ofSetColor(50, 200, 50); else ofSetColor(30, 30, 30);
    ofFill(); ofDrawRectangle(editBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(editBtnRect);
    ofDrawBitmapString("EDIT", editBtnRect.x + 12, editBtnRect.y + 20);
    
    bool showSaveFeedback = (ofGetElapsedTimef() - saveFeedbackTimer < 1.0f);
    if(showSaveFeedback) ofSetColor(50, 200, 50); else ofSetColor(80);
    ofFill(); ofDrawRectangle(saveBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(saveBtnRect);
    if(showSaveFeedback) {
        ofDrawBitmapString("SAVED", saveBtnRect.x + 8, saveBtnRect.y + 20);
    } else {
        ofDrawBitmapString("SAVE", saveBtnRect.x + 12, saveBtnRect.y + 20);
    }
    
    ofSetColor(80); // Réinitialise la couleur à gris foncé pour le bouton LOAD
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
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(histBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(histBtnRect);
    ofDrawBitmapString("HIST", histBtnRect.x + 14, histBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(vuesBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(vuesBtnRect);
    ofDrawBitmapString("VUES", vuesBtnRect.x + 14, vuesBtnRect.y + 20);
    
    ofSetColor(80);
    ofFill(); ofDrawRectangle(posWinBtnRect);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(posWinBtnRect);
    ofDrawBitmapString("POS WIN", posWinBtnRect.x + 5, posWinBtnRect.y + 20);
    
    if (mainAppPtr) {
        float btnW = 100;
        float btnH = 22;
        float margin = 10;

        if (mainAppPtr->annexeApp) {
            // Les fenêtres Annexe existent, on affiche les boutons de contrôle
            toggleAnnexesBtnRect.set(ofGetWidth() - btnW - margin, margin, btnW, btnH);
            focusAnnexesBtnRect.set(toggleAnnexesBtnRect.x - btnW - margin, margin, btnW, btnH);

            ofSetColor(50, 150, 200);
            ofFill(); ofDrawRectangle(focusAnnexesBtnRect);
            ofNoFill(); ofSetColor(255); ofDrawRectangle(focusAnnexesBtnRect);
            string focusText = "FOCUS ANNEXE";
            ofDrawBitmapString(focusText, focusAnnexesBtnRect.x + (btnW - focusText.length() * 8) / 2, focusAnnexesBtnRect.y + 15);
            
            ofSetColor(bAnnexesHidden ? ofColor(150, 50, 50) : ofColor(50, 150, 200));
            ofFill(); ofDrawRectangle(toggleAnnexesBtnRect);
            ofNoFill(); ofSetColor(255); ofDrawRectangle(toggleAnnexesBtnRect);
            string toggleText = bAnnexesHidden ? "ANNEXE OFF" : "ANNEXE ON";
            ofDrawBitmapString(toggleText, toggleAnnexesBtnRect.x + (btnW - toggleText.length() * 8) / 2, toggleAnnexesBtnRect.y + 15);
        } else {
            // Les fenêtres n'existent pas, on affiche le bouton de création
            btnW = 120; // Un peu plus large
            createAnnexeBtnRect.set(ofGetWidth() - btnW - margin, margin, btnW, btnH);

            ofSetColor(80, 180, 80);
            ofFill(); ofDrawRectangle(createAnnexeBtnRect);
            ofNoFill(); ofSetColor(255); ofDrawRectangle(createAnnexeBtnRect);
            string createText = "CREER ANNEXE";
            ofDrawBitmapString(createText, createAnnexeBtnRect.x + (btnW - createText.length() * 8) / 2, createAnnexeBtnRect.y + 15);
        }
    }
    ofPopStyle();
    
    // Dessin du tooltip par-dessus tout, non affecte par le Zoom (en coordonnees ecran brutes)
    if(hoveredTooltip.empty()) { // S'il n'y a pas déjà un tooltip du "monde", on vérifie le HUD
        float mx = ofGetMouseX();
        float my = ofGetMouseY();
        if(editBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("EDIT");
        else if(saveBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("SAVE");
        else if(loadBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("LOAD");
        else if(addNoteBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("+ NOTE");
        else if(addFrameBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("+ FRAME");
        else if(searchBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("SEARCH");
        else if(histBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("HIST");
        else if(vuesBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("VUES");
        else if(posWinBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("POS_WIN");
        else if (mainAppPtr) {
            if (mainAppPtr->annexeApp) {
                if(focusAnnexesBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("FOCUS_ANNEXES");
                else if(toggleAnnexesBtnRect.inside(mx, my)) hoveredTooltip = tooltipManager.getTooltipText("TOGGLE_ANNEXES");
            } else {
                if(createAnnexeBtnRect.inside(mx, my)) hoveredTooltip = "Crée et affiche les fenêtres Annexe.";
            }
        }
    }
    if(!hoveredTooltip.empty()) { // On redessine le tooltip s'il y en a un (monde ou HUD)
        tooltipManager.drawTooltip(hoveredTooltip, ofGetMouseX(), ofGetMouseY());
    }

    // Feedback visuel curseur
    if (isSpacePressed || bIsDraggingPan) {
        ofSetColor(50, 200, 50, 150);
        ofFill();
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
        ofSetColor(255);
        ofNoFill();
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
    }
    
    // DESSIN SEARCH BAR (par-dessus tout)
    searchBar.draw();
    historyUI.draw();
    viewsUI.draw();
    winPosUI.draw();
    
    if(sequenceUI.isActive()) {
        sequenceUI.draw(this);
    }

    sequenceUI.drawTopButtons();
    sequenceUI.drawContextMenu();
}

void PlaylistVisualizerApp::mousePressed(int x, int y, int button) {
    if (sequenceUI.mousePressed(x, y, button, this)) return;
    if (sequenceUI.isActive()) return;

    if (button == 2) { // Clic droit
        ofVec2f worldM = getTransformedMouse(x, y);
        ofRectangle* clicked = findButtonAt(worldM);
        if (clicked) {
            std::string btnName = "";
            for (auto& sb : getAllSearchableButtons()) {
                if (sb.rect == clicked) { btnName = sb.name; break; }
            }
            if (!btnName.empty()) {
                sequenceUI.openContextMenu(ofVec2f(x, y), btnName); 
                return;
            }
        }
    }

    lastMouse.set(x, y);
    
    if (searchBtnRect.inside(x, y)) {
        searchBar.toggle(getAllSearchableButtons(), pan, zoom);
        if (historyUI.isVisible()) historyUI.close();
        if (viewsUI.isVisible()) viewsUI.close();
        if (winPosUI.isVisible()) winPosUI.close();
        return;
    }
    if (histBtnRect.inside(x, y)) {
        historyUI.toggle(pan, zoom);
        if (searchBar.isVisible()) searchBar.close();
        if (viewsUI.isVisible()) viewsUI.close();
        if (winPosUI.isVisible()) winPosUI.close();
        return;
    }
    if (vuesBtnRect.inside(x, y)) {
        viewsUI.toggle(pan, zoom, vuesBtnRect.getRight(), vuesBtnRect.y);
        if (searchBar.isVisible()) searchBar.close();
        if (historyUI.isVisible()) historyUI.close();
        if (winPosUI.isVisible()) winPosUI.close();
        return;
    }
    if (posWinBtnRect.inside(x, y)) {
        winPosUI.toggle(posWinBtnRect.getRight(), posWinBtnRect.y);
        if (searchBar.isVisible()) searchBar.close();
        if (historyUI.isVisible()) historyUI.close();
        if (viewsUI.isVisible()) viewsUI.close();
        return;
    }

    if (searchBar.mousePressed(x, y, pan, zoom)) return;
    if (historyUI.mousePressed(x, y, pan, zoom)) return;
    if (viewsUI.mousePressed(x, y, pan, zoom)) return;
    if (winPosUI.mousePressed(x, y, this)) return;
    
    if (!bEnabled) return;
    if (isSpacePressed) return; // Bloque le clic sur les boutons lors du déplacement
    
    
    if (mainAppPtr) {
        if (mainAppPtr->annexeApp) {
            if (focusAnnexesBtnRect.inside(x, y)) {
                auto focusWin = [](shared_ptr<ofAppBaseWindow> win) {
                    if(win) {
                        auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(win);
                        if(glfwWin) {
                            glfwShowWindow(glfwWin->getGLFWWindow());
                            glfwRestoreWindow(glfwWin->getGLFWWindow());
                            glfwFocusWindow(glfwWin->getGLFWWindow());
                        }
                    }
                };
                focusWin(mainAppPtr->annexeWindowPtr);
                focusWin(mainAppPtr->annexePlayerWindowPtr);
                historyUI.addEvent("Focus Annexes", pan, zoom);
                return;
            }
            if (toggleAnnexesBtnRect.inside(x, y)) {
                bAnnexesHidden = !bAnnexesHidden;
                if (mainAppPtr->annexeApp) mainAppPtr->annexeApp->setEnabled(!bAnnexesHidden);
                if (mainAppPtr->annexePlayerApp) mainAppPtr->annexePlayerApp->setEnabled(!bAnnexesHidden);
                historyUI.addEvent(bAnnexesHidden ? "Annexes OFF" : "Annexes ON", pan, zoom);
                return;
            }
        } else {
            if (createAnnexeBtnRect.inside(x, y)) {
                mainAppPtr->createAnnexeWindows();
                historyUI.addEvent("Create Annexe Windows", pan, zoom);
                return;
            }
        }
    }

    // Clics dans le HUD Fixe
    if (editBtnRect.inside(x, y)) { 
        bEditMode = !bEditMode; 
        selectedRects.clear(); 
        bIsSelecting = false; 
        bIsDraggingGroup = false; 
        historyUI.addEvent(bEditMode ? "Mode Edit ON" : "Mode Edit OFF", pan, zoom);
        return; 
    }
    if (saveBtnRect.inside(x, y)) { saveButtonPositions(); historyUI.addEvent("SAVE Workspace", pan, zoom); return; }
    if (loadBtnRect.inside(x, y)) { loadButtonPositions(); historyUI.addEvent("LOAD Workspace", pan, zoom); return; }
    if (addNoteBtnRect.inside(x, y)) {
        geminiUI.unfocusAll();
        if (editingNote) editingNote->bIsEditing = false;
        
        saveUndoState("Ajout Note");
        
        ofVec2f center = getTransformedMouse(ofGetWidth()/2, ofGetHeight()/2);
        auto newNote = make_shared<PlaylistTextNote>(center);
        newNote->bIsEditing = true;
        textNotes.push_back(newNote);
        editingNote = newNote;
        return;
    }
    if (addFrameBtnRect.inside(x, y)) {
        saveUndoState("Ajout Cadre");
        ofVec2f center = getTransformedMouse(ofGetWidth()/2, ofGetHeight()/2);
        auto newFrame = make_shared<PlaylistVisualFrame>(center);
        visualFrames.push_back(newFrame);
        return;
    }

    ofVec2f worldM = getTransformedMouse(x, y);
    
    // Quitter le mode édition texte si on clique en dehors
    if (editingNote && !editingNote->rect.inside(worldM)) {
        editingNote->bIsEditing = false;
        editingNote = nullptr;
    }
    
    // Unfocus des boites de texte Seed (validation auto au clic à l'extérieur)
    if ((controlsUI.bEditingGolSeed && !controlsUI.golSeedBox.inside(worldM)) ||
        (controlsUI.bEditingGolmSeed && !controlsUI.golmSeedBox.inside(worldM))) {
        controlsUI.unfocus(roomApp);
    }
    
    ofRectangle* clickedRect = findButtonAt(worldM);
    
    // Raccourci Cmd+Clic (ou Ctrl+Clic) sur le fond pour basculer le mode édition
    bool isCmdPressed = ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_SUPER) || ofGetKeyPressed(OF_KEY_CONTROL);
    if (!clickedRect && isCmdPressed) {
        bEditMode = !bEditMode;
        selectedRects.clear();
        bIsSelecting = false;
        bIsDraggingGroup = false;
        historyUI.addEvent(bEditMode ? "Mode Edit ON" : "Mode Edit OFF", pan, zoom);
        return;
    }

    // Intercepte les interactions quand l'édition est active
    if (bEditMode) {
        // Vérifier d'abord les poignées de redimensionnement des cadres
        for (auto& frame : visualFrames) {
            if (std::find(selectedRects.begin(), selectedRects.end(), &frame->rect) != selectedRects.end()) {
                if (frame->isResizeHit(worldM)) {
                    saveUndoState("Redimensionnement Cadre");
                    resizingFrame = frame;
                    return; // On bloque le deplacement classique, on passe en mode redimensionnement
                }
            }
        }
        
        // Vérifier les poignées de TOUS les rectangles sélectionnés
        for (auto* r : selectedRects) {
            // Zone de clic un peu plus large pour être attrapée facilement (tolérance)
            ofRectangle handle(r->getRight() - 10, r->getBottom() - 10, 20, 20);
            if (handle.inside(worldM)) {
                saveUndoState("Redimensionnement Element");
                resizingRect = r;
                return;
            }
        }
        
        if (clickedRect) {
            // Vérifie si on a cliqué sur un élément DÉJÀ sélectionné
            bool alreadySelected = false;
            for (auto* r : selectedRects) {
                if (r == clickedRect) { alreadySelected = true; break; }
            }
            // Sinon, on remplace la sélection par ce seul élément
            if (!alreadySelected) {
                selectedRects.clear();
                selectedRects.push_back(clickedRect);
            }
            dragOffsets.clear();
            for (auto* r : selectedRects) {
                dragOffsets.push_back(worldM - ofVec2f(r->x, r->y));
            }
            saveUndoState("Selection/Deplacement Element");
            bIsDraggingGroup = true;
        } else {
            // On clique dans le vide -> Début d'un lasso
            selectedRects.clear();
            bIsSelecting = true;
            selectionStart = worldM;
            selectionEnd = worldM;
        }
        return;
    }
    
    // --- ENREGISTREMENT LÉGER DANS L'HISTORIQUE ---
    if (!bEditMode && clickedRect) {
        string actionName = "Action UI";
        for (auto& btn : getAllSearchableButtons()) {
            if (btn.rect == clickedRect) {
                actionName = btn.name;
                break;
            }
        }
        
        if (actionName == "Action UI") {
            if (clickedRect == &diagramDropZone) actionName = "Diagram Playlist";
            else if (clickedRect == &imageGraphDropZone) actionName = "Diagram Images";
            else if (clickedRect == &textureUI.textureDropZone) actionName = "Textures";
        }
        
        historyUI.addEvent(actionName, pan, zoom);
    }

    if (!bEditMode) {
        for(auto& note : textNotes) {
            if (note->rect.inside(worldM)) {
                if (editingNote) editingNote->bIsEditing = false;
                note->bIsEditing = true;
                editingNote = note;
                return;
            }
        }
    }

    if (textureUI.mousePressed(worldM, roomApp)) return;

    if (imageGraphPlayer.mousePressed(worldM)) return;

    // --- GEMINI UI CLICKS (Dans l'espace du monde) ---
    if (geminiUI.mousePressed(worldM, mainAppPtr)) {
        return;
    } else {
        geminiUI.unfocusAll();
    }

    if (windowControlsUI.mousePressed(worldM, mainAppPtr)) return;

    if (controlsUI.mousePressed(worldM, scene2D, roomApp)) return;

    if (playerUI.mousePressed(worldM, player, bDrawScene360VideoPtr)) return;
    if (!player) {
        if (!bEditMode) bIsDraggingPan = true;
        return;
    }
    
    // Vérifie si on a cliqué sur un noeud
    if (nodeGraph.mousePressed(worldM, player)) return;
    
    if (!bEditMode) {
        bIsDraggingPan = true;
    }
}

void PlaylistVisualizerApp::mouseDragged(int x, int y, int button) {
    if (sequenceUI.isActive()) {
        sequenceUI.mouseDragged(x, y, button);
        return;
    }
    
    if(isSpacePressed || bIsDraggingPan) {
        ofVec2f currentMouse(x, y);
        pan += (currentMouse - lastMouse);
        lastMouse = currentMouse;
        return;
    }

    if (bEditMode) {
        ofVec2f worldM = getTransformedMouse(x, y);
        if (resizingFrame) {
            float newW = worldM.x - resizingFrame->rect.x;
            float newH = worldM.y - resizingFrame->rect.y;
            resizingFrame->rect.width = std::max(20.0f, newW);
            resizingFrame->rect.height = std::max(20.0f, newH);
            return;
        }
        if (resizingRect) {
            float newW = worldM.x - resizingRect->x;
            float newH = worldM.y - resizingRect->y;
            resizingRect->width = std::max(10.0f, newW);
            resizingRect->height = std::max(10.0f, newH);
            return;
        }
        if (bIsDraggingGroup && selectedRects.size() == dragOffsets.size()) {
            for(size_t i=0; i<selectedRects.size(); i++) {
                selectedRects[i]->x = worldM.x - dragOffsets[i].x;
                selectedRects[i]->y = worldM.y - dragOffsets[i].y;
            }
        } else if (bIsSelecting) {
            selectionEnd = worldM;
            float x1 = std::min(selectionStart.x, selectionEnd.x);
            float y1 = std::min(selectionStart.y, selectionEnd.y);
            float x2 = max(selectionStart.x, selectionEnd.x);
            float y2 = max(selectionStart.y, selectionEnd.y);
            ofRectangle selRect(x1, y1, x2 - x1, y2 - y1);
            
            selectedRects.clear();
            vector<ofRectangle*> allR = getAllInteractableRects();
            for(auto* r : allR) {
                bool isFrame = false;
                for(auto& frame : visualFrames) {
                    if (&frame->rect == r) { isFrame = true; break; }
                }
                
                if (isFrame) {
                    if (selRect.x <= r->x && selRect.y <= r->y && 
                        selRect.getRight() >= r->getRight() && selRect.getBottom() >= r->getBottom()) {
                        selectedRects.push_back(r);
                    }
                } else {
                    if(selRect.intersects(*r)) {
                        selectedRects.push_back(r);
                    }
                }
            }
        }
        return;
    }
    
    if (!bEditMode) {
        if(controlsUI.mouseDragged(getTransformedMouse(x, y), roomApp)) return;
    }
}

void PlaylistVisualizerApp::mouseReleased(int x, int y, int button) {
    if (sequenceUI.isActive()) {
        sequenceUI.mouseReleased(x, y, button);
        return;
    }

    if (bEditMode) {
        bIsDraggingGroup = false;
        bIsSelecting = false;
        resizingFrame = nullptr;
        resizingRect = nullptr;
    }
    bIsDraggingPan = false;
        
        controlsUI.mouseReleased();
}

void PlaylistVisualizerApp::mouseMoved(int x, int y) {
    searchBar.mouseMoved(x, y, pan, zoom);
    historyUI.mouseMoved(x, y, pan, zoom);
    viewsUI.mouseMoved(x, y, pan, zoom);
    winPosUI.mouseMoved(x, y);
}

void PlaylistVisualizerApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (sequenceUI.isActive()) {
        sequenceUI.mouseScrolled(x, y, scrollY);
        return;
    }

    if (searchBar.isVisible()) return;
    if (historyUI.mouseScrolled(x, y, scrollY)) return;
    
    if (scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse(x, y);
    
    if (geminiUI.mouseScrolled(worldM, scrollY)) return;

    if (textureUI.mouseScrolled(worldM, scrollY)) return;

    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    zoom = ofClamp(zoom * zoomFactor, 0.01f, 50.0f);
    pan.x = x - worldM.x * zoom;
    pan.y = y - worldM.y * zoom;
}

void PlaylistVisualizerApp::keyPressed(int key) { 
    if (sequenceUI.isActive()) {
        sequenceUI.keyPressed(key, this);
        return;
    }

    if (searchBar.keyPressed(key, pan, zoom)) return;
    if (historyUI.keyPressed(key, pan, zoom)) return;
    if (viewsUI.keyPressed(key, pan, zoom)) return;
    if (winPosUI.keyPressed(key)) return;
    
    if (editingNote) {
        if (ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL) || ofGetKeyPressed(OF_KEY_SUPER)) {
            if (key == 'v' || key == 'V' || key == 22) { 
                editingNote->text += ofGetWindowPtr()->getClipboardString();
                editingNote->updateSize();
                return;
            }
        }
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if (!editingNote->text.empty()) {
                editingNote->text.pop_back();
                editingNote->updateSize();
            }
        } else if (key == OF_KEY_RETURN) {
            editingNote->text += '\n';
            editingNote->updateSize();
        } else if (key == OF_KEY_ESC) {
            editingNote->bIsEditing = false;
            editingNote = nullptr;
        } else if (key >= 32 && key <= 126) {
            editingNote->text += (char)key;
            editingNote->updateSize();
        }
        return;
    }
    
    if (controlsUI.bEditingGolmSeed) {
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if (!controlsUI.golmSeedString.empty()) {
                controlsUI.golmSeedString.pop_back();
            }
        } else if (key == OF_KEY_RETURN) {
            controlsUI.bEditingGolmSeed = false;
            if (roomApp) {
                try { roomApp->golBoxMotion.currentSeed = std::stol(controlsUI.golmSeedString); } catch(...) {}
                roomApp->golBoxMotion.reset();
            }
        } else if (key == OF_KEY_ESC) {
            controlsUI.bEditingGolmSeed = false;
        } else if (key >= '0' && key <= '9') {
            controlsUI.golmSeedString += (char)key;
        }
        return;
    }
    
    if (controlsUI.bEditingGolSeed) {
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if (!controlsUI.golSeedString.empty()) {
                controlsUI.golSeedString.pop_back();
            }
        } else if (key == OF_KEY_RETURN) {
            controlsUI.bEditingGolSeed = false;
            if (roomApp) {
                try { roomApp->golBox.currentSeed = std::stol(controlsUI.golSeedString); } catch(...) {}
                roomApp->golBox.reset();
            }
        } else if (key == OF_KEY_ESC) {
            controlsUI.bEditingGolSeed = false;
        } else if (key >= '0' && key <= '9') {
            controlsUI.golSeedString += (char)key;
        }
        return;
    }
    
    if (ofGetKeyPressed(OF_KEY_SHIFT) && (key == 'f' || key == 'F')) {
        searchBar.toggle(getAllSearchableButtons(), pan, zoom);
        return;
    }

    if (geminiUI.keyPressed(key)) return;

    if (key >= '1' && key <= '5') {
        int idx = key - '1';
        viewsUI.applyPresetByIndex(idx, pan, zoom);
        return;
    }

    if (ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL) || ofGetKeyPressed(OF_KEY_SUPER)) {
        if (bEditMode && (key == 'z' || key == 'Z' || key == 26)) {
            undo();
            return;
        }
        if (key == 's' || key == 'S' || key == 19) { // 19 est le code ASCII pour Ctrl+S sur certains systèmes
            saveButtonPositions();
            return;
        }
    }

    if (textureUI.keyPressed(key, roomApp)) return;

    if (bEditMode && !selectedRects.empty()) {
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            saveUndoState("Suppression Element");
            textNotes.erase(std::remove_if(textNotes.begin(), textNotes.end(),
                [&](const shared_ptr<PlaylistTextNote>& note) {
                    return std::find(selectedRects.begin(), selectedRects.end(), &note->rect) != selectedRects.end();
                }), textNotes.end());
            
            visualFrames.erase(std::remove_if(visualFrames.begin(), visualFrames.end(),
                [&](const shared_ptr<PlaylistVisualFrame>& frame) {
                    return std::find(selectedRects.begin(), selectedRects.end(), &frame->rect) != selectedRects.end();
                }), visualFrames.end());
            
            selectedRects.clear();
            return;
        }
        
        if (ofGetKeyPressed(OF_KEY_SHIFT)) {
            if (key == 'x' || key == 'X') {
                 saveUndoState("Alignement Vertical (Y)");
                 float targetY = selectedRects[0]->y;
                for (auto* r : selectedRects) {
                    r->y = targetY;
                }   
                return;
            }
            if (key == 'y' || key == 'Y') {
                 saveUndoState("Alignement Horizontal (X)");
                 float targetX = selectedRects[0]->x;
                for (auto* r : selectedRects) {
                    r->x = targetX;
                }
                return;
            }
        }
    }

    if(key == ' ') isSpacePressed = true; 
    if(key == 'r' || key == 'R') {
        zoom = 1.0f;
        pan.set(250, 0);
    }
    if(key == 'l' || key == 'L') {
        loadButtonPositions();
    }
    
    if(key == 'g' || key == 'G') {
        if(mainAppPtr) {
            mainAppPtr->gabMode = 3;
            if(mainAppPtr->roomApp) mainAppPtr->roomApp->wallAlpha = 0.0f;
            if(mainAppPtr->scene2D) mainAppPtr->scene2D->bgDisplayMode = 3;
        }
    }
    
    // Focus sur les fenetres avec les touches correspondantes
    auto focusWindow = [&](int index) {
        auto win = getAppWindow(index);
        if(win) {
            auto glfwWin = dynamic_pointer_cast<ofAppGLFWWindow>(win);
            if(glfwWin) {
                glfwShowWindow(glfwWin->getGLFWWindow());
                glfwRestoreWindow(glfwWin->getGLFWWindow());
                glfwFocusWindow(glfwWin->getGLFWWindow());
            }
        }
    };

    if(key == 'w' || key == 'W') focusWindow(1); // Room
    if(key == 'x' || key == 'X') focusWindow(2); // Zenit
    if(key == 'c' || key == 'C') focusWindow(3); // Scene 2D
    if(key == 'v' || key == 'V') focusWindow(4); // Preview
    if(key == 'b' || key == 'B') focusWindow(5); // Boutons
}

void PlaylistVisualizerApp::keyReleased(int key) { if(key == ' ') isSpacePressed = false; }

ofVec2f PlaylistVisualizerApp::getTransformedMouse(int x, int y) {
    return ofVec2f((x - pan.x) / zoom, (y - pan.y) / zoom);
}

ofRectangle* PlaylistVisualizerApp::findButtonAt(ofVec2f pos) {
    if (auto* r = windowControlsUI.findButtonAt(pos)) return r;
    if (auto* r = playerUI.findButtonAt(pos)) return r;
    if (auto* r = geminiUI.findButtonAt(pos)) return r;
    if (auto* r = controlsUI.findButtonAt(pos)) return r;
    if (auto* r = textureUI.findButtonAt(pos)) return r;
    if (auto* r = imageGraphPlayer.findButtonAt(pos)) return r;
    for(auto& note : textNotes) {
        if(note->rect.inside(pos)) return &note->rect;
    }
    for(auto& frame : visualFrames) {
        if(frame->isEdgeHit(pos)) return &frame->rect;
    }

    if(diagramDropZone.inside(pos)) return &diagramDropZone;
    
    if(imageGraphDropZone.inside(pos)) return &imageGraphDropZone;
    return nullptr;
}

ofJson PlaylistVisualizerApp::serializeState() {
    ofJson pt;
    
    // Sauvegarde du Zoom et du Pan
    pt["view"]["zoom"] = zoom;
    pt["view"]["pan_x"] = pan.x;
    pt["view"]["pan_y"] = pan.y;

    windowControlsUI.saveSettings(pt);
    controlsUI.saveSettings(pt);
    playerUI.saveSettings(pt);
    textureUI.saveSettings(pt);
    
    pt["diagram"]["x"] = diagramDropZone.x;
    pt["diagram"]["y"] = diagramDropZone.y;
    pt["diagram"]["w"] = diagramDropZone.width;
    pt["diagram"]["h"] = diagramDropZone.height;
    
    pt["imageGraphZone"]["x"] = imageGraphDropZone.x;
    pt["imageGraphZone"]["y"] = imageGraphDropZone.y;
    pt["imageGraphZone"]["w"] = imageGraphDropZone.width;
    pt["imageGraphZone"]["h"] = imageGraphDropZone.height;
    
    pt["notes"] = ofJson::array();
    for(auto& note : textNotes) {
        ofJson n;
        n["text"] = note->text;
        n["x"] = note->rect.x;
        n["y"] = note->rect.y;
        pt["notes"].push_back(n);
    }
    
    pt["frames"] = ofJson::array();
    for(auto& frame : visualFrames) {
        ofJson f;
        f["x"] = frame->rect.x;
        f["y"] = frame->rect.y;
        f["w"] = frame->rect.width;
        f["h"] = frame->rect.height;
        pt["frames"].push_back(f);
    }
    
    viewsUI.saveSettings(pt);
    
    winPosUI.saveSettings(pt);
    
    pt["current_windows"] = ofJson::array();
    for(int w=0; w<7; w++) {
        ofJson wJson;
        auto win = getAppWindow(w);
        if(win) {
            wJson["x"] = win->getWindowPosition().x;
            wJson["y"] = win->getWindowPosition().y;
            wJson["w"] = win->getWindowSize().x;
            wJson["h"] = win->getWindowSize().y;
        } else {
            wJson["x"] = 0; wJson["y"] = 0; wJson["w"] = 0; wJson["h"] = 0;
        }
        pt["current_windows"].push_back(wJson);
    }
    
    if (player) {
        pt["player"]["folderPath"] = player->getFolderPath();
    }
    
    geminiUI.saveSettings(pt);
    imageGraphPlayer.saveSettings(pt);
    pt["annexes"]["hidden"] = bAnnexesHidden;

    return pt;
}

void PlaylistVisualizerApp::deserializeState(const ofJson& pt) {
    if(pt.contains("view")) {
        zoom = pt["view"].value("zoom", zoom);
        pan.x = pt["view"].value("pan_x", pan.x);
        pan.y = pt["view"].value("pan_y", pan.y);
    }

    windowControlsUI.loadSettings(pt);
    controlsUI.loadSettings(pt);
    playerUI.loadSettings(pt);
    textureUI.loadSettings(pt);
    
    if(pt.contains("diagram")) {
        diagramDropZone.x = pt["diagram"].value("x", diagramDropZone.x);
        diagramDropZone.y = pt["diagram"].value("y", diagramDropZone.y);
        diagramDropZone.width = pt["diagram"].value("w", diagramDropZone.width);
        diagramDropZone.height = pt["diagram"].value("h", diagramDropZone.height);
    }
    
    if(pt.contains("imageGraphZone")) {
        imageGraphDropZone.x = pt["imageGraphZone"].value("x", imageGraphDropZone.x);
        imageGraphDropZone.y = pt["imageGraphZone"].value("y", imageGraphDropZone.y);
        imageGraphDropZone.width = pt["imageGraphZone"].value("w", imageGraphDropZone.width);
        imageGraphDropZone.height = pt["imageGraphZone"].value("h", imageGraphDropZone.height);
    }
    
    textNotes.clear();
    editingNote = nullptr;
    if(pt.contains("notes")) {
        for(auto& n : pt["notes"]) {
            auto note = make_shared<PlaylistTextNote>(ofVec2f(n.value("x", 0.0f), n.value("y", 0.0f)));
            note->text = n.value("text", "");
            note->updateSize();
            textNotes.push_back(note);
        }
    }
    
    visualFrames.clear();
    resizingFrame = nullptr;
    if(pt.contains("frames")) {
        for(auto& f : pt["frames"]) {
            auto frame = make_shared<PlaylistVisualFrame>(ofVec2f(0, 0));
            frame->rect.x = f.value("x", 0.0f);
            frame->rect.y = f.value("y", 0.0f);
            frame->rect.width = f.value("w", 300.0f);
            frame->rect.height = f.value("h", 200.0f);
            visualFrames.push_back(frame);
        }
    }
    
    viewsUI.loadSettings(pt);
    
    winPosUI.loadSettings(pt);
    
    if(pt.contains("current_windows") && pt["current_windows"].is_array()) {
        for(int w=0; w<7 && w<pt["current_windows"].size(); w++) {
            auto win = getAppWindow(w);
            if(win) {
                float x = pt["current_windows"][w].value("x", win->getWindowPosition().x);
                float y = pt["current_windows"][w].value("y", win->getWindowPosition().y);
                float width = pt["current_windows"][w].value("w", win->getWindowSize().x);
                float height = pt["current_windows"][w].value("h", win->getWindowSize().y);
                if (width > 50 && height > 50) { 
                    win->setWindowPosition(x, y);
                    win->setWindowShape(width, height);
                }
            }
        }
    }
    
    if(pt.contains("player")) {
        pendingVideoFolder = pt["player"].value("folderPath", "");
    }
    
    geminiUI.loadSettings(pt);
    imageGraphPlayer.loadSettings(pt, imageGraphDropZone);

    if(pt.contains("annexes")) {
        bAnnexesHidden = pt["annexes"].value("hidden", false);
        if (mainAppPtr && mainAppPtr->annexeApp) {
            if (mainAppPtr->annexeApp) mainAppPtr->annexeApp->setEnabled(!bAnnexesHidden);
            if (mainAppPtr->annexePlayerApp) mainAppPtr->annexePlayerApp->setEnabled(!bAnnexesHidden);
        }
    }
}

void PlaylistVisualizerApp::saveUndoState(string actionName) {
    ofJson currentState = serializeState();
    if (!undoStack.empty() && undoStack.back() == currentState) {
        return; 
    }
    undoStack.push_back(currentState);
    if(undoStack.size() > 5) {
        undoStack.pop_front();
    }
    
    historyUI.addEvent(actionName, pan, zoom);
}

void PlaylistVisualizerApp::undo() {
    if(undoStack.empty()) return;
    
    ofJson pt = undoStack.back();
    undoStack.pop_back();
    
    selectedRects.clear();
    resizingFrame = nullptr;
    resizingRect = nullptr;
    bIsDraggingGroup = false;
    bIsSelecting = false;
    editingNote = nullptr;
    
    deserializeState(pt);
}

void PlaylistVisualizerApp::saveButtonPositions() {
    ofJson pt = serializeState();
    
    // Sauvegarde de la fenêtre (Position et Taille)
    pt["window"]["x"] = ofGetWindowPositionX();
    pt["window"]["y"] = ofGetWindowPositionY();
    pt["window"]["w"] = ofGetWindowWidth();
    pt["window"]["h"] = ofGetWindowHeight();

    ofSavePrettyJson("playlist_btn_positions.json", pt);
    ofLogNotice("PlaylistVisualizerApp") << "Positions des boutons sauvegardees.";
    
    saveFeedbackTimer = ofGetElapsedTimef();
}

void PlaylistVisualizerApp::loadButtonPositions() {
    ofFile file("playlist_btn_positions.json");
    if(!file.exists()) return;

    ofJson pt = ofLoadJson(file.path());
    
    // Chargement de la fenêtre (Position et Taille)
    if(pt.contains("window")) {
        int wx = pt["window"].value("x", ofGetWindowPositionX());
        int wy = pt["window"].value("y", ofGetWindowPositionY());
        int ww = pt["window"].value("w", ofGetWindowWidth());
        int wh = pt["window"].value("h", ofGetWindowHeight());
        
        if (ww > 50 && wh > 50) { // Sécurité pour éviter d'avoir une fenêtre buggée trop petite
            ofSetWindowPosition(wx, wy);
            ofSetWindowShape(ww, wh);
        }
    }

    deserializeState(pt);
}

vector<ofRectangle*> PlaylistVisualizerApp::getAllInteractableRects() {
    vector<ofRectangle*> rects;
    auto winRects = windowControlsUI.getInteractableRects();
    rects.insert(rects.end(), winRects.begin(), winRects.end());
    
    auto playerRects = playerUI.getInteractableRects();
    rects.insert(rects.end(), playerRects.begin(), playerRects.end());
    
    auto geminiRects = geminiUI.getInteractableRects();
    rects.insert(rects.end(), geminiRects.begin(), geminiRects.end());
    
    auto controlsRects = controlsUI.getInteractableRects();
    rects.insert(rects.end(), controlsRects.begin(), controlsRects.end());
    
    auto texRects = textureUI.getInteractableRects();
    rects.insert(rects.end(), texRects.begin(), texRects.end());
    
    auto imgRects = imageGraphPlayer.getInteractableRects();
    rects.insert(rects.end(), imgRects.begin(), imgRects.end());
    for(auto& note : textNotes) rects.push_back(&note->rect);
    for(auto& frame : visualFrames) rects.push_back(&frame->rect);

    rects.push_back(&diagramDropZone);
    rects.push_back(&imageGraphDropZone);
    return rects;
}

vector<SearchableButton> PlaylistVisualizerApp::getAllSearchableButtons() {
    vector<SearchableButton> res;
    
    // Window Controls
    for(int i=0; i<4; i++) res.push_back({"V" + ofToString(i+1), &windowControlsUI.viewBtns[i]});
    for(int i=0; i<4; i++) res.push_back({"->V" + ofToString(i+1), &windowControlsUI.moveWinBtns[i]});
    for(int i=0; i<4; i++) res.push_back({"V" + ofToString(i+1) + " WIN", &windowControlsUI.toggleWinBtns[i]});
    for(int i=0; i<4; i++) res.push_back({"REC V" + ofToString(i+1), &windowControlsUI.recWinBtns[i]});
    res.push_back({"FMT", &windowControlsUI.formatBtn});
    res.push_back({"Q", &windowControlsUI.qualityBtn});
    res.push_back({"ARRANGE", &windowControlsUI.arrangeWinBtn});
    for(int i=0; i<6; i++) {
        res.push_back({windowControlsUI.wxcvbNames[i], &windowControlsUI.wxcvbBtns[i]});
        res.push_back({windowControlsUI.focusNames[i], &windowControlsUI.focusBtns[i]});
    }
    for(int i=0; i<5; i++) res.push_back({"GAB " + ofToString(i), &windowControlsUI.gabBtns[i]});
    
    if (mainAppPtr) {
        if (mainAppPtr->annexeApp) {
            res.push_back({"FOCUS ANNEXE", &focusAnnexesBtnRect});
            res.push_back({"ANNEXE ON/OFF", &toggleAnnexesBtnRect});
        } else {
            res.push_back({"CREER ANNEXE", &createAnnexeBtnRect});
        }
    }
    res.push_back({"Room Alpha", &windowControlsUI.roomAlphaBtn});
    res.push_back({"DIF:ROOM", &windowControlsUI.diffuseRoomBtn});
    res.push_back({"DIF:SCENE2D", &windowControlsUI.diffuseScene2DBtn});
    res.push_back({"LAY:ZENIT", &windowControlsUI.zenitLayoutBtn});
    res.push_back({"CREER ZENIT", &windowControlsUI.createZenitBtn});
    
    // Controls UI
    for(auto& t : controlsUI.roomToggles) res.push_back({t.name, &t.rect});
    for(auto& b : controlsUI.roomActionBtns) res.push_back({b.name, &b.rect});
    for(auto& b : controlsUI.globalActionBtns) res.push_back({b.name, &b.rect});
    for(auto& t : controlsUI.layerToggles) res.push_back({t.name, &t.rect});
    for(auto& b : controlsUI.creatureButtons) res.push_back({b.name, &b.rect});
    for(auto& b : controlsUI.interactiveButtons) res.push_back({b.name, &b.rect});
    for(auto& b : controlsUI.mainBrushButtons) res.push_back({b.name, &b.rect});
    res.push_back({"CLEAR ALL CREATURES", &controlsUI.clearAllCreaturesBtn});
    res.push_back({"UNDO CREA", &controlsUI.undoCreatureBtn});
    res.push_back({"RESET EATMAP", &controlsUI.resetEatMapBtn});
    res.push_back({"RESET COLLIDER", &controlsUI.resetCollidersBtn});
    res.push_back({"GOLM SEED BOX", &controlsUI.golmSeedBox});
    res.push_back({"GOL SEED BOX", &controlsUI.golSeedBox});
    
    // Player UI
    res.push_back({"LOOP MODE", &playerUI.loopButtonRect});
    res.push_back({"GO PLAYLIST", &playerUI.toggleButtonRect});
    res.push_back({"SIMULATE 32 VID", &playerUI.simButtonRect});
    res.push_back({"SPEED x2", &playerUI.doubleSpeedBtnRect});
    res.push_back({"MUTE", &playerUI.muteBtnRect});
    res.push_back({"CROP 106%", &playerUI.crop106BtnRect});
    res.push_back({"USE DISK IMGS", &playerUI.useDiskImagesBtnRect});
    res.push_back({"HOLD LAST FRAME", &playerUI.infinitePauseBtnRect});
    res.push_back({"PAUSE ACCORDION", &playerUI.pauseAccordionBtn});
    res.push_back({"FADE ACCORDION", &playerUI.fadeAccordionBtn});
    
    // Gemini UI
    res.push_back({"API KEY", &geminiUI.apiKeyBox});
    res.push_back({"SCAN MODELS", &geminiUI.listModelsBtn});
    res.push_back({"THEME", &geminiUI.themeBox});
    res.push_back({"PROMPT V1", &geminiUI.promptVid1Box});
    res.push_back({"PROMPT V2", &geminiUI.promptVid2Box});
    res.push_back({"GEN 360 FROM ROOM", &geminiUI.genRoomBtn});
    res.push_back({"GEN VID FROM LAST", &geminiUI.genVidLastBtn});
    res.push_back({"GEN VID FROM 2 LAST", &geminiUI.genVid2LastBtn});
    res.push_back({"MODEL", &geminiUI.modelAccordionBtn});
    res.push_back({"IMG SIZE", &geminiUI.imageSizeBtn});
    res.push_back({"VID RES", &geminiUI.videoResBtn});
    res.push_back({"GEN 360 FROM TEXT", &geminiUI.genTextToRoomBtn});
    
    for(auto& note : textNotes) {
        string preview = note->text;
        ofStringReplace(preview, "\n", " ");
        if (preview.length() > 30) preview = preview.substr(0, 27) + "...";
        if (preview.empty()) preview = "Note vide";
        res.push_back({"Note : " + preview, &note->rect});
    }
    
    return res;
}