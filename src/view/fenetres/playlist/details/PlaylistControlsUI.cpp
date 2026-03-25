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
    vector<string> iNames = {"AddGroPuyo", "AddPuyo", "AddBubble", "TargetPoulpe", "Sardine", "Shark", "AddPaperLight"};
    
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
    addToggle("Oscillate", [roomApp](){ return roomApp->bOscillateRoom; }, [roomApp](){ roomApp->bOscillateRoom = !roomApp->bOscillateRoom; });
    addToggle("Kraken", [roomApp](){ return roomApp->bDrawKraken; }, [roomApp](){ 
        roomApp->bDrawKraken = !roomApp->bDrawKraken; 
        if(roomApp->bDrawKraken) roomApp->kraken.start(roomApp->localTime);
    });
    addToggle("Ext Kraken", [roomApp](){ return roomApp->bDrawExternalKraken; }, [roomApp](){ 
        roomApp->bDrawExternalKraken = !roomApp->bDrawExternalKraken; 
        if(roomApp->bDrawExternalKraken) roomApp->externalKraken.start(roomApp->localTime);
    });
    addToggle("Cloud Ring", [roomApp](){ return roomApp->bDrawCloudRing; }, [roomApp](){ roomApp->bDrawCloudRing = !roomApp->bDrawCloudRing; });
    addToggle("Liq Sphere", [roomApp](){ return roomApp->bDrawLiquidSphere; }, [roomApp](){ roomApp->bDrawLiquidSphere = !roomApp->bDrawLiquidSphere; });
    addToggle("Jelly Sphere", [roomApp](){ return roomApp->bDrawJellySphere; }, [roomApp](){ 
        roomApp->bDrawJellySphere = !roomApp->bDrawJellySphere; 
        if(!roomApp->bDrawJellySphere) roomApp->jellySphereRing.clearJellies();
    });
    addToggle("Color Cop", [roomApp](){ return roomApp->bDrawColorCop; }, [roomApp](){ 
        roomApp->bDrawColorCop = !roomApp->bDrawColorCop; 
        if(roomApp->bDrawColorCop) roomApp->colorCopRing.resetTexture();
    });
    addToggle("Beam Proj", [roomApp](){ return roomApp->bDrawBeam; }, [roomApp](){ roomApp->bDrawBeam = !roomApp->bDrawBeam; });
    addToggle("AtmoPreview", [roomApp](){ return roomApp->bDrawAtmosphere; }, [roomApp](){ roomApp->bDrawAtmosphere = !roomApp->bDrawAtmosphere; });
    addToggle("Use Texture", [roomApp](){ return roomApp->bUseTexture; }, [roomApp](){ roomApp->bUseTexture = !roomApp->bUseTexture; });
    addToggle("Show Roof", [roomApp](){ return roomApp->bShowRoof; }, [roomApp](){ roomApp->bShowRoof = !roomApp->bShowRoof; });
    addToggle("Respire", [roomApp](){ return roomApp->respire; }, [roomApp](){ roomApp->respire = !roomApp->respire; });
    addToggle("Ripples", [roomApp](){ return roomApp->bDrawRipples; }, [roomApp](){ roomApp->bDrawRipples = !roomApp->bDrawRipples; });
    addToggle("Worms", [roomApp](){ return roomApp->bDrawWorms; }, [roomApp](){ roomApp->bDrawWorms = !roomApp->bDrawWorms; });
    addToggle("Wing Worms", [roomApp](){ return roomApp->bDrawWingedWorms; }, [roomApp](){ roomApp->bDrawWingedWorms = !roomApp->bDrawWingedWorms; });
    addToggle("Fluid Ring", [roomApp](){ return roomApp->bFluidRingEnabled; }, [roomApp](){ 
        roomApp->bFluidRingEnabled = !roomApp->bFluidRingEnabled; 
        roomApp->fluidRing.setTargetAlpha(roomApp->bFluidRingEnabled ? 1.0f : 0.0f);
    });
    addToggle("Light Fly", [roomApp](){ return roomApp->bLightFlyRingEnabled; }, [roomApp](){ roomApp->bLightFlyRingEnabled = !roomApp->bLightFlyRingEnabled; });
    addToggle("Alpha Cur", [roomApp](){ return roomApp->cursorSquare.bLowAlpha; }, [roomApp](){ roomApp->cursorSquare.bLowAlpha = !roomApp->cursorSquare.bLowAlpha; });
    addToggle("Gen 360", [](){ return false; }, [roomApp](){ roomApp->generateEquirectangularImage(); });
    addToggle("Gen 360 Full", [](){ return false; }, [roomApp](){ roomApp->generateFull360EquirectangularImage(); });
    addToggle("360fullW", [](){ return false; }, [roomApp](){ roomApp->generate360FullW(); });
    addToggle("Show Beams", [roomApp](){ return roomApp->projection.getShowBeams(); }, [roomApp](){ roomApp->projection.keyPressed('t'); });
    addToggle("Plan Colle", [roomApp](){ return roomApp->projection.getShowPlanColle(); }, [roomApp](){ roomApp->projection.keyPressed('n'); });
    addToggle("Cur Reflet", [roomApp](){ return roomApp->cursorSquare.bDrawReflections; }, [roomApp](){ roomApp->cursorSquare.bDrawReflections = !roomApp->cursorSquare.bDrawReflections; });
    addToggle("Prev Interact", [mainAppPtr](){ return mainAppPtr && mainAppPtr->roomPreviewApp ? mainAppPtr->roomPreviewApp->bDrawInteraction : false; }, [mainAppPtr](){ if(mainAppPtr && mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->bDrawInteraction = !mainAppPtr->roomPreviewApp->bDrawInteraction; });
    addToggle("Prev Cursor", [mainAppPtr](){ return mainAppPtr && mainAppPtr->roomPreviewApp ? mainAppPtr->roomPreviewApp->bShowCursor : false; }, [mainAppPtr](){ if(mainAppPtr && mainAppPtr->roomPreviewApp) mainAppPtr->roomPreviewApp->bShowCursor = !mainAppPtr->roomPreviewApp->bShowCursor; });

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

    addAction("Cam Reset [R]", [roomApp](){ roomApp->keyPressed('r'); }, false);
    addAction("Cam Lock [L]", [roomApp](){ 
        roomApp->bLockCameraCenter = !roomApp->bLockCameraCenter; 
        if (!roomApp->bLockCameraCenter) {
            roomApp->camGlobal.setDistance(4000);
            roomApp->camGlobal.setPosition(2000, 2500, 3000);
            roomApp->camGlobal.lookAt(ofVec3f(0, 600, 0));
        }
    }, false, [roomApp](){ return roomApp->bLockCameraCenter; });
    addAction("Sphere [P]", [roomApp, this](){ roomApp->atmosphere.keyPressed('p'); uiStateSphereP = !uiStateSphereP; }, false, [this](){ return uiStateSphereP; });
    addAction("Disco [M]", [roomApp, this](){ roomApp->atmosphere.keyPressed('m'); uiStateDiscoM = !uiStateDiscoM; }, false, [this](){ return uiStateDiscoM; });
    addAction("Mode 360 [O]", [roomApp](){ roomApp->atmosphere.keyPressed('o'); }, false, [roomApp](){ return roomApp->atmosphere.bShow360; });
    addAction("AutoRot [I]", [roomApp](){ roomApp->atmosphere.keyPressed('i'); }, false);
    
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
    addAction("Clear Flys [X]", [roomApp](){ roomApp->lightFlyRing.clearLights(); }, false);

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

    addAction("SAVE GAB", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->canvasManager.canvas.isAllocated()) {
            ofPixels pix;
            mainAppPtr->canvasManager.canvas.readToPixels(pix);
            ofSaveImage(pix, "export/gab_frame_" + ofGetTimestampString() + ".png");
            ofLogNotice("PlaylistControlsUI") << "Main Canvas Frame saved.";
        }
    }, false);

    addAction("UNDO CREA [Z/D]", [mainAppPtr](){ 
        if(mainAppPtr) {
            mainAppPtr->creatureSystem.removeLast();
            ofLogNotice("PlaylistControlsUI") << "Derniere creature retiree.";
        }
    }, false);

    addAction("CLEAR ALL CREA", [mainAppPtr](){ 
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

    addAction("REC CANVAS", [mainAppPtr, this](){ 
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

    addAction("BLUR SHADER", [mainAppPtr](){ 
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

    addAction("EXP SCENE2D", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportFullScene();
    }, false);

    addAction("EXP COLLIDER", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportColliders();
    }, false);

    addAction("EXP EATMAP", [mainAppPtr](){ 
        if(mainAppPtr && mainAppPtr->scene2D) mainAppPtr->scene2D->exportEatMap();
    }, false);

    addAction("GAB 3-OFF-3 [G]", [mainAppPtr](){ 
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
        if (b.name == "PAUSE [ESC]" && mainAppPtr && mainAppPtr->bGlobalPause) ofSetColor(200, 50, 50); 
        else if (b.name == "BTN WORMS" && mainAppPtr && mainAppPtr->buttonApp && mainAppPtr->buttonApp->buttonWindow.bDrawWorms) ofSetColor(50, 200, 50); 
        else if ((b.name == "REC PREVIEW" || b.name == "REC CANVAS") && b.getState && b.getState()) {
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
        if(t.name == "Show Beams" && mainAppPtr && mainAppPtr->roomApp) {
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

bool PlaylistControlsUI::mousePressed(ofVec2f worldM, Scene2D_SIDE* scene2D) {
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
    return nullptr;
}