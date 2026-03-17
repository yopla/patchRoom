#include "PlaylistVisualizerApp.h"
#include <algorithm>
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "ofApp.h"
#include "ViewApp.h"
#include "ofAppGLFWWindow.h"
#include "GLFW/glfw3.h"
#include "RoomPreview.h"

void PlaylistVisualizerApp::setup() {
    ofSetBackgroundColor(25);
    loopButtonRect.set(10, 80, 180, 30);
    toggleButtonRect.set(10, 120, 180, 30);
    simButtonRect.set(10, 160, 180, 30);
    pauseAccordionBtn.set(10, 200, 180, 30);
    
    for(int i=0; i<4; i++) {
        viewBtns[i].set(10 + i * 55, 40, 50, 30);
        viewHidden[i] = false;
    }
    
    moveV3Btn.set(10 + 2 * 55, -30, 50, 30); // Placé au-dessus du bouton V3 (monté d'un cran)
    
    for(int i=0; i<6; i++) {
        wxcvbBtns[i].set(230 + i * 59, 5, 56, 30);
        focusBtns[i].set(230 + i * 59, -65, 56, 30);
    }
    
    // Boutons GAB places de maniere plus naturelle
    gabBtns[0].set(-65, -30, 56, 30);  // GAB Master (M) -> complètement à gauche
    gabBtns[1].set(10, -30, 56, 30);   // GAB Room (R) -> au-dessus de la colonne des toggles 3D
    gabBtns[2].set(407, -30, 56, 30);  // GAB Scene2D (S) -> en dessous de ->C et au-dessus de C:SCEN

    // Ajustements spécifiques de positionnement
    focusBtns[0].set(-65, -65, 56, 30); // ->MAIN placé au-dessus de son bouton GAB M
    focusBtns[1].set(10, -65, 56, 30);  // ->W placé au-dessus de son bouton GAB R
    wxcvbBtns[0].set(-65, 5, 56, 30);   // MAIN placé en-dessous de GAB M
    wxcvbBtns[1].set(10, 5, 56, 30);    // W:ROOM placé en-dessous de GAB R

    zoom = 0.8f;
    pan.set(0, 130); // Décale tout vers le bas pour ne pas cacher le bouton V3 sous le HUD fixe
    isSpacePressed = false;
    
    tooltipManager.setup();
}

void PlaylistVisualizerApp::setupLayerToggles() {
    if(!scene2D) return;
    layerToggles.clear();

    auto addToggle = [&](string name, bool& val) {
        layerToggles.push_back({name, &val, ofRectangle()});
    };

    addToggle("Creatures", scene2D->layerManager.bDrawCreatures);
    addToggle("Poulpe", scene2D->layerManager.bDrawPoulpe);
    addToggle("Fish", scene2D->layerManager.bDrawFish);
    addToggle("Sauteurs", scene2D->layerManager.bDrawSauteurs);
    addToggle("Slime", scene2D->layerManager.bDrawSlime);
    addToggle("Walker", scene2D->layerManager.bDrawWalker);
    addToggle("Lightning", scene2D->layerManager.bDrawLightning);
    addToggle("Plants", scene2D->layerManager.bDrawPlants);
    addToggle("Flytraps", scene2D->layerManager.bDrawFlytraps);
    addToggle("FluidFloor", scene2D->layerManager.bDrawFluidFloor);
    addToggle("Gears", scene2D->layerManager.bDrawGears);
    addToggle("Machine", scene2D->layerManager.bDrawMachine);
    addToggle("Digging", scene2D->layerManager.bDrawDigging);
    addToggle("AutoMach", scene2D->layerManager.bDrawMachineAuto);
    addToggle("Curtain", scene2D->layerManager.bDrawCurtain);
    addToggle("Colliders", scene2D->layerManager.bDrawColliders);
    addToggle("Puyo", scene2D->layerManager.bDrawPuyo);
    addToggle("Bubbles", scene2D->layerManager.bDrawBubbles);
    addToggle("Kani", scene2D->layerManager.bDrawKani);
    addToggle("Slime2", scene2D->layerManager.bDrawSlime2);
    addToggle("Teaa", scene2D->layerManager.bDrawTeaa);
    addToggle("Ballet", scene2D->layerManager.bDrawBallet);
    addToggle("Kundelich", scene2D->layerManager.bDrawKundelich);
    addToggle("KineShad", scene2D->layerManager.bDrawKineShad);
    addToggle("MultiPend", scene2D->layerManager.bDrawMultiPendulum);
    addToggle("Pancarte", scene2D->layerManager.bDrawPancarte);
    addToggle("Pendulum", scene2D->layerManager.bDrawPendulum);
    addToggle("Pince", scene2D->layerManager.bDrawPince);
    addToggle("PinceBra", scene2D->layerManager.bDrawPinceBra);
    addToggle("PinceFoire", scene2D->layerManager.bDrawPinceFoire);
    addToggle("Whaa", scene2D->layerManager.bDrawWhaa);
    addToggle("ChainCrea", scene2D->layerManager.bDrawChainCrea);
    addToggle("WallWalk", scene2D->layerManager.bDrawWallWalker);
    addToggle("Swing", scene2D->layerManager.bDrawSwing);
    addToggle("Fluid", scene2D->layerManager.bDrawFluid);
    addToggle("FluidDeux", scene2D->layerManager.bDrawFluidDeux);
    addToggle("FluidTrois", scene2D->layerManager.bDrawFluidTrois);
    addToggle("FireA", scene2D->layerManager.bDrawFireA);
    addToggle("FireB", scene2D->layerManager.bDrawFireB);
    addToggle("FireC", scene2D->layerManager.bDrawFireC);
    addToggle("Mongolfier", scene2D->layerManager.bDrawMongolfier);
    addToggle("GroPuyo", scene2D->layerManager.bDrawGroPuyo);
    addToggle("WalkSquare", scene2D->layerManager.bDrawWalkingSquare);
    addToggle("TripodEye", scene2D->layerManager.bDrawTripodEye);

    int cols = 3;
    float bw = 110;
    float bh = 20;
    float pad = 5;
    float startX = 230;
    float startY = 80;

    for(int i=0; i<layerToggles.size(); i++) {
        int c = i % cols;
        int r = i / cols;
        layerToggles[i].rect.set(startX + c*(bw+pad), startY + r*(bh+pad), bw, bh);
    }
    
    // Configuration des boutons de créatures
    float cStartX = 590; // Décalé à droite
    vector<string> cNames = {
        "Ripple", "Wanco", "Breakable", "Geko", 
        "Cousin", "CousinCon", "DblPendulum", "Halo", 
        "Fluids", "Spring", "Dancing", "Creature", 
        "Otarie", "Sauteur"
    };
    
    creatureButtons.clear();
    int cCols = 3;
    for(int i=0; i<cNames.size(); i++) {
        int c = i % cCols;
        int r = i / cCols;
        creatureButtons.push_back({cNames[i], ofRectangle(cStartX + c*(bw+pad), startY + r*(bh+pad), bw, bh)});
    }
    
    // Bouton de suppression générale
    int cRows = (cNames.size() + cCols - 1) / cCols;
    clearAllCreaturesBtn.set(cStartX, startY + cRows*(bh+pad) + 10, bw*2+pad, bh);

    // Configuration des boutons interactifs (Touche 1)
    float iStartY = clearAllCreaturesBtn.y + bh + 30; // En dessous de CLEAR ALL
    vector<string> iNames = {"GroPuyo", "Pendulum", "Puyo", "Bubble", "Poulpe", "Sardine"};
    
    interactiveButtons.clear();
    for(int i=0; i<iNames.size(); i++) {
        int c = i % cCols;
        int r = i / cCols;
        interactiveButtons.push_back({iNames[i], ofRectangle(cStartX + c*(bw+pad), iStartY + r*(bh+pad), bw, bh)});
    }
    
    if(scene2D && selectedInteractiveIndex >= 0 && selectedInteractiveIndex < interactiveButtons.size()) {
        scene2D->layerManager.selectedInteractiveLayer = interactiveButtons[selectedInteractiveIndex].name;
    }

    // Décalage des boutons de simulation vers le bas
    int rows = (layerToggles.size() + cols - 1) / cols;
    float topOffset = startY + rows * (bh + pad) + 20;
    
    loopButtonRect.y = topOffset;
    toggleButtonRect.y = topOffset + 40;
    simButtonRect.y = topOffset + 80;
    pauseAccordionBtn.set(10, topOffset + 120, 180, 30);
    
    pauseOptionRects.clear();
    for(int i=0; i<pauseOptions.size(); i++) {
        pauseOptionRects.push_back(ofRectangle(10, pauseAccordionBtn.y + 30 + i * 30, 180, 30));
    }
    
    // Synchronisation initiale
    if(scene2D && !creatureButtons.empty()) {
        scene2D->layerManager.selectedCreatureToSpawn = creatureButtons[selectedCreatureIndex].name;
    }
}

void PlaylistVisualizerApp::setupRoomToggles() {
    if(!roomApp) return;
    roomToggles.clear();

    auto addToggle = [&](string name, std::function<bool()> get, std::function<void()> toggle) {
        roomToggles.push_back({name, ofRectangle(), get, toggle});
    };

    addToggle("Sol 3D", [this](){ return roomApp->bDrawUndulatingFloor; }, [this](){ roomApp->bDrawUndulatingFloor = !roomApp->bDrawUndulatingFloor; });
    addToggle("Oscillate", [this](){ return roomApp->bOscillateRoom; }, [this](){ roomApp->bOscillateRoom = !roomApp->bOscillateRoom; });
    addToggle("Kraken", [this](){ return roomApp->bDrawKraken; }, [this](){ 
        roomApp->bDrawKraken = !roomApp->bDrawKraken; 
        if(roomApp->bDrawKraken) roomApp->kraken.start(roomApp->localTime);
    });
    addToggle("Ext Kraken", [this](){ return roomApp->bDrawExternalKraken; }, [this](){ 
        roomApp->bDrawExternalKraken = !roomApp->bDrawExternalKraken; 
        if(roomApp->bDrawExternalKraken) roomApp->externalKraken.start(roomApp->localTime);
    });
    addToggle("Cloud Ring", [this](){ return roomApp->bDrawCloudRing; }, [this](){ roomApp->bDrawCloudRing = !roomApp->bDrawCloudRing; });
    addToggle("Liq Sphere", [this](){ return roomApp->bDrawLiquidSphere; }, [this](){ roomApp->bDrawLiquidSphere = !roomApp->bDrawLiquidSphere; });
    addToggle("Scene360Vid", [this](){ return roomApp->bDrawScene360Video; }, [this](){ roomApp->bDrawScene360Video = !roomApp->bDrawScene360Video; roomApp->scene360VideoPlayer.toggle(); });
    addToggle("Beam Proj", [this](){ return roomApp->bDrawBeam; }, [this](){ roomApp->bDrawBeam = !roomApp->bDrawBeam; });
    addToggle("Atmosphere", [this](){ return roomApp->bDrawAtmosphere; }, [this](){ roomApp->bDrawAtmosphere = !roomApp->bDrawAtmosphere; });
    addToggle("Use Texture", [this](){ return roomApp->bUseTexture; }, [this](){ roomApp->bUseTexture = !roomApp->bUseTexture; });
    addToggle("Show Roof", [this](){ return roomApp->bShowRoof; }, [this](){ roomApp->bShowRoof = !roomApp->bShowRoof; });
    addToggle("Respire", [this](){ return roomApp->respire; }, [this](){ roomApp->respire = !roomApp->respire; });
    addToggle("Ripples", [this](){ return roomApp->bDrawRipples; }, [this](){ roomApp->bDrawRipples = !roomApp->bDrawRipples; });
    addToggle("Worms", [this](){ return roomApp->bDrawWorms; }, [this](){ roomApp->bDrawWorms = !roomApp->bDrawWorms; });
    addToggle("Wing Worms", [this](){ return roomApp->bDrawWingedWorms; }, [this](){ roomApp->bDrawWingedWorms = !roomApp->bDrawWingedWorms; });
    addToggle("Fluid Ring", [this](){ return roomApp->bFluidRingEnabled; }, [this](){ 
        roomApp->bFluidRingEnabled = !roomApp->bFluidRingEnabled; 
        roomApp->fluidRing.setTargetAlpha(roomApp->bFluidRingEnabled ? 1.0f : 0.0f);
    });
    addToggle("Light Fly", [this](){ return roomApp->bLightFlyRingEnabled; }, [this](){ roomApp->bLightFlyRingEnabled = !roomApp->bLightFlyRingEnabled; });
    addToggle("Alpha Cur", [this](){ return roomApp->cursorSquare.bLowAlpha; }, [this](){ roomApp->cursorSquare.bLowAlpha = !roomApp->cursorSquare.bLowAlpha; });
    addToggle("Alpha Wall", [this](){ return roomApp->wallAlpha > 50.0f; }, [this](){ roomApp->wallAlpha = (roomApp->wallAlpha > 50.0f) ? 0.0f : 100.0f; });
    addToggle("Gen 360", [this](){ return false; }, [this](){ roomApp->generateEquirectangularImage(); });

    int cols = 2;
    float bw = 100;
    float bh = 20;
    float pad = 5;
    float startX = 10;
    float startY = 80;

    for(int i=0; i<roomToggles.size(); i++) {
        int c = i % cols;
        int r = i / cols;
        roomToggles[i].rect.set(startX + c*(bw+pad), startY + r*(bh+pad), bw, bh);
    }
}

void PlaylistVisualizerApp::clearAllCreatures() {
    if(!scene2D) return;
    auto& mgr = scene2D->layerManager;
    mgr.creatureSystem.clear();
    mgr.cousinCons.clear();
    mgr.halos.clear();
}

void PlaylistVisualizerApp::update() {
    if(!bEnabled) return;

    if(scene2D && layerToggles.empty()) {
        setupLayerToggles();
    }

    if(roomApp && roomToggles.empty()) {
        setupRoomToggles();
    }

    if(!player) return;

    auto& videos = player->getVideos();
    std::set<string> uniqueFrames;
    std::set<string> startFrames;
    for(auto& v : videos) {
        uniqueFrames.insert(v.startFrame);
        uniqueFrames.insert(v.endFrame);
        startFrames.insert(v.startFrame);
    }

    // Détection des culs-de-sac (Noeuds de fin sans aucune vidéo qui en repart)
    deadEnds.clear();
    for(auto& f : uniqueFrames) {
        if(startFrames.find(f) == startFrames.end()) {
            deadEnds.push_back(f);
        }
    }

    bool changed = (uniqueFrames.size() != nodes.size());
    if(!changed) {
        for(auto& f : uniqueFrames) {
            if(nodes.find(f) == nodes.end()) {
                changed = true;
                break;
            }
        }
    }

    if(changed) {
        nodes.clear();
        for(auto& f : uniqueFrames) {
            nodes[f] = VisualNode{f, ofVec2f()};
        }
    }
}

void PlaylistVisualizerApp::dragEvent(ofDragInfo dragInfo) {
    if(!bEnabled || !player) return;
    
    if(dragInfo.files.size() > 0) {
        string path = dragInfo.files[0];
        ofFile file(path);
        if(file.isDirectory()) {
            if(player->isSimulating32Videos()) {
                player->toggleSimulate32Videos();
            }
            player->startPlaylist(path);
            if(bDrawScene360VideoPtr) *bDrawScene360VideoPtr = true; // Lance l'affichage dans la room
            ofLogNotice("PlaylistVisualizerApp") << "Nouvelle playlist chargee via drag&drop : " << path;
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

    // Boutons Fermer ViewApps
    for(int i=0; i<4; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
        if(!isLoaded) {
            ofSetColor(80, 80, 80); // Gris si non chargé depuis main.cpp
        } else if(viewHidden[i]) {
            ofSetColor(50, 200, 50); // Vert si caché
        } else {
            ofSetColor(200, 50, 50); // Rouge si affiché
        }
        ofFill();
        ofDrawRectangle(viewBtns[i]);
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(viewBtns[i]);
        ofDrawBitmapString("V" + ofToString(i+1), viewBtns[i].x + 15, viewBtns[i].y + 20);
        ofPopStyle();
    }

    // Bouton pour déplacer V3 à droite
    ofPushStyle();
    bool v3Loaded = (mainAppPtr && mainAppPtr->viewApps.size() > 2 && mainAppPtr->viewApps[2]);
    if(v3Loaded) {
        ofSetColor(100, 150, 200); // Bleu si activé
    } else {
        ofSetColor(80, 80, 80); // Gris si V3 n'est pas chargé
    }
    ofFill();
    ofDrawRectangle(moveV3Btn);
    ofNoFill();
    ofSetColor(255);
    ofDrawRectangle(moveV3Btn);
    ofDrawBitmapString("->V3", moveV3Btn.x + 8, moveV3Btn.y + 20);
    ofPopStyle();

    // Boutons Focus WXCVB
    for(int i=0; i<6; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr != nullptr);
        if(isLoaded) {
            ofSetColor(100, 150, 200); // Bleu comme ->V3
        } else {
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofDrawRectangle(focusBtns[i]);
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(focusBtns[i]);
        ofDrawBitmapString(focusNames[i], focusBtns[i].x + 4, focusBtns[i].y + 20);
        ofPopStyle();
    }

    // Boutons WXCVB
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

        if(!isLoaded) {
            ofSetColor(80, 80, 80);
        } else if(!isOn) {
            ofSetColor(50, 200, 50); // Vert si caché/en pause
        } else {
            ofSetColor(200, 50, 50); // Rouge si affiché
        }
        
        ofFill();
        ofDrawRectangle(wxcvbBtns[i]);
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(wxcvbBtns[i]);
        ofDrawBitmapString(wxcvbNames[i], wxcvbBtns[i].x + 4, wxcvbBtns[i].y + 20);
        ofPopStyle();
    }

    // Boutons G (Gabarits / Backgrounds)
    for(int i=0; i<3; i++) {
        ofPushStyle();
        bool isLoaded = (mainAppPtr != nullptr);
        string text = "GAB ";
        
        if(isLoaded) {
            if(i == 0) text += "M:" + ofToString(mainAppPtr->gabMode);
            else if(i == 1 && mainAppPtr->roomApp) text += "R:" + string(mainAppPtr->roomApp->wallAlpha > 50 ? "ON" : "OFF");
            else if(i == 2 && mainAppPtr->scene2D) text += "S:" + ofToString(mainAppPtr->scene2D->bgDisplayMode);
        }

        if(!isLoaded) {
            ofSetColor(80, 80, 80);
        } else {
            ofSetColor(100, 150, 200); // Bleu pour options d'affichage
        }
        
        ofFill();
        ofDrawRectangle(gabBtns[i]);
        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(gabBtns[i]);
        ofDrawBitmapString(text, gabBtns[i].x + 2, gabBtns[i].y + 20); // Marge ajustee
        ofPopStyle();
    }

    // Toggles de la Room App (À gauche)
    for(auto& t : roomToggles) {
        if(t.getState()) {
            ofSetColor(0, 150, 200); // Couleur bleutée pour la 3D
        } else {
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofDrawRectangle(t.rect);
        
        ofNoFill();
        ofSetColor(200);
        ofDrawRectangle(t.rect);
        
        ofSetColor(255);
        ofDrawBitmapString(t.name, t.rect.x + 5, t.rect.y + 14);
    }

    // Toggles de la Scene 2D SIDE
    ofPushStyle();
    for(auto& t : layerToggles) {
        if(*(t.valuePtr)) {
            ofSetColor(0, 200, 100);
        } else {
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofDrawRectangle(t.rect);
        
        ofNoFill();
        ofSetColor(200);
        ofDrawRectangle(t.rect);
        
        ofSetColor(255);
        ofDrawBitmapString(t.name, t.rect.x + 5, t.rect.y + 14);
    }

    // --- SEPARATEURS VISUELS ---
    ofSetColor(255, 255, 255, 100);
    ofDrawLine(220, 80, 220, 480); // Séparateur Room 3D / Scene 2D
    ofDrawLine(575, 80, 575, 480); // Séparateur Scene 2D / Boutons Creatures
    
    // --- BOUTONS CREATURES ---
    for(int i=0; i<creatureButtons.size(); i++) {
        auto& b = creatureButtons[i];
        if(i == selectedCreatureIndex) ofSetColor(200, 200, 0); // Jaune si sélectionné
        else ofSetColor(80, 80, 80);
        
        ofFill();
        ofDrawRectangle(b.rect);
        ofNoFill();
        ofSetColor(200);
        ofDrawRectangle(b.rect);
        ofSetColor(255);
        ofDrawBitmapString(b.name, b.rect.x + 5, b.rect.y + 14);
    }

    // --- BOUTON CLEAR ALL ---
    ofSetColor(200, 50, 50);
    ofFill();
    ofDrawRectangle(clearAllCreaturesBtn);
    ofNoFill();
    ofSetColor(255);
    ofDrawRectangle(clearAllCreaturesBtn);
    ofDrawBitmapString("CLEAR ALL CREATURES", clearAllCreaturesBtn.x + 5, clearAllCreaturesBtn.y + 14);

    // --- BOUTONS INTERACTIFS ---
    ofSetColor(255);
    ofDrawBitmapString("INTERACTIVE LAYERS (Touche 'A')", clearAllCreaturesBtn.x, clearAllCreaturesBtn.y + 40);
    for(int i=0; i<interactiveButtons.size(); i++) {
        auto& b = interactiveButtons[i];
        if(i == selectedInteractiveIndex) ofSetColor(200, 100, 200); // Violet si sélectionné
        else ofSetColor(80, 80, 80);
        
        ofFill();
        ofDrawRectangle(b.rect);
        ofNoFill();
        ofSetColor(200);
        ofDrawRectangle(b.rect);
        ofSetColor(255);
        ofDrawBitmapString(b.name, b.rect.x + 5, b.rect.y + 14);
    }
    ofPopStyle();

    // --- DESSIN DES BOUTONS DE LECTURE (TOUJOURS VISIBLES) ---
    if (player) {
        ofPushStyle();
        if (player->isLoopMode()) ofSetColor(255, 150, 0); else ofSetColor(100);
        ofFill(); ofDrawRectangle(loopButtonRect);
        ofSetColor(255); ofDrawBitmapString("LOOP MODE: " + string(player->isLoopMode() ? "ON" : "OFF"), loopButtonRect.x + 15, loopButtonRect.y + 20);

        if (player->isActive()) ofSetColor(0, 200, 50); else ofSetColor(100);
        ofFill(); ofDrawRectangle(toggleButtonRect);
        ofSetColor(255); ofDrawBitmapString("GO PLAYLIST [8]: " + string(player->isActive() ? "ON" : "OFF"), toggleButtonRect.x + 10, toggleButtonRect.y + 20);

        if (player->isSimulating32Videos()) ofSetColor(200, 0, 200); else ofSetColor(100);
        ofFill(); ofDrawRectangle(simButtonRect);
        ofSetColor(255); ofDrawBitmapString("SIMULATE 32 VID: " + string(player->isSimulating32Videos() ? "ON" : "OFF"), simButtonRect.x + 10, simButtonRect.y + 20);
        
        if (player->isUserSelectedNext()) {
            int upcomingIdx = player->getUpcomingVideoIndex();
            auto& vids = player->getVideos();
            if (upcomingIdx >= 0 && upcomingIdx < vids.size()) {
                ofDrawBitmapStringHighlight("FORCAGE MANUEL vers : " + vids[upcomingIdx].endFrame, simButtonRect.getRight() + 20, simButtonRect.y + 20, ofColor(50, 200, 50), ofColor(0));
            }
        }
        
        // --- ACCORDEON TEMPS DE PAUSE ---
        if (bPauseAccordionOpen) ofSetColor(150, 150, 200); else ofSetColor(100);
        ofFill(); ofDrawRectangle(pauseAccordionBtn);
        ofSetColor(255);
        string loopWarning = player->isLoopMode() ? " (Ignore)" : "";
        ofDrawBitmapString("PAUSE: " + ofToString(player->pauseDurationFrames) + "f" + loopWarning + (bPauseAccordionOpen ? " [-]" : " [+]"), pauseAccordionBtn.x + 5, pauseAccordionBtn.y + 20);

        if (bPauseAccordionOpen) {
            for(int i=0; i<pauseOptions.size(); i++) {
                if (player->pauseDurationFrames == pauseOptions[i]) ofSetColor(200, 200, 50); else ofSetColor(80);
                ofFill(); ofDrawRectangle(pauseOptionRects[i]);
                ofNoFill(); ofSetColor(200); ofDrawRectangle(pauseOptionRects[i]);
                ofSetColor(255);
                ofDrawBitmapString(ofToString(pauseOptions[i]) + " frames", pauseOptionRects[i].x + 20, pauseOptionRects[i].y + 20);
            }
        }
        ofPopStyle();
    }

    // Le diagramme commence sous l'accordéon (ouvert ou fermé)
    float startYForDiagram = bPauseAccordionOpen && !pauseOptionRects.empty() ? pauseOptionRects.back().getBottom() + 40 : pauseAccordionBtn.getBottom() + 40;

    // --- CALCUL ET DESSIN DE LA ZONE DU DIAGRAMME ---
    // On force un rayon très grand, indépendant de la petite taille de la fenêtre
    float radius = 128.0f; 
    float cx = std::max((float)ofGetWidth() * 0.5f, radius + 100.0f);
    float cy = startYForDiagram + radius + 100.0f;
    float boxSize = radius * 2.0f + 150.0f; // Marge confortable pour le cadre

    ofPushStyle();
    ofSetColor(255, 255, 255, 15);
    ofFill();
    ofDrawRectangle(cx - boxSize * 0.5f, cy - boxSize * 0.5f, boxSize, boxSize);
    ofSetColor(255, 255, 255, 200);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(cx - boxSize * 0.5f, cy - boxSize * 0.5f, boxSize, boxSize);
    ofPopStyle();

    if(!player || player->getVideos().empty()) {
        ofSetColor(255);
        string msg = "En attente de chargement de la playlist (Glissez le dossier ici)...";
        ofDrawBitmapStringHighlight(msg, cx - msg.length() * 4, cy, ofColor(0, 0, 0, 150), ofColor(255));
    } else {
        auto& videos = player->getVideos();
        int curIdx = player->getCurrentVideoIndex();
        int upcomingIdx = player->getUpcomingVideoIndex();
        float currentPos = player->getVideoPosition();

        ofPushStyle();
        if(curIdx >= 0 && curIdx < videos.size()) {
            string fileName = ofFilePath::getFileName(videos[curIdx].path);
            string msg = player->isPaused() ? "EN PAUSE SUR : " : "EN LECTURE : ";
            msg += fileName;
            ofDrawBitmapStringHighlight(msg, cx - boxSize * 0.5f + 10, cy - boxSize * 0.5f + 20, ofColor(0, 0, 0, 200), ofColor(0, 255, 0));
        }
        ofPopStyle();

    int i = 0;
    int n = nodes.size();
    if(n > 0) {
        for(auto& kv : nodes) {
            float angle = TWO_PI * float(i) / float(n) - HALF_PI;
            kv.second.pos.set(cx + cos(angle) * radius, cy + sin(angle) * radius);
            i++;
        }
    }

        ofPushStyle();
        ofSetLineWidth(2);
    for(int j = 0; j < videos.size(); ++j) {
        auto& v = videos[j];
        if(nodes.count(v.startFrame) && nodes.count(v.endFrame)) {
            ofVec2f p1 = nodes[v.startFrame].pos;
            ofVec2f p2 = nodes[v.endFrame].pos;
            bool isActive = (j == curIdx);
            bool isNext = (j == upcomingIdx);
            
            bool isSamePath = false;
            if (curIdx >= 0 && upcomingIdx >= 0 && curIdx < videos.size() && upcomingIdx < videos.size()) {
                if (videos[curIdx].startFrame == videos[upcomingIdx].startFrame && 
                    videos[curIdx].endFrame == videos[upcomingIdx].endFrame) {
                    isSamePath = true;
                }
            }

            if((isActive || isNext) && isSamePath) {
                ofSetColor(255, 255, 0, 255); // Jaune pour indiquer que la prochaine repasse au même endroit
                ofSetLineWidth(4);
            } else if(isActive) {
                ofSetColor(255, 50, 50, 255);
                ofSetLineWidth(4);
            } else if (isNext) {
                ofSetColor(50, 255, 50, 200);
                ofSetLineWidth(3);
            } else {
                ofSetColor(100, 200, 255, 120);
                ofSetLineWidth(1.5);
            }

            float arrowPosPct = isActive ? currentPos : 0.7f;

            if(v.startFrame == v.endFrame) {
                ofVec2f dir = (p1 - ofVec2f(cx, cy)).getNormalized();
                ofVec2f loopCenter = p1 + dir * 60;
                ofNoFill();
                ofDrawCircle(loopCenter, 30);
                ofFill();
                
                float angle = arrowPosPct * TWO_PI;
                ofVec2f perp(-dir.y, dir.x);
                ofVec2f arrowPos = loopCenter - dir * 30 * cos(angle) + perp * 30 * sin(angle);
                ofVec2f tangent = dir * 30 * sin(angle) + perp * 30 * cos(angle);
                tangent.normalize();
                
                ofVec2f arrPerp(-tangent.y, tangent.x);
                ofDrawTriangle(arrowPos + tangent * 15, arrowPos + arrPerp * 10 - tangent * 6, arrowPos - arrPerp * 10 - tangent * 6);
            } else {
                ofDrawLine(p1, p2);
                
                ofVec2f dir = p2 - p1;
                ofVec2f nDir = dir.getNormalized();
                ofVec2f mid = p1 + dir * arrowPosPct;
                ofVec2f perp(-nDir.y, nDir.x);
                
                ofDrawTriangle(mid + nDir * 15, mid + perp * 10 - nDir * 6, mid - perp * 10 - nDir * 6);
            }
        }
    }

    for(auto& kv : nodes) {
        bool isDeadEnd = (std::find(deadEnds.begin(), deadEnds.end(), kv.first) != deadEnds.end());
        
        if(isDeadEnd) ofSetColor(255, 50, 50); // Rouge vif
        else ofSetColor(220); // Gris par défaut
        
        ofFill();
        ofDrawCircle(kv.second.pos, 15);

        ofSetColor(255);
        if(isDeadEnd) {
            ofDrawBitmapStringHighlight(kv.first + " (CUL-DE-SAC)", kv.second.pos.x + 20, kv.second.pos.y + 6, ofColor(200, 0, 0), ofColor(255));
        } else {
            ofDrawBitmapStringHighlight(kv.first, kv.second.pos.x + 20, kv.second.pos.y + 6);
        }
    }

        ofPopStyle();
    }
    
    // --- DETECTION DES TOOLTIPS (Au survol) ---
    ofVec2f worldM = getTransformedMouse(ofGetMouseX(), ofGetMouseY());
    string hoveredTooltip = "";

    for(int i=0; i<4; i++) {
        if(viewBtns[i].inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("V" + ofToString(i+1));
    }
    if(moveV3Btn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("->V3");

    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText(wxcvbNames[i]);
        if(focusBtns[i].inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText(focusNames[i]);
    }

    for(int i=0; i<3; i++) {
        if(gabBtns[i].inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("GAB " + ofToString(i));
    }

    for(auto& t : roomToggles) {
        if(t.rect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText(t.name);
    }
    for(auto& t : layerToggles) {
        if(t.rect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText(t.name);
    }
    for(auto& b : creatureButtons) {
        if(b.rect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText(b.name);
    }
    for(auto& b : interactiveButtons) {
        if(b.rect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("INT_" + b.name);
    }

    if(clearAllCreaturesBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("CLEAR_CREATURES");
    if(loopButtonRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("LOOP");
    if(toggleButtonRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("PLAY");
    if(simButtonRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("SIMU");
    if(pauseAccordionBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("PAUSE");
    
    if (bPauseAccordionOpen) {
        for(size_t i=0; i<pauseOptionRects.size(); i++) {
            if (pauseOptionRects[i].inside(worldM)) hoveredTooltip = "Arret de " + ofToString(pauseOptions[i]) + " frames avant la prochaine video.";
        }
    }
    for (auto& kv : nodes) {
        if (kv.second.pos.distance(worldM) < 30.0f) hoveredTooltip = "Clic : Forcer la prochaine lecture vers le noeud [" + kv.first + "]";
    }
    
    ofPopMatrix();

    // HUD FIXE (Non affecté par le zoom/pan)
    ofPushStyle();
    if(isSpacePressed) {
        ofDrawBitmapStringHighlight("ESPACE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20, ofColor(50, 200, 50), ofColor(0));
    } else {
        ofSetColor(255);
        ofDrawBitmapStringHighlight("ESPACE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20);
    }

    ofSetColor(255);
    ofDrawBitmapStringHighlight("PLAYLIST VISUALIZER [N]", 10, 20);
    if(player) {
        int curIdx = player->getCurrentVideoIndex();
        auto& videos = player->getVideos();
        if(curIdx >= 0 && curIdx < videos.size()) {
            string fileName = ofFilePath::getFileName(videos[curIdx].path);
            ofDrawBitmapStringHighlight("Trajet : " + videos[curIdx].startFrame + " -> " + videos[curIdx].endFrame, 10, 40);
            ofDrawBitmapStringHighlight("Fichier : " + fileName, 10, 60);
        }
        
        if(!deadEnds.empty()) {
            string warningMsg = "ATTENTION : " + ofToString(deadEnds.size()) + " Cul(s)-de-sac detecte(s) ! Bloquage sur : ";
            for(size_t k = 0; k < deadEnds.size(); ++k) {
                warningMsg += deadEnds[k] + (k < deadEnds.size() - 1 ? ", " : "");
            }
            ofDrawBitmapStringHighlight(warningMsg, 10, 80, ofColor(200, 0, 0), ofColor(255));
        }
    }
    ofPopStyle();
    
    // Dessin du tooltip par-dessus tout, non affecte par le Zoom (en coordonnees ecran brutes)
    if(!hoveredTooltip.empty()) {
        tooltipManager.drawTooltip(hoveredTooltip, ofGetMouseX(), ofGetMouseY());
    }

    // Feedback visuel curseur
    if (isSpacePressed) {
        ofSetColor(50, 200, 50, 150);
        ofFill();
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
        ofSetColor(255);
        ofNoFill();
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
    }
}

void PlaylistVisualizerApp::mousePressed(int x, int y, int button) {
    lastMouse.set(x, y);
    
    if (!bEnabled) return;
    if (isSpacePressed) return; // Bloque le clic sur les boutons lors du déplacement

    ofVec2f worldM = getTransformedMouse(x, y);

    for(int i=0; i<4; i++) {
        if(viewBtns[i].inside(worldM.x, worldM.y)) {
            bool isLoaded = (mainAppPtr && mainAppPtr->viewApps.size() == 4 && mainAppPtr->viewApps[i]);
            if(isLoaded) {
                viewHidden[i] = !viewHidden[i];
                mainAppPtr->viewApps[i]->bEnabled = !viewHidden[i]; // Coupe le calcul si caché
                
                // FOCUS : Amène la fenêtre ViewApp au premier plan, puis redonne le focus à Playlist
                if(!viewHidden[i] && mainAppPtr->viewApps[i]->myWindow) {
                    auto targetGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->viewApps[i]->myWindow);
                    auto playlistGlfw = dynamic_pointer_cast<ofAppGLFWWindow>(mainAppPtr->playlistWindowPtr);
                    if(targetGlfw) glfwFocusWindow(targetGlfw->getGLFWWindow());
                    if(playlistGlfw) glfwFocusWindow(playlistGlfw->getGLFWWindow());
                }
            }
            return;
        }
    }

    // Clic pour déplacer la V3
    if(moveV3Btn.inside(worldM.x, worldM.y)) {
        bool v3Loaded = (mainAppPtr && mainAppPtr->viewApps.size() > 2 && mainAppPtr->viewApps[2]);
        if(v3Loaded) {
            // Déplace la fenêtre sur l'écran secondaire (à la droite de l'écran principal)
            mainAppPtr->viewApps[2]->moveWindow(ofGetScreenWidth(), 0);
        }
        return;
    }
    
    // Clic pour Focus les fenêtres WXCVB
    for(int i=0; i<6; i++) {
        if(focusBtns[i].inside(worldM.x, worldM.y)) {
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
                        glfwRestoreWindow(targetGlfw->getGLFWWindow()); // Restaure si minimisée
                        glfwFocusWindow(targetGlfw->getGLFWWindow());   // Force le focus
                    }
                }
            }
            return;
        }
    }

    // Interaction avec les boutons WXCVB
    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(worldM.x, worldM.y)) {
            if(mainAppPtr) {
                if(wxcvbKeys[i] != 0) mainAppPtr->keyPressed(wxcvbKeys[i]);
                else if(i == 0) mainAppPtr->bDrawMain = !mainAppPtr->bDrawMain;
                
                // FOCUS : Amène la fenêtre WXCVB au premier plan, puis redonne le focus à Playlist
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
            return;
        }
    }

    // Interaction avec les boutons GAB
    for(int i=0; i<3; i++) {
        if(gabBtns[i].inside(worldM.x, worldM.y)) {
            if(mainAppPtr) {
                if(i == 0) mainAppPtr->keyPressed('g');
                else if(i == 1 && mainAppPtr->roomApp) mainAppPtr->roomApp->keyPressed('g');
                else if(i == 2 && mainAppPtr->scene2D) mainAppPtr->scene2D->keyPressed('g');
            }
            return;
        }
    }

    // Interaction avec les boutons de la room
    for(auto& t : roomToggles) {
        if(t.rect.inside(worldM.x, worldM.y)) {
            t.toggle();
            return; 
        }
    }

    // Interaction avec les boutons de layers (Soumis au pan/zoom)
    for(auto& t : layerToggles) {
        if(t.rect.inside(worldM.x, worldM.y)) {
            *(t.valuePtr) = !(*(t.valuePtr));
            return; // On arrête là pour ne pas cliquer sur la simulation par erreur
        }
    }

    // Interaction avec les boutons de créatures
    for(int i=0; i<creatureButtons.size(); i++) {
        if(creatureButtons[i].rect.inside(worldM.x, worldM.y)) {
            selectedCreatureIndex = i;
            selectedInteractiveIndex = -1; // Exclusivite
            if(scene2D) {
                scene2D->layerManager.selectedCreatureToSpawn = creatureButtons[i].name;
                scene2D->layerManager.selectedInteractiveLayer = "";
            }
            return;
        }
    }
    
    // Interaction avec les boutons interactifs
    for(int i=0; i<interactiveButtons.size(); i++) {
        if(interactiveButtons[i].rect.inside(worldM.x, worldM.y)) {
            if (selectedInteractiveIndex == i) {
                selectedInteractiveIndex = -1; // Toggle off
                if(scene2D) scene2D->layerManager.selectedInteractiveLayer = "";
            } else {
                selectedInteractiveIndex = i;
                selectedCreatureIndex = -1; // Exclusivite
                if(scene2D) {
                    scene2D->layerManager.selectedInteractiveLayer = interactiveButtons[i].name;
                    scene2D->layerManager.selectedCreatureToSpawn = "";
                }
            }
            return;
        }
    }
    
    if(clearAllCreaturesBtn.inside(worldM.x, worldM.y)) {
        clearAllCreatures();
        return;
    }

    if (!player) return;
    
    if (loopButtonRect.inside(worldM.x, worldM.y)) {
        player->toggleLoopMode();
        return;
    }
    
    if (toggleButtonRect.inside(worldM.x, worldM.y)) {
        player->toggle();
        if (bDrawScene360VideoPtr) {
            *bDrawScene360VideoPtr = player->isActive(); // Synchronise avec la vue Room
        }
        return;
    }

    if (simButtonRect.inside(worldM.x, worldM.y)) {
        player->toggleSimulate32Videos();
        return;
    }
    
    if (pauseAccordionBtn.inside(worldM.x, worldM.y)) {
        bPauseAccordionOpen = !bPauseAccordionOpen;
        return;
    }
    
    if (bPauseAccordionOpen) {
        for(int i=0; i<pauseOptionRects.size(); i++) {
            if (pauseOptionRects[i].inside(worldM.x, worldM.y)) {
                player->pauseDurationFrames = pauseOptions[i];
                bPauseAccordionOpen = false;
                return;
            }
        }
    }
    
    // Vérifie si on a cliqué sur un noeud
    for (auto& kv : nodes) {
        // 30.0f de rayon de clic pour être souple
        if (kv.second.pos.distance(worldM) < 30.0f) {
            player->forceNextVideoToNode(kv.first);
            break;
        }
    }
}

void PlaylistVisualizerApp::mouseDragged(int x, int y, int button) {
    if(isSpacePressed) {
        ofVec2f currentMouse(x, y);
        pan += (currentMouse - lastMouse);
        lastMouse = currentMouse;
    }
}

void PlaylistVisualizerApp::mouseReleased(int x, int y, int button) {}

void PlaylistVisualizerApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse(x, y);
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    zoom = ofClamp(zoom * zoomFactor, 0.01f, 50.0f);
    pan.x = x - worldM.x * zoom;
    pan.y = y - worldM.y * zoom;
}

void PlaylistVisualizerApp::keyPressed(int key) { 
    if(key == ' ') isSpacePressed = true; 
    if(key == 'r' || key == 'R') {
        zoom = 0.8f;
        pan.set(0, 130);
    }
}

void PlaylistVisualizerApp::keyReleased(int key) { if(key == ' ') isSpacePressed = false; }

ofVec2f PlaylistVisualizerApp::getTransformedMouse(int x, int y) {
    return ofVec2f((x - pan.x) / zoom, (y - pan.y) / zoom);
}