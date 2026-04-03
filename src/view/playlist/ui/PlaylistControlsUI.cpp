#include "PlaylistControlsUI.h"
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "ofApp.h"
#include "RoomPreview.h"
#include "ButtonApp.h"
#include "ViewApp.h"

void PlaylistControlsUI::setupLayerToggles(Scene2D_SIDE* scene2D) {
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
    addToggle("Avoider", scene2D->layerManager.bDrawAvoider);
    addToggle("Avoider2", scene2D->layerManager.bDrawAvoider2);
    addToggle("Avoider4", scene2D->layerManager.bDrawAvoider4);
    addToggle("Tourelles", scene2D->layerManager.bDrawTourelles);
    addToggle("AutoPong", scene2D->layerManager.bDrawAutoPong);
    addToggle("AutoSnake", scene2D->layerManager.bDrawAutoSnake);
    addToggle("eatMap", scene2D->layerManager.bDrawEatMap);
    addToggle("Crayon", scene2D->layerManager.bDrawCrayon);
    addToggle("SurSauteurs", scene2D->layerManager.bDrawSurSauteurs);
    addToggle("PaperLight", scene2D->layerManager.bDrawPaperLight);
    addToggle("PhysicSam", scene2D->layerManager.bDrawPhysicSam);
    addToggle("Alive", scene2D->layerManager.bDrawAlive);

    int cols = 3;
    float bw = 110;
    float bh = 20;
    float pad = 5;
    float startX = 0;
    float startY = 300;

    for(int i=0; i<layerToggles.size(); i++) {
        int c = i % cols;
        int r = i / cols;
        layerToggles[i].rect.set(startX + c*(bw+pad), startY + r*(bh+pad), bw, bh);
    }
    
    float cStartX = 800; 
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
    
    int cRows = (cNames.size() + cCols - 1) / cCols;
    clearAllCreaturesBtn.set(cStartX, startY + cRows*(bh+pad) + 10, bw*2+pad, bh);
    undoCreatureBtn.set(clearAllCreaturesBtn.getRight() + pad, startY + cRows*(bh+pad) + 10, bw, bh);
    resetEatMapBtn.set(undoCreatureBtn.getRight() + pad, startY + cRows*(bh+pad) + 10, bw, bh);
    resetCollidersBtn.set(resetEatMapBtn.getRight() + pad, startY + cRows*(bh+pad) + 10, bw, bh);

    float iStartY = clearAllCreaturesBtn.y + bh + 20; 
    vector<string> iNames = {"AddGroPuyo", "AddPuyo", "AddBubble", "TargetPoulpe", "Sardine", "Shark", "AddPaperLight", "AddAlive"};
    
    interactiveButtons.clear();
    for(int i=0; i<iNames.size(); i++) {
        int c = i % cCols;
        int r = i / cCols;
        interactiveButtons.push_back({iNames[i], ofRectangle(cStartX + c*(bw+pad), iStartY + r*(bh+pad), bw, bh)});
    }
    
    if(selectedInteractiveIndex >= 0 && selectedInteractiveIndex < interactiveButtons.size()) {
        scene2D->layerManager.selectedInteractiveLayer = interactiveButtons[selectedInteractiveIndex].name;
    }

    if(!creatureButtons.empty()) {
        scene2D->layerManager.selectedCreatureToSpawn = creatureButtons[selectedCreatureIndex].name;
    }
}

void PlaylistControlsUI::setupRoomToggles(RoomApp* roomApp, ofApp* mainAppPtr) {
    if(!roomApp) return;
    roomToggles.clear();

    auto addToggle = [&](string name, std::function<bool()> get, std::function<void()> toggle) {
        roomToggles.push_back({name, ofRectangle(), get, toggle});
    };

    addToggle("Sol 3D", [roomApp](){ return roomApp->bDrawUndulatingFloor; }, [roomApp](){ roomApp->bDrawUndulatingFloor = !roomApp->bDrawUndulatingFloor; });
    addToggle("Tangage", [roomApp](){ return roomApp->bOscillateRoom; }, [roomApp](){ roomApp->bOscillateRoom = !roomApp->bOscillateRoom; });
    addToggle("Kraken In", [roomApp](){ return roomApp->bDrawKraken; }, [roomApp](){ 
        roomApp->bDrawKraken = !roomApp->bDrawKraken; 
        if(roomApp->bDrawKraken) roomApp->kraken.start(roomApp->localTime);
    });
    addToggle("Kraken Ex", [roomApp](){ return roomApp->bDrawExternalKraken; }, [roomApp](){ 
        roomApp->bDrawExternalKraken = !roomApp->bDrawExternalKraken; 
        if(roomApp->bDrawExternalKraken) roomApp->externalKraken.start(roomApp->localTime);
    });
    addToggle("Nuages", [roomApp](){ return roomApp->bDrawCloudRing; }, [roomApp](){ roomApp->bDrawCloudRing = !roomApp->bDrawCloudRing; });
    addToggle("Eau Sphere", [roomApp](){ return roomApp->bDrawLiquidSphere; }, [roomApp](){ roomApp->bDrawLiquidSphere = !roomApp->bDrawLiquidSphere; });
    addToggle("Jelly Sph", [roomApp](){ return roomApp->bDrawJellySphere; }, [roomApp](){ 
        roomApp->bDrawJellySphere = !roomApp->bDrawJellySphere; 
        if(!roomApp->bDrawJellySphere) roomApp->jellySphereRing.clearJellies();
    });
    addToggle("Vaisseaux", [roomApp](){ return roomApp->bDrawColorCop; }, [roomApp](){ 
        roomApp->bDrawColorCop = !roomApp->bDrawColorCop; 
        if(roomApp->bDrawColorCop) roomApp->colorCopRing.resetTexture();
    });
    addToggle("Faisceau", [roomApp](){ return roomApp->bDrawBeam; }, [roomApp](){ roomApp->bDrawBeam = !roomApp->bDrawBeam; });
    addToggle("Prev 360", [roomApp](){ return roomApp->bDrawAtmosphere; }, [roomApp](){ roomApp->bDrawAtmosphere = !roomApp->bDrawAtmosphere; });
    addToggle("Texture ON", [roomApp](){ return roomApp->bUseTexture; }, [roomApp](){ roomApp->bUseTexture = !roomApp->bUseTexture; });
    addToggle("Box Checker", [roomApp](){ return roomApp->bDrawBoxTexture; }, [roomApp](){ roomApp->bDrawBoxTexture = !roomApp->bDrawBoxTexture; });
    addToggle("JupyterBox", [roomApp](){ return roomApp->bDrawJupyterBox; }, [roomApp](){ roomApp->bDrawJupyterBox = !roomApp->bDrawJupyterBox; });
    addToggle("G.O.L Box", [roomApp](){ return roomApp->bDrawGolBox; }, [roomApp](){ roomApp->bDrawGolBox = !roomApp->bDrawGolBox; });
    addToggle("GOL Motion", [roomApp](){ return roomApp->bDrawGolBoxMotion; }, [roomApp](){ roomApp->bDrawGolBoxMotion = !roomApp->bDrawGolBoxMotion; });
    addToggle("AutoSnake Box", [roomApp](){ return roomApp->bDrawAutoSnakeBox; }, [roomApp](){ roomApp->bDrawAutoSnakeBox = !roomApp->bDrawAutoSnakeBox; });
    addToggle("Snake Interact", [roomApp](){ return roomApp->bInteractAutoSnake; }, [roomApp](){ roomApp->bInteractAutoSnake = !roomApp->bInteractAutoSnake; });
    addToggle("Plafond", [roomApp](){ return roomApp->bShowRoof; }, [roomApp](){ roomApp->bShowRoof = !roomApp->bShowRoof; });
    addToggle("Respiration", [roomApp](){ return roomApp->respire; }, [roomApp](){ roomApp->respire = !roomApp->respire; });
    addToggle("Vagues", [roomApp](){ return roomApp->bDrawRipples; }, [roomApp](){ roomApp->bDrawRipples = !roomApp->bDrawRipples; });
    addToggle("Vers Murs", [roomApp](){ return roomApp->bDrawWorms; }, [roomApp](){ roomApp->bDrawWorms = !roomApp->bDrawWorms; });
    addToggle("Vers Vol", [roomApp](){ return roomApp->bDrawWingedWorms; }, [roomApp](){ roomApp->bDrawWingedWorms = !roomApp->bDrawWingedWorms; });
    addToggle("Portail", [roomApp](){ return roomApp->bFluidRingEnabled; }, [roomApp](){ 
        roomApp->bFluidRingEnabled = !roomApp->bFluidRingEnabled; 
        roomApp->fluidRing.setTargetAlpha(roomApp->bFluidRingEnabled ? 1.0f : 0.0f);
    });
    addToggle("Lucioles", [roomApp](){ return roomApp->bLightFlyRingEnabled; }, [roomApp](){ roomApp->bLightFlyRingEnabled = !roomApp->bLightFlyRingEnabled; });
    addToggle("Cur Alpha", [roomApp](){ return roomApp->cursorSquare.bLowAlpha; }, [roomApp](){ roomApp->cursorSquare.bLowAlpha = !roomApp->cursorSquare.bLowAlpha; });
    addToggle("Gen 360", [](){ return false; }, [roomApp](){ roomApp->generateEquirectangularImage(); });
    addToggle("Gen 360 F", [](){ return false; }, [roomApp](){ roomApp->generateFull360EquirectangularImage(); });
    addToggle("Gen 360 W", [](){ return false; }, [roomApp](){ roomApp->generate360FullW(); });
    addToggle("Beams", [roomApp](){ return roomApp->projection.getShowBeams(); }, [roomApp](){ roomApp->projection.keyPressed('t'); });
    addToggle("Plan Colle", [roomApp](){ return roomApp->projection.getShowPlanColle(); }, [roomApp](){ roomApp->projection.keyPressed('n'); });
    addToggle("Cur Reflet", [roomApp](){ return roomApp->cursorSquare.bDrawReflections; }, [roomApp](){ roomApp->cursorSquare.bDrawReflections = !roomApp->cursorSquare.bDrawReflections; });
    addToggle("Prev Inter", [mainAppPtr](){ return mainAppPtr && mainAppPtr->roomPreviewApp ? mainAppPtr->roomPreviewApp->bDrawInteraction : false; }, [mainAppPtr](){ if(mainAppPtr && mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->bDrawInteraction = !mainAppPtr->roomPreviewApp->bDrawInteraction; });
    addToggle("Prev Curs", [mainAppPtr](){ return mainAppPtr && mainAppPtr->roomPreviewApp ? mainAppPtr->roomPreviewApp->bShowCursor : false; }, [mainAppPtr](){ if(mainAppPtr && mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->bShowCursor = !mainAppPtr->roomPreviewApp->bShowCursor; });
    addToggle("Tuyau 3D", [roomApp](){ return roomApp->bDrawTuyau; }, [roomApp](){ roomApp->bDrawTuyau = !roomApp->bDrawTuyau; });
    addToggle("Tuyau Obj", [roomApp](){ return roomApp->tuyau.bDrawTuyauObj; }, [roomApp](){ roomApp->tuyau.bDrawTuyauObj = !roomApp->tuyau.bDrawTuyauObj; });
    addToggle("Tuyau Arcs", [roomApp](){ return roomApp->tuyau.bDrawArcs; }, [roomApp](){ roomApp->tuyau.setDrawArcs(!roomApp->tuyau.bDrawArcs); });
    addToggle("Couture Arcs", [roomApp](){ return roomApp->tuyau.bDrawCouture; }, [roomApp](){ roomApp->tuyau.bDrawCouture = !roomApp->tuyau.bDrawCouture; });

    int cols = 2;
    float bw = 100;
    float bh = 20;
    float pad = 5;
    float startX = -800;
    float startY = 100;

    for(int i=0; i<roomToggles.size(); i++) {
        int c = i % cols;
        int r = i / cols;
        roomToggles[i].rect.set(startX + c*(bw+pad), startY + r*(bh+pad), bw, bh);
    }
}

void PlaylistControlsUI::setupRoomActionBtns(RoomApp* roomApp) {
    if(!roomApp) return;
    roomActionBtns.clear();

    auto addAction = [&](string name, std::function<void()> action, bool continuous = false, std::function<bool()> getState = nullptr) {
        roomActionBtns.push_back({name, ofRectangle(), action, continuous, getState});
    };

    addAction("Reset Cam [R]", [roomApp](){ roomApp->keyPressed('r'); }, false);
    addAction("Lock Cam [L]", [roomApp](){ 
        roomApp->bLockCameraCenter = !roomApp->bLockCameraCenter; 
        if (!roomApp->bLockCameraCenter) {
            roomApp->camGlobal.setDistance(4000);
            roomApp->camGlobal.setPosition(2000, 2500, 3000);
            roomApp->camGlobal.lookAt(ofVec3f(0, 600, 0));
        }
    }, false, [roomApp](){ return roomApp->bLockCameraCenter; });
    addAction("Sphere [P]", [roomApp, this](){ roomApp->atmosphere.keyPressed('p'); uiStateSphereP = !uiStateSphereP; }, false, [this](){ return uiStateSphereP; });
    addAction("Disco [M]", [roomApp, this](){ roomApp->atmosphere.keyPressed('m'); uiStateDiscoM = !uiStateDiscoM; }, false, [this](){ return uiStateDiscoM; });
    addAction("Sph. 360 [O]", [roomApp](){ roomApp->atmosphere.keyPressed('o'); }, false, [roomApp](){ return roomApp->atmosphere.bShow360; });
    addAction("Auto Rot [I]", [roomApp](){ roomApp->atmosphere.keyPressed('i'); }, false);
    
    addAction("RotY+ [->]", [roomApp](){ roomApp->atmosphere.keyPressed(OF_KEY_RIGHT); }, true);
    addAction("RotY- [<-]", [roomApp](){ roomApp->atmosphere.keyPressed(OF_KEY_LEFT); }, true);
    addAction("RotX+ [v]", [roomApp](){ roomApp->atmosphere.keyPressed(OF_KEY_DOWN); }, true);
    addAction("RotX- [^]", [roomApp](){ roomApp->atmosphere.keyPressed(OF_KEY_UP); }, true);
    addAction("RotZ+ [2]", [roomApp](){ roomApp->atmosphere.keyPressed('2'); }, true);
    addAction("RotZ- [1]", [roomApp](){ roomApp->atmosphere.keyPressed('1'); }, true);
    addAction("OffY+ [3]", [roomApp](){ roomApp->atmosphere.keyPressed('3'); }, true);
    addAction("OffY- [4]", [roomApp](){ roomApp->atmosphere.keyPressed('4'); }, true);
    addAction("Beam +", [roomApp](){ roomApp->projection.keyPressed('+'); }, true);
    addAction("Beam -", [roomApp](){ roomApp->projection.keyPressed('-'); }, true);
    
    addAction("Beam Pan L", [roomApp](){ roomApp->projection.keyPressed(OF_KEY_LEFT); }, true);
    addAction("Beam Pan R", [roomApp](){ roomApp->projection.keyPressed(OF_KEY_RIGHT); }, true);
    addAction("Beam Tilt U", [roomApp](){ roomApp->projection.keyPressed(OF_KEY_UP); }, true);
    addAction("Beam Tilt D", [roomApp](){ roomApp->projection.keyPressed(OF_KEY_DOWN); }, true);
    addAction("Beam Roll W", [roomApp](){ roomApp->projection.keyPressed('w'); }, true);
    addAction("Beam Roll X", [roomApp](){ roomApp->projection.keyPressed('x'); }, true);
    addAction("Plan Ang+ [Q]", [roomApp](){ roomApp->projection.movePlanColle(1.0f, 0.0f); }, true);
    addAction("Plan Ang- [D]", [roomApp](){ roomApp->projection.movePlanColle(-1.0f, 0.0f); }, true);
    addAction("Plan Ele+ [Z]", [roomApp](){ roomApp->projection.movePlanColle(0.0f, 1.0f); }, true);
    addAction("Plan Ele- [S]", [roomApp](){ roomApp->projection.movePlanColle(0.0f, -1.0f); }, true);
    addAction("Del Flys [X]", [roomApp](){ roomApp->lightFlyRing.clearLights(); }, false);
    
    addAction("Jupy Reset", [roomApp](){ roomApp->jupyterBox.reset(); }, false);
    addAction("Jupy Pause", [roomApp](){ roomApp->jupyterBox.bPaused = !roomApp->jupyterBox.bPaused; }, false, [roomApp](){ return roomApp->jupyterBox.bPaused; });
    addAction("G.O.L Reset", [roomApp](){ roomApp->golBox.reset(); }, false);
    addAction("G.O.L New Seed", [roomApp, this](){ 
        roomApp->golBox.currentSeed = ofRandom(10000000); 
        this->golSeedString = ofToString(roomApp->golBox.currentSeed);
        roomApp->golBox.reset(); 
    }, false);
    addAction("GOLM Reset", [roomApp](){ roomApp->golBoxMotion.reset(); }, false);
    addAction("GOLM Pause", [roomApp](){ roomApp->golBoxMotion.bPaused = !roomApp->golBoxMotion.bPaused; }, false, [roomApp](){ return roomApp->golBoxMotion.bPaused; });
    addAction("GOLM New Seed", [roomApp, this](){ 
        roomApp->golBoxMotion.currentSeed = ofRandom(10000000); 
        this->golmSeedString = ofToString(roomApp->golBoxMotion.currentSeed);
        roomApp->golBoxMotion.reset(); 
    }, false);
    addAction("AutoS Reset", [roomApp](){ roomApp->autoSnakeBox.reset(); }, false);
    addAction("AutoS Pause", [roomApp](){ roomApp->autoSnakeBox.bPaused = !roomApp->autoSnakeBox.bPaused; }, false, [roomApp](){ return roomApp->autoSnakeBox.bPaused; });
    
    addAction("Play Tuyau", [roomApp](){ roomApp->tuyau.bIsPlaying = !roomApp->tuyau.bIsPlaying; }, false, [roomApp](){ return roomApp->tuyau.bIsPlaying; });
    addAction("Pause Carref", [roomApp](){ roomApp->tuyau.bPauseAtJunction = !roomApp->tuyau.bPauseAtJunction; }, false, [roomApp](){ return roomApp->tuyau.bPauseAtJunction; });
    addAction("Next Carref", [roomApp](){ roomApp->tuyau.nextCarrefour(); }, false);
    addAction("Reset Tuyau", [roomApp](){ roomApp->tuyau.reset(); }, false);

    int cols = 2;
    float bw = 100;
    float bh = 20;
    float pad = 5;
    float startX = -800;
    float startY = 100 + ((roomToggles.size() + cols - 1) / cols) * (bh + pad) + 20; 

    for(int i=0; i<roomActionBtns.size(); i++) {
        int c = i % cols;
        int r = i / cols;
        roomActionBtns[i].rect.set(startX + c*(bw+pad), startY + r*(bh+pad), bw, bh);
    }
    
    int numRows = (roomActionBtns.size() + cols - 1) / cols;
    golSeedBox.set(startX, startY + numRows*(bh+pad), bw*2+pad, bh);
    golmSeedBox.set(startX, startY + (numRows+1)*(bh+pad), bw*2+pad, bh);
    
    // Slider vitesse tuyau juste en dessous
    tuyauSpeedSlider.set(startX, startY + (numRows+2)*(bh+pad), bw*2+pad, bh);
    tuyauRotXSlider.set(startX, startY + (numRows+3)*(bh+pad), bw*2+pad, bh);
    tuyauRotYSlider.set(startX, startY + (numRows+4)*(bh+pad), bw*2+pad, bh);
    tuyauRotZSlider.set(startX, startY + (numRows+5)*(bh+pad), bw*2+pad, bh);
    
    tuyauPathBox.set(startX, startY + (numRows+6)*(bh+pad), bw*2+pad, bh);
    
    if (roomApp) {
        golSeedString = ofToString(roomApp->golBox.currentSeed);
        golmSeedString = ofToString(roomApp->golBoxMotion.currentSeed);
    }
}

void PlaylistControlsUI::setupGlobalActionBtns(ofApp* mainAppPtr) {
    globalActionBtns.clear();

    auto addAction = [&](string name, std::function<void()> action, bool continuous = false, std::function<bool()> getState = nullptr) {
        globalActionBtns.push_back({name, ofRectangle(), action, continuous, getState});
    };

    addAction("PAUSE [ESC]", [mainAppPtr](){ 
        if(mainAppPtr) {
            mainAppPtr->bGlobalPause = !mainAppPtr->bGlobalPause;
            if(mainAppPtr->bGlobalPause) mainAppPtr->oscTime = mainAppPtr->localTime;
        }
    }, false);

    addAction("SAVE MASTER", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->canvasManager.canvas.isAllocated()) {
            ofPixels pix;
            mainAppPtr->canvasManager.canvas.readToPixels(pix);
            ofSaveImage(pix, "export/gab_frame_" + ofGetTimestampString() + ".png");
            ofLogNotice("PlaylistControlsUI") << "Main Canvas Frame saved.";
        }
    }, false);

    addAction("UNDO CREA [Z]", [mainAppPtr](){ 
        if(mainAppPtr) {
            mainAppPtr->creatureSystem.removeLast();
            ofLogNotice("PlaylistControlsUI") << "Derniere creature retiree.";
        }
    }, false);

    addAction("CLEAR CREA", [mainAppPtr](){ 
        if(mainAppPtr) {
            mainAppPtr->creatureSystem.clear();
            ofLogNotice("PlaylistControlsUI") << "Toutes les creatures du Main Canvas retirees.";
        }
    }, false);

    addAction("REC PREVIEW", [mainAppPtr, this](){ 
        if(mainAppPtr && mainAppPtr->roomPreviewApp) {
            mainAppPtr->roomPreviewApp->keyPressed(OF_KEY_RETURN);
            uiStateRecPreview = !uiStateRecPreview;
            ofLogNotice("PlaylistControlsUI") << "Enregistrement Preview bascule.";
        }
    }, false, [this](){ return uiStateRecPreview; });

    addAction("REC MASTER", [mainAppPtr, this](){ 
        if(mainAppPtr) {
            mainAppPtr->bRecordCanvas = !mainAppPtr->bRecordCanvas;
            if(mainAppPtr->bRecordCanvas) {
                mainAppPtr->canvasRecordFolder = "export/canvas_" + ofGetTimestampString();
                ofDirectory dir(mainAppPtr->canvasRecordFolder);
                dir.create(true);
                ofLogNotice("PlaylistControlsUI") << "Enregistrement du Canvas Master ON : " << mainAppPtr->canvasRecordFolder;
            } else {
                ofLogNotice("PlaylistControlsUI") << "Enregistrement du Canvas Master OFF.";
            }
            uiStateRecCanvas = mainAppPtr->bRecordCanvas;
        }
    }, false, [this](){ return uiStateRecCanvas; });

    addAction("BLUR VIEWS", [mainAppPtr](){ 
        if(mainAppPtr) {
            bool targetState = true;
            for(auto& vApp : mainAppPtr->viewApps) {
                if(vApp) { targetState = !vApp->bBlur; break; }
            }
            for(auto& vApp : mainAppPtr->viewApps) {
                if(vApp) vApp->bBlur = targetState;
            }
            ofLogNotice("PlaylistControlsUI") << "Flou global bascule a : " << (targetState ? "ON" : "OFF");
        }
    }, false);

    addAction("EXP 2D FULL", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportFullScene();
    }, false);

    addAction("EXP 2D COL", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportColliders();
    }, false);

    addAction("GEN COL IMG", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->generateColliderFromOverlay();
    }, false);

    addAction("GEN COL IA", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->generateColliderFromAI();
    }, false);

    addAction("GEN COL SAM", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->generateColliderFromSAM();
    }, false);

    addAction("GEN COL DEX", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->generateColliderFromDexined();
    }, false);

    addAction("GEN DEPTH", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->generateColliderFromDepthAnything();
    }, false);

    addAction("EXP EATMAP", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportEatMap();
    }, false);

    addAction("GAB 3-0-3", [mainAppPtr](){ 
        if(mainAppPtr) {
            mainAppPtr->gabMode = 3;
            if(mainAppPtr->roomApp) { mainAppPtr->roomApp->wallAlpha = 0.0f; mainAppPtr->roomApp->bgMode = 1; }
            if(mainAppPtr->scene2D) mainAppPtr->scene2D->bgDisplayMode = 3;
        }
    }, false);

    addAction("BTN WORMS", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->buttonApp) {
            mainAppPtr->buttonApp->buttonWindow.toggleWorms();
        }
    }, false);

    addAction("EXP 7 MURS", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->export7Murs();
    }, false);

    addAction("SAM CONTROL", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->toggleSamControl();
    }, false);

    for(int i=0; i<globalActionBtns.size(); i++) {
        int c = i % 2;
        int r = i / 2;
        globalActionBtns[i].rect.set(-300 + c * 110, -250 + r * 35, 105, 30);
    }
    
    // Pinceaux du Main Canvas (positions initiales proches du GAB/Clear)
    vector<string> mbNames = {"MainCrea", "MainRand"};
    mainBrushButtons.clear();
    for(int i=0; i<mbNames.size(); i++) {
        mainBrushButtons.push_back({mbNames[i], ofRectangle(-300, -250 + ((globalActionBtns.size() + 1) / 2) * 35 + 20 + i * 25, 100, 20)});
    }
}

void PlaylistControlsUI::clearAllCreatures(Scene2D_SIDE* scene2D) {
    if(!scene2D) return;
    auto& mgr = scene2D->layerManager;
    mgr.creatureSystem.clear();
    mgr.cousinCons.clear();
    mgr.halos.clear();
    mgr.groPuyoLayer.puyos.clear();
    mgr.paperLightLayer.lights.clear();
}

void PlaylistControlsUI::draw(ofApp* mainAppPtr) {
    ofPushStyle();
    
    // Toggles Room
    for(auto& t : roomToggles) {
        if(t.getState()) ofSetColor(0, 150, 200); else ofSetColor(64, 94, 104);
        ofFill(); ofDrawRectangle(t.rect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(t.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(t.rect.x, t.rect.y); ofScale(t.rect.height / 20.0f, t.rect.height / 20.0f);
        ofDrawBitmapString(t.name, 5, 14);
        ofPopMatrix();
    }
    
    if(mainAppPtr && mainAppPtr->roomApp) {
        if (bEditingGolSeed) ofSetColor(200, 200, 50);
        else ofSetColor(64, 94, 104);
        ofFill(); ofDrawRectangle(golSeedBox);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(golSeedBox);
        
        ofRectangle pasteGolBtn(golSeedBox.getRight() - 60, golSeedBox.y, 30, golSeedBox.height);
        ofSetColor(100, 200, 150);
        ofFill(); ofDrawRectangle(pasteGolBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(pasteGolBtn);
        ofPushMatrix(); ofTranslate(pasteGolBtn.x, pasteGolBtn.y); ofScale(pasteGolBtn.height / 20.0f, pasteGolBtn.height / 20.0f);
        ofDrawBitmapString("V", 10, 14);
        ofPopMatrix();

        ofRectangle copyGolBtn(golSeedBox.getRight() - 30, golSeedBox.y, 30, golSeedBox.height);
        ofSetColor(100, 150, 200);
        ofFill(); ofDrawRectangle(copyGolBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(copyGolBtn);
        ofPushMatrix(); ofTranslate(copyGolBtn.x, copyGolBtn.y); ofScale(copyGolBtn.height / 20.0f, copyGolBtn.height / 20.0f);
        ofDrawBitmapString("C", 10, 14);
        ofPopMatrix();

        ofSetColor(255);
        string displayStrGol = "GOL SD: " + golSeedString + (bEditingGolSeed && (ofGetFrameNum() % 60 < 30) ? "_" : "");
        ofPushMatrix(); ofTranslate(golSeedBox.x, golSeedBox.y); ofScale(golSeedBox.height / 20.0f, golSeedBox.height / 20.0f);
        ofDrawBitmapString(displayStrGol, 5, 14);
        ofPopMatrix();

        if (bEditingGolmSeed) ofSetColor(200, 200, 50);
        else ofSetColor(64, 94, 104);
        ofFill(); ofDrawRectangle(golmSeedBox);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(golmSeedBox);
        
        ofRectangle pasteGolmBtn(golmSeedBox.getRight() - 60, golmSeedBox.y, 30, golmSeedBox.height);
        ofSetColor(100, 200, 150);
        ofFill(); ofDrawRectangle(pasteGolmBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(pasteGolmBtn);
        ofPushMatrix(); ofTranslate(pasteGolmBtn.x, pasteGolmBtn.y); ofScale(pasteGolmBtn.height / 20.0f, pasteGolmBtn.height / 20.0f);
        ofDrawBitmapString("V", 10, 14);
        ofPopMatrix();

        ofRectangle copyGolmBtn(golmSeedBox.getRight() - 30, golmSeedBox.y, 30, golmSeedBox.height);
        ofSetColor(100, 150, 200);
        ofFill(); ofDrawRectangle(copyGolmBtn);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(copyGolmBtn);
        ofPushMatrix(); ofTranslate(copyGolmBtn.x, copyGolmBtn.y); ofScale(copyGolmBtn.height / 20.0f, copyGolmBtn.height / 20.0f);
        ofDrawBitmapString("C", 10, 14);
        ofPopMatrix();

        ofSetColor(255);
        string displayStrGolm = "GOLM SD: " + golmSeedString + (bEditingGolmSeed && (ofGetFrameNum() % 60 < 30) ? "_" : "");
        ofPushMatrix(); ofTranslate(golmSeedBox.x, golmSeedBox.y); ofScale(golmSeedBox.height / 20.0f, golmSeedBox.height / 20.0f);
        ofDrawBitmapString(displayStrGolm, 5, 14);
        ofPopMatrix();
        
        // Dessin du Slider Tuyau
        ofSetColor(64, 94, 104);
        ofFill(); ofDrawRectangle(tuyauSpeedSlider);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(tuyauSpeedSlider);
        
        float handleX = tuyauSpeedSlider.x + (mainAppPtr->roomApp->tuyau.speed / 5.0f) * tuyauSpeedSlider.width; // Max speed 5x
        ofSetColor(200, 200, 50);
        ofFill();
        ofDrawRectangle(ofClamp(handleX - 5, tuyauSpeedSlider.x, tuyauSpeedSlider.getRight() - 10), tuyauSpeedSlider.y, 10, tuyauSpeedSlider.height);
        
        ofSetColor(255);
        ofPushMatrix(); ofTranslate(tuyauSpeedSlider.x, tuyauSpeedSlider.y); ofScale(tuyauSpeedSlider.height / 20.0f, tuyauSpeedSlider.height / 20.0f);
        ofDrawBitmapString("VITESSE TUYAU", 5, 14);
        ofPopMatrix();
        
        // Dessin Rot X
        ofSetColor(64, 94, 104); ofFill(); ofDrawRectangle(tuyauRotXSlider);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(tuyauRotXSlider);
        float handleRx = tuyauRotXSlider.x + (mainAppPtr->roomApp->tuyau.rotX / 360.0f) * tuyauRotXSlider.width;
        ofSetColor(200, 100, 50); ofFill();
        ofDrawRectangle(ofClamp(handleRx - 5, tuyauRotXSlider.x, tuyauRotXSlider.getRight() - 10), tuyauRotXSlider.y, 10, tuyauRotXSlider.height);
        ofSetColor(255);
        ofPushMatrix(); ofTranslate(tuyauRotXSlider.x, tuyauRotXSlider.y); ofScale(tuyauRotXSlider.height / 20.0f, tuyauRotXSlider.height / 20.0f);
        ofDrawBitmapString("ROT X TUYAU", 5, 14);
        ofPopMatrix();

        // Dessin Rot Y
        ofSetColor(64, 94, 104); ofFill(); ofDrawRectangle(tuyauRotYSlider);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(tuyauRotYSlider);
        float handleRy = tuyauRotYSlider.x + (mainAppPtr->roomApp->tuyau.rotY / 360.0f) * tuyauRotYSlider.width;
        ofSetColor(50, 200, 100); ofFill();
        ofDrawRectangle(ofClamp(handleRy - 5, tuyauRotYSlider.x, tuyauRotYSlider.getRight() - 10), tuyauRotYSlider.y, 10, tuyauRotYSlider.height);
        ofSetColor(255);
        ofPushMatrix(); ofTranslate(tuyauRotYSlider.x, tuyauRotYSlider.y); ofScale(tuyauRotYSlider.height / 20.0f, tuyauRotYSlider.height / 20.0f);
        ofDrawBitmapString("ROT Y TUYAU", 5, 14);
        ofPopMatrix();

        // Dessin Rot Z
        ofSetColor(64, 94, 104); ofFill(); ofDrawRectangle(tuyauRotZSlider);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(tuyauRotZSlider);
        float handleRz = tuyauRotZSlider.x + (mainAppPtr->roomApp->tuyau.rotZ / 360.0f) * tuyauRotZSlider.width;
        ofSetColor(50, 100, 200); ofFill();
        ofDrawRectangle(ofClamp(handleRz - 5, tuyauRotZSlider.x, tuyauRotZSlider.getRight() - 10), tuyauRotZSlider.y, 10, tuyauRotZSlider.height);
        ofSetColor(255);
        ofPushMatrix(); ofTranslate(tuyauRotZSlider.x, tuyauRotZSlider.y); ofScale(tuyauRotZSlider.height / 20.0f, tuyauRotZSlider.height / 20.0f);
        ofDrawBitmapString("ROT Z TUYAU", 5, 14);
        ofPopMatrix();
        
        // Dessin Path Box
        auto& tuyau = mainAppPtr->roomApp->tuyau;
        if (bTuyauPathBoxOpen) ofSetColor(150, 150, 200); else ofSetColor(64, 94, 104);
        ofFill(); ofDrawRectangle(tuyauPathBox);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(tuyauPathBox);
        
        vector<string> paths = tuyau.getAvailablePaths();
        string currentChoice = (tuyau.selectedPathOption < paths.size()) ? paths[tuyau.selectedPathOption] : "---";
        
        ofSetColor(255);
        ofPushMatrix(); ofTranslate(tuyauPathBox.x, tuyauPathBox.y); ofScale(tuyauPathBox.height / 20.0f, tuyauPathBox.height / 20.0f);
        ofDrawBitmapString("VOIE: " + currentChoice + (bTuyauPathBoxOpen ? " [-]" : " [+]"), 5, 14);
        ofPopMatrix();
        
        if (bTuyauPathBoxOpen) {
            tuyauPathOptionsRects.clear();
            for(size_t j=0; j<paths.size(); j++) {
                ofRectangle optRect(tuyauPathBox.x, tuyauPathBox.getBottom() + j * tuyauPathBox.height, tuyauPathBox.width, tuyauPathBox.height);
                tuyauPathOptionsRects.push_back(optRect);
                
                if (tuyau.selectedPathOption == j) ofSetColor(200, 200, 50); else ofSetColor(80);
                ofFill(); ofDrawRectangle(optRect);
                ofNoFill(); ofSetColor(200); ofDrawRectangle(optRect);
                
                ofSetColor(255);
                ofPushMatrix(); ofTranslate(optRect.x, optRect.y); ofScale(optRect.height / 20.0f, optRect.height / 20.0f);
                ofDrawBitmapString(paths[j], 10, 14);
                ofPopMatrix();
            }
        }
    }

    // Action Room
    for(auto& b : roomActionBtns) {
        if (b.getState && b.getState()) ofSetColor(0, 150, 200); else ofSetColor(64, 94, 104);
        ofFill(); ofDrawRectangle(b.rect);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(b.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(b.rect.x, b.rect.y); ofScale(b.rect.height / 20.0f, b.rect.height / 20.0f);
        ofDrawBitmapString(b.name, 5, 14);
        ofPopMatrix();
    }

    // Action Globale
    for(auto& b : globalActionBtns) {
        if (b.name == "SAM CONTROL" && mainAppPtr && mainAppPtr->scene2D && mainAppPtr->scene2D->samController.isActive()) ofSetColor(100, 180, 100);
        else if (b.name == "PAUSE [ESC]" && mainAppPtr && mainAppPtr->bGlobalPause) ofSetColor(200, 50, 50); 
        else if (b.name == "BTN WORMS" && mainAppPtr && mainAppPtr->buttonApp && mainAppPtr->buttonApp->buttonWindow.bDrawWorms) ofSetColor(50, 200, 50); 
        else if ((b.name == "REC PREVIEW" || b.name == "REC MASTER") && b.getState && b.getState()) {
            // Clignotement rouge pendant l'enregistrement
            if (sin(ofGetElapsedTimef() * 10.0f) > 0) ofSetColor(255, 50, 50); else ofSetColor(150, 0, 0);
        }
        else ofSetColor(60, 60, 80);
        ofFill(); ofDrawRectangle(b.rect);
        ofNoFill(); ofSetColor(150); ofDrawRectangle(b.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(b.rect.x, b.rect.y); ofScale(b.rect.height / 30.0f, b.rect.height / 30.0f);
        ofDrawBitmapString(b.name, 5, 20);
        ofPopMatrix();
    }

    // Toggles Scene 2D
    for(auto& t : layerToggles) {
        if(*(t.valuePtr)) ofSetColor(0, 200, 100); else ofSetColor(64, 104, 84);
        ofFill(); ofDrawRectangle(t.rect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(t.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(t.rect.x, t.rect.y); ofScale(t.rect.height / 20.0f, t.rect.height / 20.0f);
        
        string displayName = t.name;
        if(t.name == "Beams" && mainAppPtr && mainAppPtr->roomApp) {
            int mode = mainAppPtr->roomApp->projection.getBeamAlphaMode();
            if(mode == 0) displayName = "Beams 100%";
            else if(mode == 1) displayName = "Beams 75%";
            else if(mode == 2) displayName = "Beams 33%";
            else displayName = "Beams OFF";
        }
        
        ofDrawBitmapString(displayName, 5, 14);
        ofPopMatrix();
    }

    // Creatures
    for(int i=0; i<creatureButtons.size(); i++) {
        auto& b = creatureButtons[i];
        if(i == selectedCreatureIndex) ofSetColor(200, 200, 0); else ofSetColor(104, 104, 64);
        ofFill(); ofDrawRectangle(b.rect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(b.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(b.rect.x, b.rect.y); ofScale(b.rect.height / 20.0f, b.rect.height / 20.0f);
        ofDrawBitmapString(b.name, 5, 14);
        ofPopMatrix();
    }

    ofSetColor(200, 50, 50);
    ofFill(); ofDrawRectangle(clearAllCreaturesBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(clearAllCreaturesBtn);
    ofPushMatrix(); ofTranslate(clearAllCreaturesBtn.x, clearAllCreaturesBtn.y); ofScale(clearAllCreaturesBtn.height / 20.0f, clearAllCreaturesBtn.height / 20.0f);
    ofDrawBitmapString("CLEAR ALL CREATURES", 5, 14);
    ofPopMatrix();

    ofSetColor(200, 150, 50);
    ofFill(); ofDrawRectangle(undoCreatureBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(undoCreatureBtn);
    ofPushMatrix(); ofTranslate(undoCreatureBtn.x, undoCreatureBtn.y); ofScale(undoCreatureBtn.height / 20.0f, undoCreatureBtn.height / 20.0f);
    ofDrawBitmapString("UNDO CREA [Z]", 5, 14);
    ofPopMatrix();

    ofSetColor(50, 150, 200);
    ofFill(); ofDrawRectangle(resetEatMapBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(resetEatMapBtn);
    ofPushMatrix(); ofTranslate(resetEatMapBtn.x, resetEatMapBtn.y); ofScale(resetEatMapBtn.height / 20.0f, resetEatMapBtn.height / 20.0f);
    ofDrawBitmapString("RESET EATMAP", 5, 14);
    ofPopMatrix();

    ofSetColor(150, 50, 200);
    ofFill(); ofDrawRectangle(resetCollidersBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(resetCollidersBtn);
    ofPushMatrix(); ofTranslate(resetCollidersBtn.x, resetCollidersBtn.y); ofScale(resetCollidersBtn.height / 20.0f, resetCollidersBtn.height / 20.0f);
    ofDrawBitmapString("RESET COLLIDER", 5, 14);
    ofPopMatrix();

    // Interactives
    ofSetColor(255);
    ofDrawBitmapString("INTERACTIVE LAYERS (Touche 'A')", clearAllCreaturesBtn.x, clearAllCreaturesBtn.y + 40);
    for(int i=0; i<interactiveButtons.size(); i++) {
        auto& b = interactiveButtons[i];
        if(i == selectedInteractiveIndex) ofSetColor(200, 100, 200); else ofSetColor(104, 84, 104);
        ofFill(); ofDrawRectangle(b.rect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(b.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(b.rect.x, b.rect.y); ofScale(b.rect.height / 20.0f, b.rect.height / 20.0f);
        ofDrawBitmapString(b.name, 5, 14);
        ofPopMatrix();
    }
    
    // Pinceaux Main Canvas
    for(int i=0; i<mainBrushButtons.size(); i++) {
        auto& b = mainBrushButtons[i];
        if(i == selectedMainBrushIndex) ofSetColor(200, 100, 200); else ofSetColor(104, 84, 104);
        ofFill(); ofDrawRectangle(b.rect);
        ofNoFill(); ofSetColor(200); ofDrawRectangle(b.rect);
        ofSetColor(255); 
        ofPushMatrix(); ofTranslate(b.rect.x, b.rect.y); ofScale(b.rect.height / 20.0f, b.rect.height / 20.0f);
        ofDrawBitmapString(b.name, 5, 14);
        ofPopMatrix();
    }
    ofPopStyle();
}

void PlaylistControlsUI::unfocus(RoomApp* roomApp) {
    if (bEditingGolSeed) {
        bEditingGolSeed = false;
        if (roomApp) {
            try { roomApp->golBox.currentSeed = std::stol(golSeedString); } catch(...) {}
            roomApp->golBox.reset();
        }
    }
    if (bEditingGolmSeed) {
        bEditingGolmSeed = false;
        if (roomApp) {
            try { roomApp->golBoxMotion.currentSeed = std::stol(golmSeedString); } catch(...) {}
            roomApp->golBoxMotion.reset();
        }
    }
}

bool PlaylistControlsUI::mousePressed(ofVec2f worldM, Scene2D_SIDE* scene2D, RoomApp* roomApp) {
    ofRectangle copyGolBtn(golSeedBox.getRight() - 30, golSeedBox.y, 30, golSeedBox.height);
    ofRectangle copyGolmBtn(golmSeedBox.getRight() - 30, golmSeedBox.y, 30, golmSeedBox.height);
    ofRectangle pasteGolBtn(golSeedBox.getRight() - 60, golSeedBox.y, 30, golSeedBox.height);
    ofRectangle pasteGolmBtn(golmSeedBox.getRight() - 60, golmSeedBox.y, 30, golmSeedBox.height);

    if(pasteGolmBtn.inside(worldM)) {
        if(ofGetWindowPtr()) golmSeedString = ofTrim(ofGetWindowPtr()->getClipboardString());
        ofLogNotice("PlaylistControlsUI") << "Pasted GOLM seed from clipboard: " << golmSeedString;
        // Active le mode édition pour que l'utilisateur puisse valider avec Entrée
        bEditingGolmSeed = true; bEditingGolSeed = false;
        return true;
    }
    if(pasteGolBtn.inside(worldM)) {
        if(ofGetWindowPtr()) golSeedString = ofTrim(ofGetWindowPtr()->getClipboardString());
        ofLogNotice("PlaylistControlsUI") << "Pasted GOL seed from clipboard: " << golSeedString;
        bEditingGolSeed = true; bEditingGolmSeed = false;
        return true;
    }
    if(copyGolmBtn.inside(worldM)) {
        if(ofGetWindowPtr()) ofGetWindowPtr()->setClipboardString(golmSeedString);
        ofLogNotice("PlaylistControlsUI") << "Copied GOLM seed to clipboard: " << golmSeedString;
        return true;
    }
    if(copyGolBtn.inside(worldM)) {
        if(ofGetWindowPtr()) ofGetWindowPtr()->setClipboardString(golSeedString);
        ofLogNotice("PlaylistControlsUI") << "Copied GOL seed to clipboard: " << golSeedString;
        return true;
    }

    if(golmSeedBox.inside(worldM)) {
        bEditingGolmSeed = true;
        bEditingGolSeed = false;
        return true;
    } else if(golSeedBox.inside(worldM)) {
        bEditingGolSeed = true;
        bEditingGolmSeed = false;
        return true;
    } else {
        bEditingGolmSeed = false;
        bEditingGolSeed = false;
    }
    
    if(tuyauSpeedSlider.inside(worldM)) {
        bDraggingTuyauSlider = true;
        if(roomApp) {
            float pct = (worldM.x - tuyauSpeedSlider.x) / tuyauSpeedSlider.width;
            pct = ofClamp(pct, 0.0f, 1.0f);
            roomApp->tuyau.speed = pct * 5.0f;
        }
        return true;
    }
    
    if(tuyauRotXSlider.inside(worldM)) {
        bDraggingTuyauRotX = true;
        if(roomApp) {
            float pct = (worldM.x - tuyauRotXSlider.x) / tuyauRotXSlider.width;
            roomApp->tuyau.rotX = ofClamp(pct, 0.0f, 1.0f) * 360.0f;
        }
        return true;
    }
    if(tuyauRotYSlider.inside(worldM)) {
        bDraggingTuyauRotY = true;
        if(roomApp) {
            float pct = (worldM.x - tuyauRotYSlider.x) / tuyauRotYSlider.width;
            roomApp->tuyau.rotY = ofClamp(pct, 0.0f, 1.0f) * 360.0f;
        }
        return true;
    }
    if(tuyauRotZSlider.inside(worldM)) {
        bDraggingTuyauRotZ = true;
        if(roomApp) {
            float pct = (worldM.x - tuyauRotZSlider.x) / tuyauRotZSlider.width;
            roomApp->tuyau.rotZ = ofClamp(pct, 0.0f, 1.0f) * 360.0f;
        }
        return true;
    }

    if (tuyauPathBox.inside(worldM)) {
        bTuyauPathBoxOpen = !bTuyauPathBoxOpen;
        return true;
    }
    if (bTuyauPathBoxOpen && roomApp) {
        for(size_t i=0; i<tuyauPathOptionsRects.size(); i++) {
            if(tuyauPathOptionsRects[i].inside(worldM)) {
                roomApp->tuyau.selectPath(i);
                bTuyauPathBoxOpen = false;
                return true;
            }
        }
    }

    for(auto& t : roomToggles) {
        if(t.rect.inside(worldM)) { t.toggle(); return true; }
    }
    for(auto& b : roomActionBtns) {
        if(b.rect.inside(worldM)) { if (!b.continuous) b.action(); return true; }
    }
    for(auto& b : globalActionBtns) {
        if(b.rect.inside(worldM)) { if (!b.continuous) b.action(); return true; }
    }
    for(auto& t : layerToggles) {
        if(t.rect.inside(worldM)) {
            *(t.valuePtr) = !(*(t.valuePtr));
            if (t.name == "GroPuyo" && !(*(t.valuePtr))) {
                if (scene2D) scene2D->layerManager.groPuyoLayer.puyos.clear();
            }
            if (t.name == "SurSauteurs" && *(t.valuePtr)) {
                if (scene2D) scene2D->layerManager.surSauteurLayer.reset();
            }
            return true;
        }
    }
    for(int i=0; i<creatureButtons.size(); i++) {
        if(creatureButtons[i].rect.inside(worldM)) {
            selectedCreatureIndex = i;
            selectedInteractiveIndex = -1;
            if(scene2D) {
                scene2D->layerManager.selectedCreatureToSpawn = creatureButtons[i].name;
                scene2D->layerManager.selectedInteractiveLayer = "";
            }
            return true;
        }
    }
    for(int i=0; i<interactiveButtons.size(); i++) {
        if(interactiveButtons[i].rect.inside(worldM)) {
            if (selectedInteractiveIndex == i) {
                selectedInteractiveIndex = -1;
                if(scene2D) scene2D->layerManager.selectedInteractiveLayer = "";
            } else {
                selectedInteractiveIndex = i;
                selectedCreatureIndex = -1;
                if(scene2D) {
                    scene2D->layerManager.selectedInteractiveLayer = interactiveButtons[i].name;
                    scene2D->layerManager.selectedCreatureToSpawn = "";
                }
            }
            return true;
        }
    }
    for(int i=0; i<mainBrushButtons.size(); i++) {
        if(mainBrushButtons[i].rect.inside(worldM)) {
            if (selectedMainBrushIndex == i) {
                selectedMainBrushIndex = -1;
            } else {
                selectedMainBrushIndex = i;
            }
            return true;
        }
    }

    if(clearAllCreaturesBtn.inside(worldM)) {
        clearAllCreatures(scene2D);
        return true;
    }
    if(undoCreatureBtn.inside(worldM)) {
        if(scene2D) scene2D->layerManager.removeLastCreature();
        return true;
    }
    if(resetEatMapBtn.inside(worldM)) {
        if(scene2D && scene2D->layerManager.eatMapLayer) scene2D->layerManager.eatMapLayer->reset();
        return true;
    }
    if(resetCollidersBtn.inside(worldM)) {
        if(scene2D && scene2D->layerManager.colliderLayer) scene2D->layerManager.colliderLayer->reset();
        return true;
    }
    return false;
}

bool PlaylistControlsUI::mouseDragged(ofVec2f worldM, RoomApp* roomApp) {
    if(bDraggingTuyauSlider && roomApp) {
        float pct = (worldM.x - tuyauSpeedSlider.x) / tuyauSpeedSlider.width;
        pct = ofClamp(pct, 0.0f, 1.0f);
        roomApp->tuyau.speed = pct * 5.0f; // Map pct to 0.0x to 5.0x
        return true;
    }
    if(bDraggingTuyauRotX && roomApp) {
        float pct = (worldM.x - tuyauRotXSlider.x) / tuyauRotXSlider.width;
        roomApp->tuyau.rotX = ofClamp(pct, 0.0f, 1.0f) * 360.0f;
        return true;
    }
    if(bDraggingTuyauRotY && roomApp) {
        float pct = (worldM.x - tuyauRotYSlider.x) / tuyauRotYSlider.width;
        roomApp->tuyau.rotY = ofClamp(pct, 0.0f, 1.0f) * 360.0f;
        return true;
    }
    if(bDraggingTuyauRotZ && roomApp) {
        float pct = (worldM.x - tuyauRotZSlider.x) / tuyauRotZSlider.width;
        roomApp->tuyau.rotZ = ofClamp(pct, 0.0f, 1.0f) * 360.0f;
        return true;
    }
    return false;
}

void PlaylistControlsUI::mouseReleased() {
    bDraggingTuyauSlider = false;
    bDraggingTuyauRotX = false;
    bDraggingTuyauRotY = false;
    bDraggingTuyauRotZ = false;
}

void PlaylistControlsUI::handleContinuousActions(ofVec2f worldM) {
    for(auto& btn : roomActionBtns) {
        if(btn.continuous && btn.rect.inside(worldM)) btn.action();
    }
}

string PlaylistControlsUI::getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager) {
    for(auto& t : roomToggles) { if(t.rect.inside(worldM)) return tooltipManager.getTooltipText(t.name); }
    for(auto& b : roomActionBtns) {
        if(b.rect.inside(worldM)) {
            string t = tooltipManager.getTooltipText(b.name);
            if(t.find("Active ou desactive l'affichage de :") != string::npos) return "Action de la Room: " + b.name;
            return t;
        }
    }
    for(auto& b : globalActionBtns) { if(b.rect.inside(worldM)) return tooltipManager.getTooltipText(b.name); }
    for(auto& t : layerToggles) { if(t.rect.inside(worldM)) return tooltipManager.getTooltipText(t.name); }
    for(auto& b : creatureButtons) { if(b.rect.inside(worldM)) return tooltipManager.getTooltipText(b.name); }
    for(auto& b : interactiveButtons) { if(b.rect.inside(worldM)) return tooltipManager.getTooltipText("INT_" + b.name); }
    for(auto& b : mainBrushButtons) { if(b.rect.inside(worldM)) return tooltipManager.getTooltipText("MAIN_BRUSH_" + b.name); }
    if(clearAllCreaturesBtn.inside(worldM)) return tooltipManager.getTooltipText("CLEAR_CREATURES");
    if(undoCreatureBtn.inside(worldM)) return tooltipManager.getTooltipText("UNDO_SCENE2D_CREATURE");
    if(resetEatMapBtn.inside(worldM)) return tooltipManager.getTooltipText("RESET_EATMAP");
    if(resetCollidersBtn.inside(worldM)) return tooltipManager.getTooltipText("RESET_COLLIDERS");
    ofRectangle copyGolBtn(golSeedBox.getRight() - 30, golSeedBox.y, 30, golSeedBox.height);
    ofRectangle copyGolmBtn(golmSeedBox.getRight() - 30, golmSeedBox.y, 30, golmSeedBox.height);
    ofRectangle pasteGolBtn(golSeedBox.getRight() - 60, golSeedBox.y, 30, golSeedBox.height);
    ofRectangle pasteGolmBtn(golmSeedBox.getRight() - 60, golmSeedBox.y, 30, golmSeedBox.height);
    if(pasteGolBtn.inside(worldM)) return "Coller la seed GOL depuis le presse-papier.";
    if(pasteGolmBtn.inside(worldM)) return "Coller la seed GOL Motion depuis le presse-papier.";
    if(copyGolBtn.inside(worldM)) return "Copier la seed GOL dans le presse-papier.";
    if(copyGolmBtn.inside(worldM)) return "Copier la seed GOL Motion dans le presse-papier.";
    if(golSeedBox.inside(worldM)) return "Cliquez pour editer la seed GOL manuellement. Appuyez sur Entree pour valider.";
    if(golmSeedBox.inside(worldM)) return "Cliquez pour editer la seed GOL Motion manuellement. Appuyez sur Entree pour valider.";
    if(tuyauSpeedSlider.inside(worldM)) return "Slider: Controle la vitesse globale de l'animation du grand Tuyau 3D.";
    if(tuyauRotXSlider.inside(worldM)) return "Slider: Pivoter le Tuyau 3D sur l'axe X.";
    if(tuyauRotYSlider.inside(worldM)) return "Slider: Pivoter le Tuyau 3D sur l'axe Y.";
    if(tuyauRotZSlider.inside(worldM)) return "Slider: Pivoter le Tuyau 3D sur l'axe Z.";
    if(tuyauPathBox.inside(worldM)) return "Menu deroulant: Choisir la prochaine direction (Arc ou Tuyau Principal).";
    if (bTuyauPathBoxOpen) {
        for(auto& r : tuyauPathOptionsRects) if(r.inside(worldM)) return "Cliquez pour selectionner cette voie.";
    }
    return "";
}

void PlaylistControlsUI::saveSettings(ofJson& pt) {
    auto saveR = [&](const string& key, const ofRectangle& r) { pt[key]["x"] = r.x; pt[key]["y"] = r.y; pt[key]["w"] = r.width; pt[key]["h"] = r.height; };
    for(auto& t : roomToggles) saveR("room_" + t.name, t.rect);
    for(auto& b : roomActionBtns) saveR("action_" + b.name, b.rect);
    for(auto& b : globalActionBtns) saveR("globalAction_" + b.name, b.rect);
    for(auto& t : layerToggles) saveR("layer_" + t.name, t.rect);
    for(auto& b : creatureButtons) saveR("creature_" + b.name, b.rect);
    for(auto& b : interactiveButtons) saveR("interactive_" + b.name, b.rect);
    for(auto& b : mainBrushButtons) saveR("mainBrush_" + b.name, b.rect);
    saveR("clearAll", clearAllCreaturesBtn);
    saveR("undoCreature", undoCreatureBtn);
    saveR("resetEatMap", resetEatMapBtn);
    saveR("resetColliders", resetCollidersBtn);
    saveR("golmSeedBox", golmSeedBox);
    saveR("golSeedBox", golSeedBox);
    saveR("tuyauSpeedSlider", tuyauSpeedSlider);
    saveR("tuyauRotXSlider", tuyauRotXSlider);
    saveR("tuyauRotYSlider", tuyauRotYSlider);
    saveR("tuyauRotZSlider", tuyauRotZSlider);
    saveR("tuyauPathBox", tuyauPathBox);
}

void PlaylistControlsUI::loadSettings(const ofJson& pt) {
    auto loadR = [&](const string& key, ofRectangle& r) {
        if(pt.contains(key)) { 
            r.x = pt[key].value("x", r.x); 
            r.y = pt[key].value("y", r.y); 
            if(pt[key].contains("w")) r.width = pt[key].value("w", r.width);
            if(pt[key].contains("h")) r.height = pt[key].value("h", r.height);
        }
    };
    for(auto& t : roomToggles) loadR("room_" + t.name, t.rect);
    for(auto& b : roomActionBtns) loadR("action_" + b.name, b.rect);
    for(auto& b : globalActionBtns) loadR("globalAction_" + b.name, b.rect);
    for(auto& t : layerToggles) loadR("layer_" + t.name, t.rect);
    for(auto& b : creatureButtons) loadR("creature_" + b.name, b.rect);
    for(auto& b : interactiveButtons) loadR("interactive_" + b.name, b.rect);
    for(auto& b : mainBrushButtons) loadR("mainBrush_" + b.name, b.rect);
    loadR("clearAll", clearAllCreaturesBtn);
    loadR("undoCreature", undoCreatureBtn);
    loadR("resetEatMap", resetEatMapBtn);
    loadR("resetColliders", resetCollidersBtn);
    loadR("golmSeedBox", golmSeedBox);
    loadR("golSeedBox", golSeedBox);
    loadR("tuyauSpeedSlider", tuyauSpeedSlider);
    loadR("tuyauRotXSlider", tuyauRotXSlider);
    loadR("tuyauRotYSlider", tuyauRotYSlider);
    loadR("tuyauRotZSlider", tuyauRotZSlider);
    loadR("tuyauPathBox", tuyauPathBox);
}

vector<ofRectangle*> PlaylistControlsUI::getInteractableRects() {
    vector<ofRectangle*> rects;
    for(auto& t : roomToggles) rects.push_back(&t.rect);
    for(auto& b : roomActionBtns) rects.push_back(&b.rect);
    for(auto& b : globalActionBtns) rects.push_back(&b.rect);
    for(auto& t : layerToggles) rects.push_back(&t.rect);
    for(auto& b : creatureButtons) rects.push_back(&b.rect);
    for(auto& b : interactiveButtons) rects.push_back(&b.rect);
    for(auto& b : mainBrushButtons) rects.push_back(&b.rect);
    rects.push_back(&clearAllCreaturesBtn);
    rects.push_back(&undoCreatureBtn);
    rects.push_back(&resetEatMapBtn);
    rects.push_back(&resetCollidersBtn);
    rects.push_back(&golmSeedBox);
    rects.push_back(&golSeedBox);
    rects.push_back(&tuyauSpeedSlider);
    rects.push_back(&tuyauRotXSlider);
    rects.push_back(&tuyauRotYSlider);
    rects.push_back(&tuyauRotZSlider);
    rects.push_back(&tuyauPathBox);
    if (bTuyauPathBoxOpen) {
        for(auto& r : tuyauPathOptionsRects) rects.push_back(&r);
    }
    return rects;
}

ofRectangle* PlaylistControlsUI::findButtonAt(ofVec2f pos) {
    for(auto& t : roomToggles) if(t.rect.inside(pos)) return &t.rect;
    for(auto& b : roomActionBtns) if(b.rect.inside(pos)) return &b.rect;
    for(auto& b : globalActionBtns) if(b.rect.inside(pos)) return &b.rect;
    for(auto& t : layerToggles) if(t.rect.inside(pos)) return &t.rect;
    for(auto& b : creatureButtons) if(b.rect.inside(pos)) return &b.rect;
    for(auto& b : interactiveButtons) if(b.rect.inside(pos)) return &b.rect;
    for(auto& b : mainBrushButtons) if(b.rect.inside(pos)) return &b.rect;
    if(clearAllCreaturesBtn.inside(pos)) return &clearAllCreaturesBtn;
    if(undoCreatureBtn.inside(pos)) return &undoCreatureBtn;
    if(resetEatMapBtn.inside(pos)) return &resetEatMapBtn;
    if(resetCollidersBtn.inside(pos)) return &resetCollidersBtn;
    if(golmSeedBox.inside(pos)) return &golmSeedBox;
    if(golSeedBox.inside(pos)) return &golSeedBox;
    if(tuyauSpeedSlider.inside(pos)) return &tuyauSpeedSlider;
    if(tuyauRotXSlider.inside(pos)) return &tuyauRotXSlider;
    if(tuyauRotYSlider.inside(pos)) return &tuyauRotYSlider;
    if(tuyauRotZSlider.inside(pos)) return &tuyauRotZSlider;
    if (bTuyauPathBoxOpen) {
        for(auto& r : tuyauPathOptionsRects) if(r.inside(pos)) return &r;
    }
    if(tuyauPathBox.inside(pos)) return &tuyauPathBox;
    return nullptr;
}