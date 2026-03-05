#include "Scene2DLayerManager.h"

void Scene2DLayerManager::setup(float totalWidth, float jarW, float jarX, float frontW, float frontX) {
    totalSceneWidth = totalWidth;
    
    imgConcombre.load("con.png");
    imgRush.load("rushA.png");

    // --- SETUP CREATURE SYSTEM ---
    creatureSystem.setup();
    float centerX = jarX + jarW / 2.0f;
    float centerY = 1080.0f; 

    /*
    creatureSystem.addDoublePendulum(centerX+400, centerY-200);
    creatureSystem.addWancoCreature(centerX+400, centerY-200);
    creatureSystem.addFluidsCreature(centerX, centerY-700);
    creatureSystem.addCreature(2400 * 0.33, 736);
    creatureSystem.addRipple(2900 * 0.66, 736);
    creatureSystem.addDancingCreature(frontX + frontW/2, 600);
    creatureSystem.addSpringCreature(frontX + frontW/2, 712);
    creatureSystem.addGekoCreature(frontX + frontW/2 - 300, 600);
    creatureSystem.addGekoCreature(centerX, centerY);       // Crée le Gecko Standard
    creatureSystem.addGekoCreature(centerX - 50, centerY);  // Crée le Gecko Agile
    creatureSystem.addGekoCreature(centerX + 50, centerY);  // Crée le Gecko Orbiter
    */

    // --- INIT COLLIDERS & LAYERS ---
    colliderLayer = make_shared<ColliderLayer>();
    float simWidth = 2048.0f;
    float scale = totalSceneWidth / simWidth;
    // On utilise une seule simulation qui couvre toute la hauteur de l'image (1472px)
    float simHeight = 1472.0f / scale;
    colliderLayer->setup(simWidth, simHeight, scale);

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

    // --- SETUP MACHINE LAYER ---
    machineLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP DIGGING CREATURE ---
    diggingCreature.setup(frontX + frontW/2.0f, 1000.0f, imgRush);

    // --- SETUP MACHINE AUTO ---
    machineAuto.setup(totalSceneWidth, 1472.0f);

    // --- SETUP CURTAIN (RIDEAU) ---
    curtain.setup(frontX + 200, 100, 600, 800, "garde.png");
    
    // --- SETUP PUYO LAYER ---
    puyoLayer.setup(simWidth, simHeight, scale, colliderLayer);

    // --- SETUP BUBBLE LAYER ---
    // Le layer de bulles utilise maintenant le collider principal, qui est déjà pleine hauteur.
    // L'offset de collision (dernier paramètre) reste à 0.0f.
    bubbleLayer.setup(simWidth, simHeight, scale, colliderLayer, 0.0f);
    
    // --- SETUP KANI LAYER ---
    kaniLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);

    // --- SETUP TEAA LAYER ---
    teaaLayer.setup(totalSceneWidth, 1472.0f, colliderLayer);

    // --- SETUP BALLET LAYER ---
    balletLayer.setup(totalSceneWidth, 1472.0f);
}

void Scene2DLayerManager::update(const ofVec2f& m, float time, bool isSpacePressed) {
    if (bDrawLightning) {
        lightningLayer.update(m.x, m.y, time);
    }

    if (bDrawCreatures) {
        creatureSystem.update(m, time);
        for(auto& c : cousinCons) c->update(m.x, m.y); // CousinCon doesn't use time in update signature yet, but uses internal time
        for(auto& h : halos) h->update(time);
    }

    if (bDrawWalker) {
        walkerLayer.update(m.x, m.y, time);
    }

    if (bDrawPoulpe) {
        if (isSpacePressed) {
            poulpeLayer.setTarget(m.x, m.y);
        }
        poulpeLayer.update(0, 0, time); // PoulpeLayer::update overrides BaseLayer, so it needs args
    }

    if (bDrawFish) {
        fishSchoolLayer.update(m.x, m.y, time); 
        
        // Les layers sont maintenant en pleine hauteur, on passe les coordonnées de la souris directement.
        if(ofGetMousePressed(0)) fishSchoolLayer.addSardine(m.x, m.y);
        if(ofGetMousePressed(2)) fishSchoolLayer.addShark(m.x, m.y);
    }

    if (bDrawSauteurs) {
        sauteursLayer.update(m.x, m.y, time); 
    }

    if (bDrawSlime) {
        slimeLayer.update(m.x, m.y, time);
    }

    if (bDrawPlants) {
        plantLayer.update(m.x, m.y, time); 
    }

    if (bDrawFlytraps) {
        flytrapLayer.update(m.x, m.y, time); 
    }

    if (bDrawGears) {
        gearLayer.update(m.x, m.y, time);
        
        if (bDrawFluidFloor) {
            for(const auto& s : gearLayer.squares) {
                // Conversion World -> Sim pour l'interaction
                float simX = s.pos.x / fluidFloorLayer.scale;
                float simY = s.pos.y / fluidFloorLayer.scale;
                float simVelX = s.vel.x / fluidFloorLayer.scale;
                float simVelY = s.vel.y / fluidFloorLayer.scale;
                fluidFloorLayer.addForce(simX, simY, simVelX * 0.5f, simVelY * 0.5f);
            }
        }
    }

    if (bDrawFluidFloor) {
        fluidFloorLayer.update(m.x, m.y); // FluidFloorLayer not BaseLayer
    }

    if (bDrawMachine) {
        machineLayer.update(m.x, m.y, time);
    }

    if (bDrawDigging) {
        diggingCreature.update(m.x, m.y); // DiggingCreature not BaseLayer
    }

    if (bDrawMachineAuto) {
        machineAuto.update(time); 
    }

    if (bDrawCurtain) {
        curtain.update(m.x, m.y); // CurtainCreature not BaseLayer
    }
    
    if (bDrawPuyo) {
        puyoLayer.update(m.x, m.y, time); 
    }

    if (bDrawBubbles) {
        bubbleLayer.update(m.x, m.y, time); // BubbleLayer uses time
    }
    
    if (bDrawKani) {
        kaniLayer.update(m.x, m.y, time);
    }

    if (bDrawTeaa) {
        teaaLayer.update(time);
    }

    if (bDrawBallet) {
        balletLayer.update(time);
    }
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

    if (bDrawLightning) {
        ofPushMatrix();
        ofTranslate(0, 0);
        lightningLayer.draw();
        ofPopMatrix();
    }

    if (bDrawSlime) {
        slimeLayer.draw();
    }

    if (bDrawSauteurs) {
        sauteursLayer.draw();
    }


    if (bDrawFish) {
        fishSchoolLayer.draw();
    }

    if (bDrawPoulpe) {
        poulpeLayer.draw();
    }

    if (bDrawWalker) {
        ofPushMatrix();
        ofTranslate(0, 1472 - 830); 
        walkerLayer.draw();
        ofPopMatrix();
    }

    if (bDrawPlants) {
        plantLayer.draw();
    }

    if (bDrawFlytraps) {
        flytrapLayer.draw();
    }

    if (bDrawGears) {
        gearLayer.draw();
    }

    if (bDrawFluidFloor) {
        fluidFloorLayer.draw(0, 0);
    }

    if (bDrawMachine) {
        machineLayer.draw();
    }

    if (bDrawDigging) {
        diggingCreature.draw();
    }

    if (bDrawMachineAuto) {
        machineAuto.draw();
    }

    if (bDrawCurtain) {
        curtain.draw();
    }
    
    if (bDrawPuyo) {
        puyoLayer.draw();
    }

    if (bDrawBubbles) {
        // Le dessin se fait maintenant directement dans les coordonnées du monde
        bubbleLayer.draw();
    }
    
    if (bDrawKani) {
        kaniLayer.draw();
    }
    
    if (bDrawTeaa) {
        teaaLayer.draw();
    }
    
    if (bDrawBallet) {
        balletLayer.draw();
    }

     if (bDrawColliders) {
        if(colliderLayer) colliderLayer->draw();
    }
}

void Scene2DLayerManager::keyPressed(int key, const ofVec2f& m) {
    // --- FENETRE 2D_SIDE ---
    switch (key) {
        case 'h': case 'H': bDrawCreatures = !bDrawCreatures; return; // display creature
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
        case 'a': case 'A': creatureSystem.addCousinCreature(m.x, m.y); break; // cousinHairWire
        case 'z': case 'Z': cousinCons.push_back(make_shared<CousinCon>(m.x, m.y, &imgConcombre)); break; // cousinCon
        case 's': case 'S': creatureSystem.addDoublePendulum(m.x, m.y); break;
        case 'y': case 'Y': halos.push_back(make_shared<HaloCreature>(m.x, m.y)); break;
        


        case '1': bDrawTeaa = !bDrawTeaa; teaaLayer.bActive = bDrawTeaa; if(bDrawTeaa) teaaLayer.generate(); 
        //bDrawCurtain = !bDrawCurtain; 
        break; // Generates new floor on toggle
        
        case '2': 
        creatureSystem.addFluidsCreature(m.x, m.y); 
        bDrawBallet = !bDrawBallet; balletLayer.bActive = bDrawBallet;
        break;
       
        case '3': creatureSystem.addSpringCreature(m.x, m.y); break;
        case '4': creatureSystem.addCreature(m.x, m.y); break; // Jelly 
        case '5': bDrawColliders = !bDrawColliders; break;
        case '6': creatureSystem.addDancingCreature(m.x, m.y); break;
        case '7': bDrawPuyo = !bDrawPuyo; break;
        case '8': bDrawBubbles = !bDrawBubbles; break;
        case '9': bDrawKani = !bDrawKani; break;
    }

    // --- LAYER Toggles ---
    switch (key) {
        case 'v': case 'V': bDrawFluidFloor = !bDrawFluidFloor; break;
        case 'i': case 'I': bDrawLightning = !bDrawLightning; break;
        case 'p': case 'P': // Toggles both Plants and Flytraps
            bDrawPlants = !bDrawPlants;
            bDrawFlytraps = !bDrawFlytraps;
            break;
        case 'm': case 'M': bDrawSlime = !bDrawSlime; break;
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
        case 'n': case 'N': bDrawWalker = !bDrawWalker; break;
        case 't': case 'T': // Digging creature layer
            bDrawDigging = !bDrawDigging;
            if (bDrawDigging && !diggingCreature.isEnabled()) diggingCreature.toggle();
            break;
        case 'w': case 'W': fluidFloorLayer.toggleBackground(); break;
    }
}

void Scene2DLayerManager::mousePressed(const ofVec2f& m, int button) {
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
    if (bDrawMachineAuto) machineAuto.mousePressed(m.x, m.y);
    if (bDrawCurtain) curtain.mousePressed(m.x, m.y);
    if (bDrawTeaa) teaaLayer.mousePressed(m.x, m.y, button);
}

void Scene2DLayerManager::mouseReleased(const ofVec2f& m, int button) {
    if(bDrawCreatures) {
        creatureSystem.onRelease(m.x, m.y);
        for(auto& c : cousinCons) c->onRelease(m.x, m.y);
    }
    if(bDrawCurtain) curtain.mouseReleased(m.x, m.y);
}

int Scene2DLayerManager::getSardineCount() {
    return fishSchoolLayer.getSardineCount();
}