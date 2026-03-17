#include "PlaylistVisualizerApp.h"
#include <algorithm>
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "ofApp.h"
#include "ViewApp.h"
#include "ofAppGLFWWindow.h"
#include "GLFW/glfw3.h"
#include "RoomPreview.h"
#include "ButtonApp.h"

void PlaylistVisualizerApp::setup() {
    ofSetBackgroundColor(25);
    loopButtonRect.set(10, 80, 180, 30);
    toggleButtonRect.set(10, 120, 180, 30);
    simButtonRect.set(10, 160, 180, 30);
    doubleSpeedBtnRect.set(10, 200, 180, 30);
    muteBtnRect.set(10, 240, 180, 30);
    crop106BtnRect.set(10, 280, 180, 30);
    pauseAccordionBtn.set(10, 320, 180, 30);
    infinitePauseBtnRect.set(10, 360, 180, 30);
    videoInfoBox.set(10, 300, 350, 65); // Position par défaut du bloc d'info
    
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
    
    editBtnRect.set(10, 110-75, 60, 30);
    saveBtnRect.set(10, 150-75, 60, 30);
    loadBtnRect.set(10, 190-75, 60, 30);
    
    // Positions par defaut (elles seront ecrasees par le loadButtonPositions si existantes)
    apiKeyBox.set(600, 100, 400, 30);
    themeBox.set(600, 140, 400, 30);
    genRoomBtn.set(600, 180, 400, 30);
    promptVid1Box.set(600, 220, 400, 30);
    genVidLastBtn.set(600, 260, 400, 30);
    promptVid2Box.set(600, 300, 400, 30);
    genVid2LastBtn.set(600, 340, 400, 30);
    
    modelAccordionBtn.set(600, 380, 400, 30);
    for(int i=0; i<modelOptions.size(); i++) {
        modelOptionRects.push_back(ofRectangle(600, 380 + 30 + i * 30, 400, 30));
    }
    imageSizeBtn.set(600, 420, 195, 30);
    videoResBtn.set(805, 420, 195, 30);
    genTextToRoomBtn.set(600, 460, 400, 30);
    
    loadButtonPositions();
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
    vector<string> iNames = {"GroPuyo", "Puyo", "Bubble", "Poulpe", "Sardine"};
    
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
    doubleSpeedBtnRect.y = topOffset + 120;
    muteBtnRect.set(10, topOffset + 160, 180, 30);
    crop106BtnRect.set(10, topOffset + 200, 180, 30);
    pauseAccordionBtn.set(10, topOffset + 240, 180, 30);
    // infinitePauseBtnRect.y ajusté dynamiquement dans update()
    // videoInfoBox n'est pas lié à cet alignement vertical, il est libre
    
    pauseOptionRects.clear();
    for(int i=0; i<pauseOptions.size(); i++) {
        pauseOptionRects.push_back(ofRectangle(10, pauseAccordionBtn.y + 30 + i * 30, 180, 30));
    }
    
    // Synchronisation initiale
    if(scene2D && !creatureButtons.empty()) {
        scene2D->layerManager.selectedCreatureToSpawn = creatureButtons[selectedCreatureIndex].name;
    }
    
    loadButtonPositions();
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
    addToggle("Show Beams", [this](){ return roomApp->projection.getShowBeams(); }, [this](){ roomApp->projection.keyPressed('t'); });
    addToggle("Plan Colle", [this](){ return roomApp->projection.getShowPlanColle(); }, [this](){ roomApp->projection.keyPressed('n'); });
    addToggle("Cur Reflet", [this](){ return roomApp->cursorSquare.bDrawReflections; }, [this](){ roomApp->cursorSquare.bDrawReflections = !roomApp->cursorSquare.bDrawReflections; });
    addToggle("Prev Interact", [this](){ return mainAppPtr && mainAppPtr->roomPreviewApp ? mainAppPtr->roomPreviewApp->bDrawInteraction : false; }, [this](){ if(mainAppPtr && mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->bDrawInteraction = !mainAppPtr->roomPreviewApp->bDrawInteraction; });
    addToggle("Prev Cursor", [this](){ return mainAppPtr && mainAppPtr->roomPreviewApp ? mainAppPtr->roomPreviewApp->bShowCursor : false; }, [this](){ if(mainAppPtr && mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->bShowCursor = !mainAppPtr->roomPreviewApp->bShowCursor; });

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
    
    loadButtonPositions();
}

void PlaylistVisualizerApp::setupRoomActionBtns() {
    if(!roomApp) return;
    roomActionBtns.clear();

    auto addAction = [&](string name, std::function<void()> action, bool continuous = false, std::function<bool()> getState = nullptr) {
        roomActionBtns.push_back({name, ofRectangle(), action, continuous, getState});
    };

    addAction("Cam Reset [R]", [this](){ roomApp->keyPressed('r'); }, false);
    addAction("Cam Center [L]", [this](){ 
        roomApp->camGlobal.setDistance(0); 
        roomApp->camGlobal.setPosition(0, 600, 0); 
    }, false);
    addAction("Sphere [P]", [this](){ roomApp->atmosphere.keyPressed('p'); uiStateSphereP = !uiStateSphereP; }, false, [this](){ return uiStateSphereP; });
    addAction("Disco [M]", [this](){ roomApp->atmosphere.keyPressed('m'); uiStateDiscoM = !uiStateDiscoM; }, false, [this](){ return uiStateDiscoM; });
    addAction("Mode 360 [O]", [this](){ roomApp->atmosphere.keyPressed('o'); }, false, [this](){ return roomApp->atmosphere.bShow360; });
    addAction("AutoRot [I]", [this](){ roomApp->atmosphere.keyPressed('i'); }, false);
    
    addAction("RotY+ [->]", [this](){ roomApp->atmosphere.keyPressed(OF_KEY_RIGHT); }, true);
    addAction("RotY- [<-]", [this](){ roomApp->atmosphere.keyPressed(OF_KEY_LEFT); }, true);
    addAction("RotX+ [v]", [this](){ roomApp->atmosphere.keyPressed(OF_KEY_DOWN); }, true);
    addAction("RotX- [^]", [this](){ roomApp->atmosphere.keyPressed(OF_KEY_UP); }, true);
    addAction("RotZ+ [2]", [this](){ roomApp->atmosphere.keyPressed('2'); }, true);
    addAction("RotZ- [1]", [this](){ roomApp->atmosphere.keyPressed('1'); }, true);
    addAction("OffY+ [3]", [this](){ roomApp->atmosphere.keyPressed('3'); }, true);
    addAction("OffY- [4]", [this](){ roomApp->atmosphere.keyPressed('4'); }, true);
    addAction("Beam +", [this](){ roomApp->projection.keyPressed('+'); }, true);
    addAction("Beam -", [this](){ roomApp->projection.keyPressed('-'); }, true);
    
    addAction("Beam Pan L", [this](){ roomApp->projection.keyPressed(OF_KEY_LEFT); }, true);
    addAction("Beam Pan R", [this](){ roomApp->projection.keyPressed(OF_KEY_RIGHT); }, true);
    addAction("Beam Tilt U", [this](){ roomApp->projection.keyPressed(OF_KEY_UP); }, true);
    addAction("Beam Tilt D", [this](){ roomApp->projection.keyPressed(OF_KEY_DOWN); }, true);
    addAction("Beam Roll W", [this](){ roomApp->projection.keyPressed('w'); }, true);
    addAction("Beam Roll X", [this](){ roomApp->projection.keyPressed('x'); }, true);
    addAction("Plan Ang+ [Q]", [this](){ roomApp->projection.movePlanColle(1.0f, 0.0f); }, true);
    addAction("Plan Ang- [D]", [this](){ roomApp->projection.movePlanColle(-1.0f, 0.0f); }, true);
    addAction("Plan Ele+ [Z]", [this](){ roomApp->projection.movePlanColle(0.0f, 1.0f); }, true);
    addAction("Plan Ele- [S]", [this](){ roomApp->projection.movePlanColle(0.0f, -1.0f); }, true);
    addAction("Clear Flys [X]", [this](){ roomApp->lightFlyRing.clearLights(); }, false);

    int cols = 2;
    float bw = 100;
    float bh = 20;
    float pad = 5;
    float startX = 10;
    float startY = 80 + ((roomToggles.size() + cols - 1) / cols) * (bh + pad) + 30; // Juste sous les toggles de la room

    for(int i=0; i<roomActionBtns.size(); i++) {
        int c = i % cols;
        int r = i / cols;
        roomActionBtns[i].rect.set(startX + c*(bw+pad), startY + r*(bh+pad), bw, bh);
    }
    loadButtonPositions();
}

void PlaylistVisualizerApp::setupGlobalActionBtns() {
    globalActionBtns.clear();

    auto addAction = [&](string name, std::function<void()> action, bool continuous = false, std::function<bool()> getState = nullptr) {
        globalActionBtns.push_back({name, ofRectangle(), action, continuous, getState});
    };

    addAction("PAUSE [ESC]", [this](){ 
        if(mainAppPtr) {
            mainAppPtr->bGlobalPause = !mainAppPtr->bGlobalPause;
            if(mainAppPtr->bGlobalPause) mainAppPtr->oscTime = mainAppPtr->localTime;
        }
    }, false);

    addAction("SAVE GAB", [this](){ 
        if(mainAppPtr && mainAppPtr->canvasManager.canvas.isAllocated()) {
            ofPixels pix;
            mainAppPtr->canvasManager.canvas.readToPixels(pix);
            ofSaveImage(pix, "export/gab_frame_" + ofGetTimestampString() + ".png");
            ofLogNotice("PlaylistVisualizerApp") << "Main Canvas Frame saved.";
        }
    }, false);

    addAction("UNDO CREA [D]", [this](){ 
        if(mainAppPtr) {
            mainAppPtr->creatureSystem.removeLast();
            ofLogNotice("PlaylistVisualizerApp") << "Derniere creature retiree.";
        }
    }, false);

    addAction("RECORD [ENTER]", [this](){ 
        if(mainAppPtr) {
            if(mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->keyPressed(OF_KEY_RETURN);
            if(mainAppPtr->viewApps.size() > 2 && mainAppPtr->viewApps[2]) mainAppPtr->viewApps[2]->keyPressed(OF_KEY_RETURN);
            ofLogNotice("PlaylistVisualizerApp") << "Enregistrement bascule (ENTER).";
        }
    }, false);

    addAction("BLUR SHADER", [this](){ 
        if(mainAppPtr) {
            // 1. On cherche l'état inverse du premier ViewApp actif pour synchroniser tout le monde
            bool targetState = true;
            for(auto& vApp : mainAppPtr->viewApps) {
                if(vApp) { targetState = !vApp->bBlur; break; }
            }
            
            // 2. On applique ce nouvel état à tous les ViewApps instanciés
            for(auto& vApp : mainAppPtr->viewApps) {
                if(vApp) vApp->bBlur = targetState;
            }
            ofLogNotice("PlaylistVisualizerApp") << "Flou global bascule a : " << (targetState ? "ON" : "OFF");
        }
    }, false);

    addAction("EXP SCENE2D", [this](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportFullScene();
    }, false);

    addAction("EXP COLLIDER", [this](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportColliders();
    }, false);

    addAction("GAB 3-OFF-3", [this](){ 
        if(mainAppPtr) {
            mainAppPtr->gabMode = 3;
            if(mainAppPtr->roomApp) mainAppPtr->roomApp->wallAlpha = 0.0f;
            if(mainAppPtr->scene2D) mainAppPtr->scene2D->bgDisplayMode = 3;
        }
    }, false);

    addAction("BTN WORMS", [this](){ 
        if(mainAppPtr && mainAppPtr->buttonApp) {
            mainAppPtr->buttonApp->buttonWindow.toggleWorms();
        }
    }, false);

    globalActionBtns[0].rect.set(600, -65, 100, 30);
    globalActionBtns[1].rect.set(710, -65, 100, 30);
    globalActionBtns[2].rect.set(820, -65, 120, 30);
    globalActionBtns[3].rect.set(950, -65, 120, 30);
    globalActionBtns[4].rect.set(1080, -65, 100, 30);
    globalActionBtns[5].rect.set(1190, -65, 100, 30);
    globalActionBtns[6].rect.set(1300, -65, 110, 30);
    globalActionBtns[7].rect.set(1420, -65, 110, 30);
    globalActionBtns[8].rect.set(1540, -65, 100, 30);
    loadButtonPositions();
}

void PlaylistVisualizerApp::clearAllCreatures() {
    if(!scene2D) return;
    auto& mgr = scene2D->layerManager;
    mgr.creatureSystem.clear();
    mgr.cousinCons.clear();
    mgr.halos.clear();
    mgr.groPuyoLayer.puyos.clear();
}

void PlaylistVisualizerApp::update() {
    if(!bEnabled) return;

    if(scene2D && layerToggles.empty()) {
        setupLayerToggles();
    }

    if(roomApp && roomToggles.empty()) {
        setupRoomToggles();
    }
    
    if(roomApp && roomActionBtns.empty()) {
        setupRoomActionBtns();
    }
    
    if(globalActionBtns.empty()) {
        setupGlobalActionBtns();
    }

    // --- MISE A JOUR DYNAMIQUE DE L'ACCORDEON (ASCENSEUR) ---
    if(pauseOptionRects.size() == pauseOptions.size()) {
        for(size_t i=0; i<pauseOptions.size(); i++) {
            pauseOptionRects[i].x = pauseAccordionBtn.x;
            pauseOptionRects[i].y = pauseAccordionBtn.y + pauseAccordionBtn.height + i * pauseAccordionBtn.height;
            pauseOptionRects[i].width = pauseAccordionBtn.width;
            pauseOptionRects[i].height = pauseAccordionBtn.height;
        }
    }
    
    if(modelOptionRects.size() == modelOptions.size()) {
        for(size_t i=0; i<modelOptions.size(); i++) {
            modelOptionRects[i].x = modelAccordionBtn.x;
            modelOptionRects[i].y = modelAccordionBtn.y + modelAccordionBtn.height + i * modelAccordionBtn.height;
            modelOptionRects[i].width = modelAccordionBtn.width;
            modelOptionRects[i].height = modelAccordionBtn.height;
        }
    }
    
    if (mainAppPtr) {
        mainAppPtr->geminiGen.setModelName(modelOptions[currentModelIndex]);
        string iSize = imageSizeOptions[currentImageSizeIndex];
        mainAppPtr->geminiGen.setImageSize(iSize == "DEFAULT" ? "" : iSize);
        string vRes = videoResOptions[currentVideoResIndex];
        mainAppPtr->geminiGen.setVideoResolution(vRes == "DEFAULT" ? "" : vRes);
    }
    
    infinitePauseBtnRect.x = pauseAccordionBtn.x;
    infinitePauseBtnRect.width = pauseAccordionBtn.width;
    infinitePauseBtnRect.height = pauseAccordionBtn.height;
    if(bPauseAccordionOpen) {
        infinitePauseBtnRect.y = pauseAccordionBtn.y + pauseAccordionBtn.height + pauseOptions.size() * pauseAccordionBtn.height + 10;
    } else {
        infinitePauseBtnRect.y = pauseAccordionBtn.y + pauseAccordionBtn.height + 10;
    }

    // Pression continue pour les boutons d'action qui le supportent (rotation, etc.)
    if (ofGetMousePressed(0) && !isSpacePressed && !bIsDraggingPan && !bEditMode) {
        ofVec2f worldM = getTransformedMouse(ofGetMouseX(), ofGetMouseY());
        for(auto& btn : roomActionBtns) {
            if(btn.continuous && btn.rect.inside(worldM.x, worldM.y)) {
                btn.action();
            }
        }
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
        } else if(!viewHidden[i]) {
            ofSetColor(50, 200, 50); // Vert si affiché (actif)
        } else {
            ofSetColor(30, 30, 30); // Noir-gris foncé si caché (désactivé)
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
        } else if(isOn) {
            ofSetColor(50, 200, 50); // Vert si affiché (actif)
        } else {
            ofSetColor(30, 30, 30); // Noir-gris foncé si caché/en pause (désactivé)
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
        ofEnableAlphaBlending();
        bool isLoaded = (mainAppPtr != nullptr);
        string text = "GAB ";
        int alpha = 255;
        bool isWireframe = false;
        
        if(isLoaded) {
            if(i == 0) {
                text += "M:" + ofToString(mainAppPtr->gabMode);
                if(mainAppPtr->gabMode == 0) alpha = 255;
                else if(mainAppPtr->gabMode == 1) alpha = 200;
                else if(mainAppPtr->gabMode == 2) alpha = 150;
                else if(mainAppPtr->gabMode == 3) alpha = 100;
                else alpha = 5; // Etat 4 = Presque invisible / OFF
            }
            else if(i == 1 && mainAppPtr->roomApp) {
                bool isOn = mainAppPtr->roomApp->wallAlpha > 50;
                text += "R:" + string(isOn ? "ON" : "OFF");
                alpha = isOn ? 255 : 5;
            }
            else if(i == 2 && mainAppPtr->scene2D) {
                int mode = mainAppPtr->scene2D->bgDisplayMode;
                text += "S:" + ofToString(mode);
                if(mode == 0) alpha = 255;      // Opaque
                else if(mode == 1) alpha = 100;  // Transparent
                else if(mode == 2) {
                    alpha = 255;
                    isWireframe = true;
                } else alpha = 5;
            }
        }

        if(!isLoaded) {
            ofSetColor(80, 80, 80);
            ofFill();
            ofDrawRectangle(gabBtns[i]);
        } else {
            ofSetColor(255, 150, 0, alpha); // Jaune-orange avec transparence dynamique
            
            if(isWireframe) {
                ofNoFill();
                ofSetLineWidth(3); // Filaire épais pour l'état 2
            } else {
                ofFill();
            }
            ofDrawRectangle(gabBtns[i]);
        }
        
        ofNoFill();
        ofSetLineWidth(1);
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
            ofSetColor(64, 94, 104); // Teinté légèrement bleu
        }
        ofFill();
        ofDrawRectangle(t.rect);
        
        ofNoFill();
        ofSetColor(200);
        ofDrawRectangle(t.rect);
        
        ofSetColor(255);
        ofDrawBitmapString(t.name, t.rect.x + 5, t.rect.y + 14);
    }

    // Boutons d'Action de la Room App
    for(auto& b : roomActionBtns) {
        ofPushStyle();
        if (b.getState && b.getState()) {
            ofSetColor(0, 150, 200); // Actif (Bleuté)
        } else {
            ofSetColor(64, 94, 104); // Inactif (Teinté légèrement bleu)
        }
        ofFill();
        ofDrawRectangle(b.rect);
        
        ofNoFill();
        ofSetColor(150);
        ofDrawRectangle(b.rect);
        
        ofSetColor(255);
        ofDrawBitmapString(b.name, b.rect.x + 5, b.rect.y + 14);
        ofPopStyle();
    }

    // Boutons d'Action Globale
    for(auto& b : globalActionBtns) {
        ofPushStyle();
        if (b.name == "PAUSE [ESC]" && mainAppPtr && mainAppPtr->bGlobalPause) {
            ofSetColor(200, 50, 50); // Rouge vif si en pause
        } else if (b.name == "BTN WORMS" && mainAppPtr && mainAppPtr->buttonApp && mainAppPtr->buttonApp->buttonWindow.bDrawWorms) {
            ofSetColor(50, 200, 50); // Vert si actif
        } else {
            ofSetColor(60, 60, 80);
        }
        ofFill();
        ofDrawRectangle(b.rect);
        
        ofNoFill();
        ofSetColor(150);
        ofDrawRectangle(b.rect);
        
        ofSetColor(255);
        ofDrawBitmapString(b.name, b.rect.x + 5, b.rect.y + 20);
        ofPopStyle();
    }

    // Toggles de la Scene 2D SIDE
    ofPushStyle();
    for(auto& t : layerToggles) {
        if(*(t.valuePtr)) {
            ofSetColor(0, 200, 100);
        } else {
            ofSetColor(64, 104, 84); // Teinté légèrement vert
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
        else ofSetColor(104, 104, 64); // Teinté légèrement jaune
        
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
        else ofSetColor(104, 84, 104); // Teinté légèrement violet
        
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
            if (!player->getPlannedPath().empty()) {
                ofDrawBitmapStringHighlight("CHEMIN PLANIFIE vers : " + vids[player->getPlannedPath().back()].endFrame, simButtonRect.getRight() + 20, simButtonRect.y + 20, ofColor(50, 150, 255), ofColor(0));
            } else if (upcomingIdx >= 0 && upcomingIdx < vids.size()) {
                ofDrawBitmapStringHighlight("FORCAGE MANUEL vers : " + vids[upcomingIdx].endFrame, simButtonRect.getRight() + 20, simButtonRect.y + 20, ofColor(50, 200, 50), ofColor(0));
            }
        }
        
        if (player->isDoubleSpeed()) ofSetColor(200, 100, 50); else ofSetColor(100);
        ofFill(); ofDrawRectangle(doubleSpeedBtnRect);
        ofSetColor(255); ofDrawBitmapString("SPEED x2: " + string(player->isDoubleSpeed() ? "ON" : "OFF"), doubleSpeedBtnRect.x + 10, doubleSpeedBtnRect.y + 20);
        
        if (player->isMuted()) ofSetColor(200, 50, 50); else ofSetColor(100);
        ofFill(); ofDrawRectangle(muteBtnRect);
        ofSetColor(255); ofDrawBitmapString("MUTE: " + string(player->isMuted() ? "ON" : "OFF"), muteBtnRect.x + 10, muteBtnRect.y + 20);

        if (player->isCrop106()) ofSetColor(100, 150, 200); else ofSetColor(100);
        ofFill(); ofDrawRectangle(crop106BtnRect);
        ofSetColor(255); ofDrawBitmapString("CROP 106%: " + string(player->isCrop106() ? "ON" : "OFF"), crop106BtnRect.x + 10, crop106BtnRect.y + 20);

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

        // --- HOLD LAST FRAME (Maintenant en dernier, sous l'accordeon) ---
        if (player->isInfinitePause()) ofSetColor(200, 50, 50); else ofSetColor(100);
        ofFill(); ofDrawRectangle(infinitePauseBtnRect);
        ofSetColor(255); ofDrawBitmapString("HOLD LAST FRAME: " + string(player->isInfinitePause() ? "ON" : "OFF"), infinitePauseBtnRect.x + 10, infinitePauseBtnRect.y + 20);

        ofPopStyle();
    }

    // --- BLOC D'INFO VIDEO (déplaçable) ---
    if (player) {
        ofPushStyle();
        // Fond semi-transparent
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
        
        if(!deadEnds.empty()) {
            string warningMsg = "CUL-DE-SAC: " + deadEnds[0];
            if (deadEnds.size() > 1) warningMsg += ", ...";
            ofDrawBitmapStringHighlight(warningMsg, videoInfoBox.x + 10, videoInfoBox.y + 60, ofColor(200, 0, 0), ofColor(255));
        }
        ofPopStyle();
    }

    // Le diagramme commence sous le bouton Hold Last Frame
    float startYForDiagram = infinitePauseBtnRect.getBottom() + 40;

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

            bool isPlanned = false;
            int pathIndex = -1;
            int pathLength = 1;
            if (player && !player->getPlannedPath().empty()) {
                auto& p = player->getPlannedPath();
                auto it = std::find(p.begin(), p.end(), j);
                if (it != p.end()) {
                    isPlanned = true;
                    pathIndex = std::distance(p.begin(), it);
                    pathLength = p.size();
                }
            }

            if((isActive || isNext) && isSamePath && !isPlanned) {
                ofSetColor(255, 255, 0, 255);
                ofSetLineWidth(4);
            } else if(isActive) {
                ofSetColor(255, 50, 50, 255);
                ofSetLineWidth(4);
            } else if (isPlanned) {
                float t = pathLength > 1 ? (float)pathIndex / (pathLength - 1) : 0.0f;
                ofSetColor(50, 255 * (1.0f - t), 50 + 205 * t, 255); // Dégradé de Vert vers Bleu
                ofSetLineWidth(3.5);
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
                
                if (isPlanned) {
                    ofDrawBitmapStringHighlight(ofToString(pathLength - pathIndex), loopCenter.x - 4, loopCenter.y + 4, ofColor(0, 150), ofColor(255));
                }
            } else {
                ofDrawLine(p1, p2);
                
                ofVec2f dir = p2 - p1;
                ofVec2f nDir = dir.getNormalized();
                ofVec2f mid = p1 + dir * arrowPosPct;
                ofVec2f perp(-nDir.y, nDir.x);
                
                ofDrawTriangle(mid + nDir * 15, mid + perp * 10 - nDir * 6, mid - perp * 10 - nDir * 6);
                
                if (isPlanned) {
                    ofVec2f textPos = p1 + dir * 0.5f;
                    ofDrawBitmapStringHighlight(ofToString(pathLength - pathIndex), textPos.x - 4, textPos.y + 4, ofColor(0, 150), ofColor(255));
                }
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
    for(auto& b : roomActionBtns) {
        if(b.rect.inside(worldM)) {
            string t = tooltipManager.getTooltipText(b.name);
            if(t.find("Active ou desactive l'affichage de :") != string::npos) hoveredTooltip = "Action de la Room: " + b.name;
            else hoveredTooltip = t;
        }
    }
    for(auto& b : globalActionBtns) {
        if(b.rect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText(b.name);
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
    if(doubleSpeedBtnRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("SPEED_X2");
    if(muteBtnRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("MUTE");
    if(crop106BtnRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("106CROP");
    if(infinitePauseBtnRect.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("HOLD_FRAME");
    if(videoInfoBox.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("VIDEO_INFO");
    
    // --- GEMINI UI TOOLTIPS ---
    if(apiKeyBox.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("API_KEY_BOX");
    if(themeBox.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("THEME_BOX");
    if(promptVid1Box.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("PROMPT_VID1_BOX");
    if(promptVid2Box.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("PROMPT_VID2_BOX");
    if(genRoomBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("GEN_ROOM_360");
    if(genVidLastBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("GEN_VID_LAST");
    if(genVid2LastBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("GEN_VID_2_LAST");
    
    if(modelAccordionBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("MODEL_ACCORDION");
    if(imageSizeBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("IMG_SIZE_BTN");
    if(videoResBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("VID_RES_BTN");
    if(genTextToRoomBtn.inside(worldM)) hoveredTooltip = tooltipManager.getTooltipText("GEN_TEXT_ROOM");

    if (bPauseAccordionOpen) {
        for(size_t i=0; i<pauseOptionRects.size(); i++) {
            if (pauseOptionRects[i].inside(worldM)) hoveredTooltip = "Arret de " + ofToString(pauseOptions[i]) + " frames avant la prochaine video.";
        }
    }
    for (auto& kv : nodes) {
        if (kv.second.pos.distance(worldM) < 30.0f) hoveredTooltip = "Clic : Calculer un chemin vers le noeud [" + kv.first + "]";
    }
    
    // --- GEMINI UI (A l'interieur du monde zoomable) ---
    ofPushStyle();

    // API Key
    ofFill();
    ofSetColor(bApiKeyFocused ? 60 : 30);
    ofDrawRectangle(apiKeyBox);
    ofNoFill();
    ofSetColor(bApiKeyFocused ? ofColor(50, 200, 50) : ofColor(150));
    ofDrawRectangle(apiKeyBox);
    ofSetColor(255);
    
    string displayApi = apiKeyText;
    if(displayApi.length() > 40) displayApi = "..." + displayApi.substr(displayApi.length() - 37);
    if(displayApi.empty()) displayApi = "Coller Clef API ici...";
    ofDrawBitmapString("API: " + displayApi, apiKeyBox.x + 5, apiKeyBox.y + 20);

    // Theme
    ofFill();
    ofSetColor(bThemeFocused ? 60 : 30);
    ofDrawRectangle(themeBox);
    ofNoFill();
    ofSetColor(bThemeFocused ? ofColor(50, 200, 50) : ofColor(150));
    ofDrawRectangle(themeBox);
    ofSetColor(255);
    
    string displayTheme = themeText;
    if(displayTheme.length() > 40) displayTheme = displayTheme.substr(0, 37) + "...";
    if(displayTheme.empty()) displayTheme = "Entrer le theme...";
    ofDrawBitmapString("Theme: " + displayTheme, themeBox.x + 5, themeBox.y + 20);

    // Prompt Video 1
    ofFill();
    ofSetColor(bPromptVid1Focused ? 60 : 30);
    ofDrawRectangle(promptVid1Box);
    ofNoFill();
    ofSetColor(bPromptVid1Focused ? ofColor(50, 200, 50) : ofColor(150));
    ofDrawRectangle(promptVid1Box);
    ofSetColor(255);
    string displayPrompt1 = promptVid1Text;
    if(displayPrompt1.length() > 40) displayPrompt1 = displayPrompt1.substr(0, 37) + "...";
    if(displayPrompt1.empty()) displayPrompt1 = "Entrer prompt video 1...";
    ofDrawBitmapString("Prompt V1: " + displayPrompt1, promptVid1Box.x + 5, promptVid1Box.y + 20);

    // Prompt Video 2
    ofFill();
    ofSetColor(bPromptVid2Focused ? 60 : 30);
    ofDrawRectangle(promptVid2Box);
    ofNoFill();
    ofSetColor(bPromptVid2Focused ? ofColor(50, 200, 50) : ofColor(150));
    ofDrawRectangle(promptVid2Box);
    ofSetColor(255);
    string displayPrompt2 = promptVid2Text;
    if(displayPrompt2.length() > 40) displayPrompt2 = displayPrompt2.substr(0, 37) + "...";
    if(displayPrompt2.empty()) displayPrompt2 = "Entrer prompt video 2...";
    ofDrawBitmapString("Prompt V2: " + displayPrompt2, promptVid2Box.x + 5, promptVid2Box.y + 20);

    // Generate Buttons
    auto drawGenBtn = [&](const ofRectangle& rect, string text, ofColor col) {
        ofFill(); ofSetColor(col); ofDrawRectangle(rect);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(rect);
        ofDrawBitmapStringHighlight(text, rect.x + (rect.width/2.0f) - (text.length()*4.0f), rect.y + 20, col, ofColor(255));
    };
    
    drawGenBtn(genRoomBtn, "GENERATE 360 FROM ROOM", ofColor(100, 150, 200));
    drawGenBtn(genVidLastBtn, "GEN VID FROM LAST FRAME", ofColor(100, 180, 150));
    drawGenBtn(genVid2LastBtn, "GEN VID FROM 2 LAST FRAMES", ofColor(120, 180, 120));
    drawGenBtn(genTextToRoomBtn, "GENERATE 360 FROM TEXT", ofColor(150, 100, 200));
    
    ofFill(); ofSetColor(100); ofDrawRectangle(imageSizeBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(imageSizeBtn);
    ofSetColor(255); ofDrawBitmapString("IMG SIZE: " + imageSizeOptions[currentImageSizeIndex], imageSizeBtn.x + 5, imageSizeBtn.y + 20);

    ofFill(); ofSetColor(100); ofDrawRectangle(videoResBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(videoResBtn);
    ofSetColor(255); ofDrawBitmapString("VID RES: " + videoResOptions[currentVideoResIndex], videoResBtn.x + 5, videoResBtn.y + 20);

    // Accordion en dernier pour recouvrir les autres boutons s'il est ouvert
    if (bModelAccordionOpen) ofSetColor(150, 150, 200); else ofSetColor(100);
    ofFill(); ofDrawRectangle(modelAccordionBtn);
    ofSetColor(255);
    ofDrawBitmapString("MODEL: " + modelOptions[currentModelIndex] + (bModelAccordionOpen ? " [-]" : " [+]"), modelAccordionBtn.x + 5, modelAccordionBtn.y + 20);

    if (bModelAccordionOpen) {
        for(size_t i=0; i<modelOptions.size(); i++) {
            if (currentModelIndex == i) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(modelOptionRects[i]);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(modelOptionRects[i]);
            ofSetColor(255);
            ofDrawBitmapString(modelOptions[i], modelOptionRects[i].x + 20, modelOptionRects[i].y + 20);
        }
    }

    ofPopStyle();
    // --- FIN GEMINI UI ---
    
    if (bEditMode) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(255, 200, 0, 255); // Jaune pour les éléments sélectionnés
        ofSetLineWidth(2);
        for(auto* r : selectedRects) {
            ofDrawRectangle(r->x - 2, r->y - 2, r->width + 4, r->height + 4);
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
    ofPopStyle();
    
    // Dessin du tooltip par-dessus tout, non affecte par le Zoom (en coordonnees ecran brutes)
    if(!hoveredTooltip.empty()) {
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
}

void PlaylistVisualizerApp::mousePressed(int x, int y, int button) {
    lastMouse.set(x, y);
    
    if (!bEnabled) return;
    if (isSpacePressed) return; // Bloque le clic sur les boutons lors du déplacement
    
    // Clics dans le HUD Fixe
    if (editBtnRect.inside(x, y)) { 
        bEditMode = !bEditMode; 
        selectedRects.clear(); 
        bIsSelecting = false; 
        bIsDraggingGroup = false; 
        return; 
    }
    if (saveBtnRect.inside(x, y)) { saveButtonPositions(); return; }
    if (loadBtnRect.inside(x, y)) { loadButtonPositions(); return; }

    ofVec2f worldM = getTransformedMouse(x, y);
    ofRectangle* clickedRect = findButtonAt(worldM);
    
    // Raccourci Cmd+Clic (ou Ctrl+Clic) sur le fond pour basculer le mode édition
    bool isCmdPressed = ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_SUPER) || ofGetKeyPressed(OF_KEY_CONTROL);
    if (!clickedRect && isCmdPressed) {
        bEditMode = !bEditMode;
        selectedRects.clear();
        bIsSelecting = false;
        bIsDraggingGroup = false;
        return;
    }

    // Intercepte les interactions quand l'édition est active
    if (bEditMode) {
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

    // --- GEMINI UI CLICKS (Dans l'espace du monde) ---
    if(apiKeyBox.inside(worldM)) {
        bApiKeyFocused = true; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false; return;
    }
    if(themeBox.inside(worldM)) {
        bThemeFocused = true; bApiKeyFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false; return;
    }
    if(promptVid1Box.inside(worldM)) {
        bPromptVid1Focused = true; bApiKeyFocused = false; bThemeFocused = false; bPromptVid2Focused = false; return;
    }
    if(promptVid2Box.inside(worldM)) {
        bPromptVid2Focused = true; bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; return;
    }
    if(genRoomBtn.inside(worldM)) {
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            string prompt = "Transform this room into vector illustration of " + themeText + " (in style of day of the tentacle) , keeping the structure but changing materials and lighting";
            mainAppPtr->geminiGen.generateImage360FromImage(prompt, "export_360_room.png");
            ofLogNotice("PlaylistVisualizerApp") << "Generation 360 lancee via bouton.";
        }
        return;
    }
    if(genVidLastBtn.inside(worldM)) {
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            mainAppPtr->geminiGen.generateVideoFromImage(promptVid1Text, "gen360_last.jpg");
        }
        return;
    }
    if(genVid2LastBtn.inside(worldM)) {
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            mainAppPtr->geminiGen.generateVideoFromDeuxImages(promptVid2Text, "gen360.jpg", "gen360_last.jpg");
        }
        return;
    }
    
    if (bModelAccordionOpen) {
        for(int i=0; i<modelOptionRects.size(); i++) {
            if (modelOptionRects[i].inside(worldM)) {
                currentModelIndex = i;
                bModelAccordionOpen = false;
                return;
            }
        }
    }
    if (modelAccordionBtn.inside(worldM)) {
        bModelAccordionOpen = !bModelAccordionOpen;
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        return;
    }
    if (imageSizeBtn.inside(worldM)) {
        currentImageSizeIndex = (currentImageSizeIndex + 1) % imageSizeOptions.size();
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        return;
    }
    if (videoResBtn.inside(worldM)) {
        currentVideoResIndex = (currentVideoResIndex + 1) % videoResOptions.size();
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        return;
    }
    if (genTextToRoomBtn.inside(worldM)) {
        bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            string prompt = themeText;
            mainAppPtr->geminiGen.generateImage360(prompt);
            ofLogNotice("PlaylistVisualizerApp") << "Generation 360 via texte lancee.";
        }
        return;
    }
    
    // Si clic ailleurs que les UI IA : Perte de focus
    bApiKeyFocused = false;
    bThemeFocused = false;
    bPromptVid1Focused = false;
    bPromptVid2Focused = false;

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

    // Interaction avec les boutons d'action de la room
    for(auto& b : roomActionBtns) {
        if(b.rect.inside(worldM.x, worldM.y)) {
            if (!b.continuous) b.action(); // Les actions continues sont gérées dans update()
            return; 
        }
    }

    // Interaction avec les boutons d'action globale
    for(auto& b : globalActionBtns) {
        if(b.rect.inside(worldM.x, worldM.y)) {
            if (!b.continuous) b.action();
            return; 
        }
    }

    // Interaction avec les boutons de layers (Soumis au pan/zoom)
    for(auto& t : layerToggles) {
        if(t.rect.inside(worldM.x, worldM.y)) {
            *(t.valuePtr) = !(*(t.valuePtr));
            
            if (t.name == "GroPuyo" && !(*(t.valuePtr))) {
                if (scene2D) scene2D->layerManager.groPuyoLayer.puyos.clear();
            }
            
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

    if (!player) {
        if (!bEditMode) bIsDraggingPan = true;
        return;
    }
    
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

    if (doubleSpeedBtnRect.inside(worldM.x, worldM.y)) {
        player->toggleDoubleSpeed();
        return;
    }

    if (muteBtnRect.inside(worldM.x, worldM.y)) {
        player->toggleMute();
        return;
    }

    if (crop106BtnRect.inside(worldM.x, worldM.y)) {
        player->toggleCrop106();
        return;
    }

    if (infinitePauseBtnRect.inside(worldM.x, worldM.y)) {
        player->toggleInfinitePause();
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
            player->planPathToNode(kv.first);
            return;
        }
    }
    
    if (!bEditMode) {
        bIsDraggingPan = true;
    }
}

void PlaylistVisualizerApp::mouseDragged(int x, int y, int button) {
    if(isSpacePressed || bIsDraggingPan) {
        ofVec2f currentMouse(x, y);
        pan += (currentMouse - lastMouse);
        lastMouse = currentMouse;
        return;
    }

    if (bEditMode) {
        ofVec2f worldM = getTransformedMouse(x, y);
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
                if(selRect.intersects(*r)) {
                    selectedRects.push_back(r);
                }
            }
        }
        return;
    }
}

void PlaylistVisualizerApp::mouseReleased(int x, int y, int button) {
    if (bEditMode) {
        bIsDraggingGroup = false;
        bIsSelecting = false;
    }
    bIsDraggingPan = false;
}

void PlaylistVisualizerApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse(x, y);
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    zoom = ofClamp(zoom * zoomFactor, 0.01f, 50.0f);
    pan.x = x - worldM.x * zoom;
    pan.y = y - worldM.y * zoom;
}

void PlaylistVisualizerApp::keyPressed(int key) { 
    // INTERCEPTION DE LA SAISIE CLAVIER POUR GEMINI
    if(bApiKeyFocused || bThemeFocused || bPromptVid1Focused || bPromptVid2Focused) {
        string* targetText = bApiKeyFocused ? &apiKeyText : 
                             bThemeFocused ? &themeText : 
                             bPromptVid1Focused ? &promptVid1Text : &promptVid2Text;

        // Collage (Ctrl+V / Cmd+V)
        if (ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) {
            if (key == 'v' || key == 'V' || key == 22) { // 22 is ASCII for standard Ctrl+V
                *targetText += ofGetWindowPtr()->getClipboardString();
                return;
            }
        }

        if(key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if(!targetText->empty()) targetText->pop_back();
        } else if (key == OF_KEY_RETURN) {
            bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false;
        } else if (key >= 32 && key <= 126) { // Caractères imprimables
            *targetText += (char)key;
        }
        return; // On arrête là pour ne pas déclencher d'autres raccourcis
    }

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

ofRectangle* PlaylistVisualizerApp::findButtonAt(ofVec2f pos) {
    for(int i=0; i<4; i++) if(viewBtns[i].inside(pos)) return &viewBtns[i];
    if(moveV3Btn.inside(pos)) return &moveV3Btn;
    for(int i=0; i<6; i++) {
        if(wxcvbBtns[i].inside(pos)) return &wxcvbBtns[i];
        if(focusBtns[i].inside(pos)) return &focusBtns[i];
    }
    for(int i=0; i<3; i++) {
        if(gabBtns[i].inside(pos)) return &gabBtns[i];
    }
    for(auto& t : roomToggles) if(t.rect.inside(pos)) return &t.rect;
    for(auto& b : roomActionBtns) if(b.rect.inside(pos)) return &b.rect;
    for(auto& b : globalActionBtns) if(b.rect.inside(pos)) return &b.rect;
    for(auto& t : layerToggles) if(t.rect.inside(pos)) return &t.rect;
    for(auto& b : creatureButtons) if(b.rect.inside(pos)) return &b.rect;
    for(auto& b : interactiveButtons) if(b.rect.inside(pos)) return &b.rect;
    if(clearAllCreaturesBtn.inside(pos)) return &clearAllCreaturesBtn;
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
    if(videoInfoBox.inside(pos)) return &videoInfoBox;
    if(infinitePauseBtnRect.inside(pos)) return &infinitePauseBtnRect;
    if(apiKeyBox.inside(pos)) return &apiKeyBox;
    if(themeBox.inside(pos)) return &themeBox;
    if(promptVid1Box.inside(pos)) return &promptVid1Box;
    if(promptVid2Box.inside(pos)) return &promptVid2Box;
    if(genRoomBtn.inside(pos)) return &genRoomBtn;
    if(genVidLastBtn.inside(pos)) return &genVidLastBtn;
    if(genVid2LastBtn.inside(pos)) return &genVid2LastBtn;
    if(bModelAccordionOpen) {
        for(auto& r : modelOptionRects) if(r.inside(pos)) return &r;
    }
    if(modelAccordionBtn.inside(pos)) return &modelAccordionBtn;
    if(imageSizeBtn.inside(pos)) return &imageSizeBtn;
    if(videoResBtn.inside(pos)) return &videoResBtn;
    if(genTextToRoomBtn.inside(pos)) return &genTextToRoomBtn;
    
    return nullptr;
}

void PlaylistVisualizerApp::saveButtonPositions() {
    ofJson pt;
    
    // Sauvegarde du Zoom et du Pan
    pt["view"]["zoom"] = zoom;
    pt["view"]["pan_x"] = pan.x;
    pt["view"]["pan_y"] = pan.y;
    
    // Sauvegarde de la fenêtre (Position et Taille)
    pt["window"]["x"] = ofGetWindowPositionX();
    pt["window"]["y"] = ofGetWindowPositionY();
    pt["window"]["w"] = ofGetWindowWidth();
    pt["window"]["h"] = ofGetWindowHeight();

    // Sauvegarde des champs texte Gemini (Clé API retirée pour sécurité)
    pt["gemini"]["theme"] = themeText;
    pt["gemini"]["promptVid1"] = promptVid1Text;
    pt["gemini"]["promptVid2"] = promptVid2Text;

    auto saveR = [&](const string& key, const ofRectangle& r) {
        pt[key]["x"] = r.x; pt[key]["y"] = r.y;
    };

    for(int i=0; i<4; i++) saveR("view_" + ofToString(i), viewBtns[i]);
    saveR("moveV3", moveV3Btn);
    for(int i=0; i<6; i++) {
        saveR("wxcvb_" + ofToString(i), wxcvbBtns[i]);
        saveR("focus_" + ofToString(i), focusBtns[i]);
    }
    for(int i=0; i<3; i++) saveR("gab_" + ofToString(i), gabBtns[i]);
    for(auto& t : roomToggles) saveR("room_" + t.name, t.rect);
    for(auto& b : roomActionBtns) saveR("action_" + b.name, b.rect);
    for(auto& b : globalActionBtns) saveR("globalAction_" + b.name, b.rect);
    for(auto& t : layerToggles) saveR("layer_" + t.name, t.rect);
    for(auto& b : creatureButtons) saveR("creature_" + b.name, b.rect);
    for(auto& b : interactiveButtons) saveR("interactive_" + b.name, b.rect);
    saveR("clearAll", clearAllCreaturesBtn);
    saveR("loop", loopButtonRect);
    saveR("toggle", toggleButtonRect);
    saveR("sim", simButtonRect);
    saveR("speedX2", doubleSpeedBtnRect);
    saveR("mute", muteBtnRect);
    saveR("crop106", crop106BtnRect);
    saveR("infinitePause", infinitePauseBtnRect);
    saveR("videoInfo", videoInfoBox);
    saveR("pause", pauseAccordionBtn);
    for(int i=0; i<pauseOptionRects.size(); i++) saveR("pauseOpt_" + ofToString(i), pauseOptionRects[i]);
    
    saveR("gemini_apiKeyBox", apiKeyBox);
    saveR("gemini_themeBox", themeBox);
    saveR("gemini_promptVid1Box", promptVid1Box);
    saveR("gemini_promptVid2Box", promptVid2Box);
    saveR("gemini_genRoomBtn", genRoomBtn);
    saveR("gemini_genVidLastBtn", genVidLastBtn);
    saveR("gemini_genVid2LastBtn", genVid2LastBtn);
    saveR("gemini_modelAccordionBtn", modelAccordionBtn);
    for(int i=0; i<modelOptionRects.size(); i++) saveR("gemini_modelOpt_" + ofToString(i), modelOptionRects[i]);
    saveR("gemini_imageSizeBtn", imageSizeBtn);
    saveR("gemini_videoResBtn", videoResBtn);
    saveR("gemini_genTextToRoomBtn", genTextToRoomBtn);

    pt["gemini"]["currentModelIndex"] = currentModelIndex;
    pt["gemini"]["currentImageSizeIndex"] = currentImageSizeIndex;
    pt["gemini"]["currentVideoResIndex"] = currentVideoResIndex;

    ofSaveJson("playlist_btn_positions.json", pt);
    ofLogNotice("PlaylistVisualizerApp") << "Positions des boutons sauvegardees.";
    
    saveFeedbackTimer = ofGetElapsedTimef();
}

void PlaylistVisualizerApp::loadButtonPositions() {
    ofFile file("playlist_btn_positions.json");
    if(!file.exists()) return;

    ofJson pt = ofLoadJson(file.path());
    
    // Chargement du Zoom et du Pan
    if(pt.contains("view")) {
        zoom = pt["view"].value("zoom", zoom);
        pan.x = pt["view"].value("pan_x", pan.x);
        pan.y = pt["view"].value("pan_y", pan.y);
    }
    
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

    // Chargement des champs texte Gemini
    if(pt.contains("gemini")) {
        themeText = pt["gemini"].value("theme", themeText);
        promptVid1Text = pt["gemini"].value("promptVid1", promptVid1Text);
        promptVid2Text = pt["gemini"].value("promptVid2", promptVid2Text);
    }

    auto loadR = [&](const string& key, ofRectangle& r) {
        if(pt.contains(key)) {
            r.x = pt[key].value("x", r.x);
            r.y = pt[key].value("y", r.y);
        }
    };

    for(int i=0; i<4; i++) loadR("view_" + ofToString(i), viewBtns[i]);
    loadR("moveV3", moveV3Btn);
    for(int i=0; i<6; i++) {
        loadR("wxcvb_" + ofToString(i), wxcvbBtns[i]);
        loadR("focus_" + ofToString(i), focusBtns[i]);
    }
    for(int i=0; i<3; i++) loadR("gab_" + ofToString(i), gabBtns[i]);
    for(auto& t : roomToggles) loadR("room_" + t.name, t.rect);
    for(auto& b : roomActionBtns) loadR("action_" + b.name, b.rect);
    for(auto& b : globalActionBtns) loadR("globalAction_" + b.name, b.rect);
    for(auto& t : layerToggles) loadR("layer_" + t.name, t.rect);
    for(auto& b : creatureButtons) loadR("creature_" + b.name, b.rect);
    for(auto& b : interactiveButtons) loadR("interactive_" + b.name, b.rect);
    loadR("clearAll", clearAllCreaturesBtn);
    loadR("loop", loopButtonRect);
    loadR("toggle", toggleButtonRect);
    loadR("sim", simButtonRect);
    loadR("speedX2", doubleSpeedBtnRect);
    loadR("mute", muteBtnRect);
    loadR("crop106", crop106BtnRect);
    loadR("infinitePause", infinitePauseBtnRect);
    loadR("videoInfo", videoInfoBox);
    loadR("pause", pauseAccordionBtn);
    for(int i=0; i<pauseOptionRects.size(); i++) loadR("pauseOpt_" + ofToString(i), pauseOptionRects[i]);
    
    loadR("gemini_apiKeyBox", apiKeyBox);
    loadR("gemini_themeBox", themeBox);
    loadR("gemini_promptVid1Box", promptVid1Box);
    loadR("gemini_promptVid2Box", promptVid2Box);
    loadR("gemini_genRoomBtn", genRoomBtn);
    loadR("gemini_genVidLastBtn", genVidLastBtn);
    loadR("gemini_genVid2LastBtn", genVid2LastBtn);
    loadR("gemini_modelAccordionBtn", modelAccordionBtn);
    for(int i=0; i<modelOptionRects.size(); i++) loadR("gemini_modelOpt_" + ofToString(i), modelOptionRects[i]);
    loadR("gemini_imageSizeBtn", imageSizeBtn);
    loadR("gemini_videoResBtn", videoResBtn);
    loadR("gemini_genTextToRoomBtn", genTextToRoomBtn);
    if (pt.contains("gemini")) {
        currentModelIndex = pt["gemini"].value("currentModelIndex", currentModelIndex);
        currentImageSizeIndex = pt["gemini"].value("currentImageSizeIndex", currentImageSizeIndex);
        currentVideoResIndex = pt["gemini"].value("currentVideoResIndex", currentVideoResIndex);
    }
}

vector<ofRectangle*> PlaylistVisualizerApp::getAllInteractableRects() {
    vector<ofRectangle*> rects;
    for(int i=0; i<4; i++) rects.push_back(&viewBtns[i]);
    rects.push_back(&moveV3Btn);
    for(int i=0; i<6; i++) {
        rects.push_back(&wxcvbBtns[i]);
        rects.push_back(&focusBtns[i]);
    }
    for(int i=0; i<3; i++) rects.push_back(&gabBtns[i]);
    for(auto& t : roomToggles) rects.push_back(&t.rect);
    for(auto& b : roomActionBtns) rects.push_back(&b.rect);
    for(auto& b : globalActionBtns) rects.push_back(&b.rect);
    for(auto& t : layerToggles) rects.push_back(&t.rect);
    for(auto& b : creatureButtons) rects.push_back(&b.rect);
    for(auto& b : interactiveButtons) rects.push_back(&b.rect);
    rects.push_back(&clearAllCreaturesBtn);
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
    rects.push_back(&videoInfoBox);
    rects.push_back(&infinitePauseBtnRect);
    rects.push_back(&apiKeyBox);
    rects.push_back(&themeBox);
    rects.push_back(&promptVid1Box);
    rects.push_back(&promptVid2Box);
    rects.push_back(&genRoomBtn);
    rects.push_back(&genVidLastBtn);
    rects.push_back(&genVid2LastBtn);
    rects.push_back(&modelAccordionBtn);
    if (bModelAccordionOpen) {
        for(auto& r : modelOptionRects) rects.push_back(&r);
    }
    rects.push_back(&imageSizeBtn);
    rects.push_back(&videoResBtn);
    rects.push_back(&genTextToRoomBtn);
    return rects;
}