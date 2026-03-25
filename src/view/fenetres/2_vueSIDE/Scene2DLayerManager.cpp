#include "Scene2DLayerManager.h"
#include "SwingLayer.h"

void Scene2DLayerManager::setup(float totalWidth, float jarW, float jarX, float frontW, float frontX) {
    totalSceneWidth = totalWidth;
    
    imgConcombre.load("IMG_REF/con.png");
    imgRush.load("IMG_REF/rushA.png");

    // --- SETUP CREATURE SYSTEM ---
    creatureSystem.setup();
    float centerX = jarX + jarW / 2.0f;
    float centerY = 1080.0f; 

    // --- INIT COLLIDERS & LAYERS ---
    colliderLayer = make_shared<ColliderLayer>();
    float simWidth = 2048.0f;
    float scale = totalSceneWidth / simWidth;
    // On utilise une seule simulation qui couvre toute la hauteur de l'image (1472px)
    float simHeight = 1472.0f / scale;
    colliderLayer->setup(simWidth, simHeight, scale);
    creatureSystem.setCollider(colliderLayer);

    // --- SETUP SUBSYSTEMS ---
    slimeLayer.setup(totalSceneWidth, 1472.0f);
    slimeLayer.setCollider(colliderLayer);
    slimeLayer.setScale(scale);

    sauteursLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);
    fishSchoolLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);
    poulpeLayer.setup(totalSceneWidth, 1472.0f);
    walkerLayer.setup(totalSceneWidth, 830.0f);
    walkerLayer.setScale(2.0f);

    // --- SETUP GEARS ---
    gearLayer.setup(totalSceneWidth, 1472.0f);

    lightningLayer.setup(totalSceneWidth, 1500.0f);
    lightningLayer.setScale(1);

    // --- SETUP PLANTS ---
    plantLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP FLYTRAPS ---
    flytrapLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP FLUID FLOOR ---
    fluidFloorLayer.setup(simWidth, simHeight, scale, 720, 256);
    fluidFloorLayer.setCollider(colliderLayer);
    fluidFloorLayer.globalAlpha = 0.0f; // On démarre caché dans la scène 2D
    fluidFloorLayer.targetAlpha = 0.0f;

    // --- SETUP MACHINE LAYER ---
    machineLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP DIGGING CREATURE ---
    diggingCreature.setup(frontX + frontW/2.0f, 1000.0f, imgRush);

    // --- SETUP MACHINE AUTO ---
    machineAuto.setup(totalSceneWidth, 1472.0f);

    // --- SETUP CURTAIN (RIDEAU) ---
    curtain.setup(frontX + 200, 100, 600, 800, "IMG_REF/garde.png");
    
    // --- SETUP PUYO LAYER ---
    puyoLayer.setup(simWidth, simHeight, scale, colliderLayer);
    
    // --- SETUP GRO PUYO LAYER ---
    groPuyoLayer.setup(simWidth, simHeight, scale, colliderLayer);

    // --- SETUP BUBBLE LAYER ---
    bubbleLayer.setup(simWidth, simHeight, scale, colliderLayer, 0.0f);
    
    // --- SETUP KANI LAYER ---
    kaniLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);

    // --- SETUP SLIME2 LAYER ---
    slime2Layer.setup(totalSceneWidth, 1472.0f, scale, colliderLayer);

    // --- SETUP TEAA LAYER ---
    teaaLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);

    // --- SETUP BALLET LAYER ---
    balletLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP KUNDELICH LAYER ---
    kundelichLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP KINESHAD LAYER ---
    kineShadLayer.setup();
    
    // --- SETUP MULTI PENDULUM ---
    multiPendulumLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP PANCARTE ---
    pancarteLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP PENDULUM ---
    pendulumLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP PINCE ---
    pinceLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP PINCE BRA ---
    pinceBraLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP PINCE FOIRE ---
    pinceFoireLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP WHAA ---
    whaaLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP CHAIN CREA ---
    chainCreaLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP WALL WALKER ---
    wallWalkerLayer.setup(simWidth, simHeight, scale, colliderLayer);

    // --- SETUP SWING LAYER ---
    swingLayer.setup(simWidth, simHeight, scale, colliderLayer);

    // --- SETUP FLUID LAYER ---
    fluidLayer.setup(simWidth, simHeight, scale);
    fluidLayer.setCollider(colliderLayer);

    // --- SETUP FLUID DEUX LAYER ---
    fluidDeuxLayer.setup(simWidth, simHeight, scale);
    fluidDeuxLayer.setCollider(colliderLayer);

    // --- SETUP FLUID TROIS LAYER ---
    fluidTroisLayer.setup(simWidth, simHeight, scale);
    fluidTroisLayer.setCollider(colliderLayer);
    
    // --- SETUP FIRE A LAYER ---
    fireALayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP FIRE B LAYER ---
    fireBLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP FIRE C LAYER ---
    fireCLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP MONGOLFIER LAYER ---
    mongolfierLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP WALKING SQUARE LAYER ---
    walkingSquareLayer.setup(totalSceneWidth, 1472.0f);
    
    // --- SETUP TRIPOD EYE LAYER ---
    tripodEyeLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);

    // --- SETUP AVOIDER LAYER ---
    avoiderLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP AVOIDER 2 LAYER ---
    avoider2Layer.setup(totalSceneWidth, 1472.0f);
    avoider2Layer.setCollider(colliderLayer);
    avoider2Layer.setScale(scale);

    // --- SETUP AVOIDER 4 LAYER ---
    avoider4Layer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP TOURELLES LAYER ---
    tourellesLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP AUTO PONG LAYER ---
    autoPongLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP AUTO SNAKE LAYER ---
    autoSnakeLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);

    eatMapLayer = make_shared<EatMapLayer>();
    eatMapLayer->setup(simWidth, simHeight, scale);
    surSauteurLayer.setup(totalSceneWidth, 1472.0f, colliderLayer, eatMapLayer);
}

void Scene2DLayerManager::update(const ofVec2f& m, float time, bool isSpacePressed) {
    if (bDrawLightning) lightningLayer.update(m.x, m.y, time);

    if (bDrawCreatures) {
        creatureSystem.update(m, time);
        for(auto& c : cousinCons) c->update(m.x, m.y); // CousinCon doesn't use time in update signature yet, but uses internal time
        for(auto& h : halos) h->update(time);
    }

    if (bDrawWalker) walkerLayer.update(m.x, m.y, time);

    if (bDrawPoulpe) {
        if (!isSpacePressed && ofGetMousePressed() && selectedInteractiveLayer == "TargetPoulpe") {
            poulpeLayer.setTarget(m.x, m.y);
        }
        poulpeLayer.update(0, 0, time); // PoulpeLayer::update overrides BaseLayer, so it needs args
    }

    if (bDrawFish) {
        fishSchoolLayer.update(m.x, m.y, time); 
        if (!isSpacePressed && ofGetMousePressed()) {
            if (selectedInteractiveLayer == "Sardine") fishSchoolLayer.addSardine(m.x, m.y);
            else if (selectedInteractiveLayer == "Shark") fishSchoolLayer.addShark(m.x, m.y);
        }
    }

    if (bDrawSauteurs) sauteursLayer.update(m.x, m.y, time); 
    if (bDrawSlime) slimeLayer.update(m.x, m.y, time);
    if (bDrawPlants) plantLayer.update(m.x, m.y, time); 
    if (bDrawFlytraps) flytrapLayer.update(m.x, m.y, time); 

    if (bDrawGears) {
        gearLayer.update(m.x, m.y, time);
        
        if (bDrawFluidFloor || fluidFloorLayer.globalAlpha > 0.0f) {
            for(const auto& s : gearLayer.squares) {
                // Conversion World -> Sim pour l'interaction
                float simX = s.pos.x / fluidFloorLayer.scale;
                float simY = s.pos.y / fluidFloorLayer.scale;
                fluidFloorLayer.addForce(simX, simY, s.vel.x / fluidFloorLayer.scale * 0.5f, s.vel.y / fluidFloorLayer.scale * 0.5f);
            }
        }
    }

    fluidFloorLayer.setTargetAlpha(bDrawFluidFloor ? 1.0f : 0.0f);
    if (bDrawFluidFloor || fluidFloorLayer.globalAlpha > 0.0f) fluidFloorLayer.update(m.x, m.y);

    if (bDrawMachine) machineLayer.update(m.x, m.y, time);
    if (bDrawDigging) diggingCreature.update(m.x, m.y);
    if (bDrawMachineAuto) machineAuto.update(time); 
    if (bDrawCurtain) curtain.update(m.x, m.y);
    if (bDrawPuyo) puyoLayer.update(m.x, m.y, time); 
    if (bDrawGroPuyo) groPuyoLayer.update(m.x, m.y, time);
    if (bDrawBubbles) bubbleLayer.update(m.x, m.y, time);
    if (bDrawKani) kaniLayer.update(m.x, m.y, time);
    if (bDrawSlime2) slime2Layer.update(time);
    if (bDrawTeaa) teaaLayer.update(time);
    
    balletLayer.bActive = bDrawBallet;
    if (bDrawBallet) {
        balletLayer.update(time);
        if (bDrawFluidFloor || fluidFloorLayer.globalAlpha > 0.0f) {
            for(const auto& p : balletLayer.particles) {
                // Conversion World -> Sim
                float simX = p.pos.x / fluidFloorLayer.scale;
                float simY = p.pos.y / fluidFloorLayer.scale;
                // On applique une légère force en fonction de la vélocité de la particule
                fluidFloorLayer.addForce(simX, simY, p.vel.x / fluidFloorLayer.scale * 0.2f, p.vel.y / fluidFloorLayer.scale * 0.2f);
            }
        }
    }
    
    if (bDrawKundelich) kundelichLayer.update(m.x, m.y, time);
    if (bDrawKineShad) kineShadLayer.update(m, time);
    if (bDrawMultiPendulum) multiPendulumLayer.update(m.x, m.y, time);
    if (bDrawPancarte) pancarteLayer.update(m.x, m.y, time);
    if (bDrawPendulum) pendulumLayer.update(m.x, m.y);
    if (bDrawPince) pinceLayer.update(m.x, m.y, time);
    if (bDrawPinceBra) pinceBraLayer.update(m.x, m.y, time);
    if (bDrawPinceFoire) pinceFoireLayer.update(m.x, m.y, time);
    if (bDrawWhaa) whaaLayer.update(m.x, m.y);
    if (bDrawChainCrea) chainCreaLayer.update(m.x, m.y, time);
    if (bDrawWallWalker) wallWalkerLayer.update(m.x, m.y, time);
    if (bDrawSwing) swingLayer.update(m.x, m.y, time);
    if (bDrawFluid) fluidLayer.update(m.x, m.y, time);
    if (bDrawFluidDeux) fluidDeuxLayer.update(m.x, m.y, time);
    if (bDrawFluidTrois) fluidTroisLayer.update(m.x, m.y, time);
    if (bDrawFireA) fireALayer.update(m.x, m.y);
    if (bDrawFireB) fireBLayer.update(m.x, m.y, time);
    if (bDrawFireC) fireCLayer.update(m.x, m.y);
    if (bDrawMongolfier) mongolfierLayer.update(m.x, m.y);
    if (bDrawWalkingSquare) walkingSquareLayer.update(m.x, m.y, time);
    if (bDrawTripodEye) tripodEyeLayer.update(m.x, m.y, time);
    
    if (bDrawAvoider) {
        avoiderLayer.update(time);
        // Interaction avec le fluide
        if (bDrawFluidFloor || fluidFloorLayer.globalAlpha > 0.0f) {
            for(const auto& a : avoiderLayer.avoiders) {
                float simX = a.pos.x / fluidFloorLayer.scale;
                float simY = a.pos.y / fluidFloorLayer.scale;
                fluidFloorLayer.addForce(simX, simY, a.vel.x / fluidFloorLayer.scale * 0.4f, a.vel.y / fluidFloorLayer.scale * 0.4f);
            }
        }
    }

    if (bDrawAvoider2) {
        avoider2Layer.update(time);
        // Interaction avec le fluide
        if (bDrawFluidFloor || fluidFloorLayer.globalAlpha > 0.0f) {
            for(const auto& a : avoider2Layer.avoiders) {
                float simX = a.pos.x / fluidFloorLayer.scale;
                float simY = a.pos.y / fluidFloorLayer.scale;
                fluidFloorLayer.addForce(simX, simY, a.vel.x / fluidFloorLayer.scale * 0.4f, a.vel.y / fluidFloorLayer.scale * 0.4f);
            }
        }
    }

    if (bDrawAvoider4) avoider4Layer.update(time);
    if (bDrawTourelles) tourellesLayer.update(time);
    if (bDrawAutoPong) autoPongLayer.update(time);
    if (bDrawAutoSnake) autoSnakeLayer.update(time);
    
    if (bDrawSurSauteurs) {
        surSauteurLayer.bActive = true;
        surSauteurLayer.update(m.x, m.y, time);
    } else { surSauteurLayer.bActive = false; }
}

void Scene2DLayerManager::draw(const ofVec2f& m) {
    if (bDrawCreatures) {
        creatureSystem.draw(m);
        for(auto& c : cousinCons) c->draw();
        for(auto& h : halos) h->draw();
    }

    if (bDrawLightning && bLightningHasStart) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(255, 50, 50);
        ofSetLineWidth(2);
        ofDrawCircle(lightningStartPos.x, lightningStartPos.y, 10);
        ofSetColor(255, 255, 255, 100);
        ofDrawLine(lightningStartPos, m);
        ofPopStyle();
    }

    if (bDrawLightning) lightningLayer.draw();
    if (bDrawSlime) slimeLayer.draw();
    if (bDrawSauteurs) sauteursLayer.draw();
    if (bDrawFish) fishSchoolLayer.draw();
    if (bDrawPoulpe) poulpeLayer.draw();

    if (bDrawWalker) {
        ofPushMatrix();
        ofTranslate(0, 1472 - 830); 
        walkerLayer.draw();
        ofPopMatrix();
    }

    if (bDrawPlants) plantLayer.draw();
    if (bDrawFlytraps) flytrapLayer.draw();
    if (bDrawGears) gearLayer.draw();
    if (bDrawFluidFloor || fluidFloorLayer.globalAlpha > 0.0f) fluidFloorLayer.draw(0, 0);
    if (bDrawMachine) machineLayer.draw();
    if (bDrawDigging) diggingCreature.draw();
    if (bDrawMachineAuto) machineAuto.draw();
    if (bDrawCurtain) curtain.draw();
    if (bDrawPuyo) puyoLayer.draw();
    if (bDrawGroPuyo) groPuyoLayer.draw();
    if (bDrawBubbles) bubbleLayer.draw();
    if (bDrawKani) kaniLayer.draw();
    if (bDrawSlime2) slime2Layer.draw();
    if (bDrawTeaa) teaaLayer.draw();
    if (bDrawBallet) balletLayer.draw();
    if (bDrawKundelich) kundelichLayer.draw();
    if (bDrawKineShad) kineShadLayer.draw();
    if (bDrawMultiPendulum) multiPendulumLayer.draw();
    if (bDrawPancarte) pancarteLayer.draw();
    if (bDrawPendulum) pendulumLayer.draw();
    if (bDrawPince) pinceLayer.draw();
    if (bDrawPinceBra) pinceBraLayer.draw();
    if (bDrawPinceFoire) pinceFoireLayer.draw();
    if (bDrawWhaa) whaaLayer.draw();
    if (bDrawChainCrea) chainCreaLayer.draw();
    if (bDrawWallWalker) wallWalkerLayer.draw();
    if (bDrawSwing) swingLayer.draw();
    if (bDrawFluid) fluidLayer.draw();
    if (bDrawFluidDeux) fluidDeuxLayer.draw();
    if (bDrawFluidTrois) fluidTroisLayer.draw();
    if (bDrawFireA) fireALayer.draw();
    if (bDrawFireB) fireBLayer.draw();
    if (bDrawFireC) fireCLayer.draw();
    if (bDrawMongolfier) mongolfierLayer.draw();
    if (bDrawWalkingSquare) walkingSquareLayer.draw();
    if (bDrawTripodEye) tripodEyeLayer.draw();
    if (bDrawAvoider) avoiderLayer.draw();
    if (bDrawAvoider2) avoider2Layer.draw();
    if (bDrawAvoider4) avoider4Layer.draw();
    if (bDrawTourelles) tourellesLayer.draw();
    if (bDrawAutoPong) autoPongLayer.draw();
    if (bDrawAutoSnake) autoSnakeLayer.draw();
    if (bDrawSurSauteurs) surSauteurLayer.draw();
    if (bDrawEatMap && eatMapLayer) eatMapLayer->draw();
    
    // Les colliders sont dessinés en tout dernier pour apparaître au premier plan
    if (bDrawColliders && colliderLayer) colliderLayer->draw();
}

void Scene2DLayerManager::addCousinCon(float x, float y) {
    cousinCons.push_back(make_shared<CousinCon>(x, y, &imgConcombre));
}

void Scene2DLayerManager::addHalo(float x, float y) {
    halos.push_back(make_shared<HaloCreature>(x, y));
}

void Scene2DLayerManager::spawnSelectedCreature(float x, float y) {
    if(selectedCreatureToSpawn == "Ripple") creatureSystem.addRipple(x, y, 0);
    else if(selectedCreatureToSpawn == "Wanco") creatureSystem.addWancoCreature(x, y);
    else if(selectedCreatureToSpawn == "Breakable") creatureSystem.addBreakableCreature(x, y);
    else if(selectedCreatureToSpawn == "Geko") creatureSystem.addGekoCreature(x, y);
    else if(selectedCreatureToSpawn == "Cousin") creatureSystem.addCousinCreature(x, y);
    else if(selectedCreatureToSpawn == "CousinCon") addCousinCon(x, y);
    else if(selectedCreatureToSpawn == "DblPendulum") creatureSystem.addDoublePendulum(x, y);
    else if(selectedCreatureToSpawn == "Halo") addHalo(x, y);
    else if(selectedCreatureToSpawn == "Fluids") creatureSystem.addFluidsCreature(x, y);
    else if(selectedCreatureToSpawn == "Spring") creatureSystem.addSpringCreature(x, y);
    else if(selectedCreatureToSpawn == "Dancing") creatureSystem.addDancingCreature(x, y);
    else if(selectedCreatureToSpawn == "Creature") creatureSystem.addCreature(x, y);
    else if(selectedCreatureToSpawn == "Otarie") creatureSystem.addOtarieCreature(x, y);
    else if(selectedCreatureToSpawn == "Sauteur") creatureSystem.addCousinSauteur(x, y);
}

void Scene2DLayerManager::removeLastCreature() {
    creatureSystem.removeLast();
    if(!cousinCons.empty()) cousinCons.pop_back();
    if(!halos.empty()) halos.pop_back();
}

void Scene2DLayerManager::keyPressed(int key, const ofVec2f& m) {
    // --- FENETRE 2D_SIDE ---
    switch (key) {
        //case 'h': case 'H': bDrawCreatures = !bDrawCreatures; return; // display creature
        case 'd': case 'D': // remove last
            creatureSystem.removeLast();
            if (!cousinCons.empty()) cousinCons.pop_back();
            if (!halos.empty()) halos.pop_back();
            return;
        case 'c': case 'C': // clean creature
            creatureSystem.clear();
            cousinCons.clear();
            halos.clear();
            return;
    }

    // --- CREATURE Spawning ---
    switch (key) {
        case 'o': case 'O': creatureSystem.addRipple(m.x, m.y, 0); break;
      
        case 'q': case 'Q': creatureSystem.addWancoCreature(m.x, m.y); break;
        case 'b': case 'B': creatureSystem.addBreakableCreature(m.x, m.y); break;
        case 'f': case 'F': creatureSystem.addGekoCreature(m.x, m.y); break;
        case 'a': case 'A': 
            if (selectedInteractiveLayer != "") {
                if (selectedInteractiveLayer == "AddGroPuyo" && bDrawGroPuyo) {
                    groPuyoLayer.addGroPuyo(m.x / groPuyoLayer.scale, m.y / groPuyoLayer.scale);
                } else if (selectedInteractiveLayer == "Pendulum" && bDrawPendulum) {
                    pendulumLayer.mousePressed(m.x, m.y);
                } else if (selectedInteractiveLayer == "AddPuyo" && bDrawPuyo) {
                    puyoLayer.addPuyo(m.x / puyoLayer.scale, m.y / puyoLayer.scale);
                } else if (selectedInteractiveLayer == "AddBubble" && bDrawBubbles) {
                    bubbleLayer.addBubble(m.x / bubbleLayer.scale, m.y / bubbleLayer.scale);
                } else if (selectedInteractiveLayer == "TargetPoulpe" && bDrawPoulpe) {
                    poulpeLayer.setTarget(m.x, m.y);
                } else if (selectedInteractiveLayer == "Sardine" && bDrawFish) {
                    fishSchoolLayer.addSardine(m.x, m.y);
                } else if (selectedInteractiveLayer == "Shark" && bDrawFish) {
                    fishSchoolLayer.addShark(m.x, m.y);
                }
            } else if (selectedCreatureToSpawn != "") {
                spawnSelectedCreature(m.x, m.y); 
            }
            break; 
        case 'z': case 'Z': removeLastCreature(); break; 
        case 's': case 'S': creatureSystem.addDoublePendulum(m.x, m.y); break;
        case 'y': case 'Y': addHalo(m.x, m.y); break;
        
            
            
            //creatureSystem.addFluidsCreature(m.x, m.y); 
            //bDrawBallet = !bDrawBallet; balletLayer.bActive = bDrawBallet;
            //creatureSystem.addSpringCreature(m.x, m.y);
            //bDrawKundelich = !bDrawKundelich;
            //bDrawPinceFoire = !bDrawPinceFoire;
            //bDrawTeaa = !bDrawTeaa; teaaLayer.bActive = bDrawTeaa; if(bDrawTeaa) teaaLayer.generate(); 
            //bDrawCurtain = !bDrawCurtain; 
            //bDrawMultiPendulum = !bDrawMultiPendulum;
            
            //bDrawWhaa = !bDrawWhaa;
            //creatureSystem.addDancingCreature(m.x, m.y);
            //creatureSystem.addCreature(m.x, m.y); 
            //bDrawKani = !bDrawKani; 
            //bDrawWallWalker = !bDrawWallWalker;
            
            //bDrawChainCrea = !bDrawChainCrea;
            //bDrawKineShad = !bDrawKineShad;  
            //bDrawPancarte = !bDrawPancarte;
            
            //bDrawFluid = !bDrawFluid;
            //bDrawFluidDeux = !bDrawFluidDeux;
            //bDrawFluidTrois = !bDrawFluidTrois;

            //bDrawFireA = !bDrawFireA;
            //bDrawFireB = !bDrawFireB;
            //bDrawFireC = !bDrawFireC;

            //creatureSystem.addOtarieCreature(m.x, m.y);
            //bDrawSwing = !bDrawSwing;
            //bDrawMongolfier = !bDrawMongolfier;

            //bDrawPendulum = !bDrawPendulum;
            //bDrawPince = !bDrawPince;
            //bDrawPinceBra = !bDrawPinceBra;

            //bDrawBubbles = !bDrawBubbles; 
            //creatureSystem.addCousinSauteur(m.x, m.y);

        case '2': 
            bDrawWalkingSquare = !bDrawWalkingSquare;
        break;
       
        case '3':
            bDrawTripodEye = !bDrawTripodEye;
        break;

        case '4':                  
        break; 
        
        case '5': 
        bDrawColliders = !bDrawColliders;           
        break;
        
        case '6':            
        break;
        
        case '7':           
            if (bDrawPuyo){
                puyoLayer.puyos.clear();
            } else {
                for(int i=0; i<15; i++) {
                    puyoLayer.addPuyo(ofRandom(puyoLayer.simWidth), puyoLayer.simHeight - ofRandom(50, 150));
                }
            }
            bDrawPuyo = !bDrawPuyo; 
        break;
        
        case '8': 
             bDrawGroPuyo = !bDrawGroPuyo;
            if(!bDrawGroPuyo) {
                groPuyoLayer.puyos.clear();
            }      
        break;
        
        case '9': 
             if(bDrawGroPuyo) groPuyoLayer.addGroPuyo(m.x / groPuyoLayer.scale, m.y / groPuyoLayer.scale);
        break;

        case '0':
            
        break;
    }

    // --- LAYER Toggles ---
    switch (key) {
        case 'v': case 'V': bDrawFluidFloor = !bDrawFluidFloor; break;
        case 'i': case 'I': bDrawLightning = !bDrawLightning; break;
        case 'p': case 'P': // Toggles both Plants and Flytraps
            bDrawPlants = !bDrawPlants;
            bDrawFlytraps = !bDrawFlytraps;
            break;
        case 'm': case 'M': 
        bDrawSlime = !bDrawSlime; 
        bDrawSlime2 = !bDrawSlime2;
        //if(bDrawSlime2) slime2Layer.explode(m.x, m.y);
        break;
        
        case 'k': case 'K': bDrawFish = !bDrawFish; break;
        case 'e': case 'E':
            bDrawGears = !bDrawGears;
            if (!bDrawGears) gearLayer.squares.clear();
            break;
        case 'u': case 'U': // Toggles both Machine and MachineAuto
            bDrawMachine = !bDrawMachine;
            bDrawMachineAuto = !bDrawMachineAuto;
            break;
        case 'j': case 'J': bDrawPoulpe = !bDrawPoulpe; break;
        case 'l': case 'L': bDrawSauteurs = !bDrawSauteurs; break;
        case '0': bDrawWalker = !bDrawWalker; break; // Ancien 'N' remplacé par 0 pour éviter le conflit avec Playlist
        case 't': case 'T': // Digging creature layer
            bDrawDigging = !bDrawDigging;
            break;
        case 'w': case 'W': fluidFloorLayer.toggleBackground(); break;
    }
}

void Scene2DLayerManager::mousePressed(const ofVec2f& m, int button) {
    // Gestion des clics pour les calques interactifs (sauf poissons gérés en continu dans update)
    if (selectedInteractiveLayer != "") {
        if (selectedInteractiveLayer == "AddGroPuyo" && bDrawGroPuyo) {
            groPuyoLayer.addGroPuyo(m.x / groPuyoLayer.scale, m.y / groPuyoLayer.scale);
        } else if (selectedInteractiveLayer == "Pendulum" && bDrawPendulum) {
            pendulumLayer.mousePressed(m.x, m.y);
        } else if (selectedInteractiveLayer == "AddPuyo" && bDrawPuyo) {
            puyoLayer.addPuyo(m.x / puyoLayer.scale, m.y / puyoLayer.scale);
        } else if (selectedInteractiveLayer == "AddBubble" && bDrawBubbles) {
            bubbleLayer.addBubble(m.x / bubbleLayer.scale, m.y / bubbleLayer.scale);
        } else if (selectedInteractiveLayer == "TargetPoulpe" && bDrawPoulpe) {
            poulpeLayer.setTarget(m.x, m.y);
        }
    }

    if(bDrawCreatures) {
        creatureSystem.onPress(m.x, m.y);
        for(auto& c : cousinCons) c->onPress(m.x, m.y);
    }
    if(bDrawLightning) {
        if (!bLightningHasStart) {
            lightningStartPos = m;
            bLightningHasStart = true;
        } else {
            lightningLayer.trigger(lightningStartPos, m);
            bLightningHasStart = false; 
        }
    }
    if (bDrawMachine) machineLayer.mousePressed(m.x, m.y, button);
    if (bDrawSlime2) slime2Layer.explode(m.x, m.y);
    if (bDrawMachineAuto) machineAuto.mousePressed(m.x, m.y);
    if (bDrawCurtain) curtain.mousePressed(m.x, m.y);
    if (bDrawTeaa) teaaLayer.mousePressed(m.x, m.y, button);
    if (bDrawPancarte) pancarteLayer.mousePressed(m.x, m.y, button);
    if (bDrawPendulum) pendulumLayer.mousePressed(m.x, m.y);
    if (bDrawPinceBra) pinceBraLayer.mousePressed(m.x, m.y, button);
    if (bDrawPinceFoire) pinceFoireLayer.mousePressed(m.x, m.y, button);
    if (bDrawWhaa) whaaLayer.mousePressed(m.x, m.y);
    if (bDrawFluid) fluidLayer.mousePressed(m.x, m.y, button);
    if (bDrawFluidDeux) fluidDeuxLayer.mousePressed(m.x, m.y, button);
    if (bDrawFluidTrois) fluidTroisLayer.mousePressed(m.x, m.y, button);
    if (bDrawFireA) fireALayer.mousePressed(m.x, m.y);
    if (bDrawFireB) fireBLayer.mousePressed(m.x, m.y, button);
    if (bDrawFireC) fireCLayer.mousePressed(m.x, m.y);
}

void Scene2DLayerManager::mouseReleased(const ofVec2f& m, int button) {
    if(bDrawCreatures) {
        creatureSystem.onRelease(m.x, m.y);
        for(auto& c : cousinCons) c->onRelease(m.x, m.y);
    }
    if(bDrawCurtain) curtain.mouseReleased(m.x, m.y);
    if (bDrawPancarte) pancarteLayer.mouseReleased(m.x, m.y, button);
    if (bDrawPendulum) pendulumLayer.mouseReleased(m.x, m.y);
    if (bDrawPinceFoire) pinceFoireLayer.mouseReleased(m.x, m.y, button);
    if (bDrawWhaa) whaaLayer.mouseReleased(m.x, m.y);
    if (bDrawFluid) fluidLayer.mouseReleased(m.x, m.y, button);
    if (bDrawFluidDeux) fluidDeuxLayer.mouseReleased(m.x, m.y, button);
    if (bDrawFluidTrois) fluidTroisLayer.mouseReleased(m.x, m.y, button);
    if (bDrawFireA) fireALayer.mouseReleased(m.x, m.y);
}

int Scene2DLayerManager::getSardineCount() {
    return fishSchoolLayer.getSardineCount();
}