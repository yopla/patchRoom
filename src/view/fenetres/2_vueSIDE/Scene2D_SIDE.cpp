#include "Scene2D_SIDE.h"
#include "PlantLayer.h"

//--------------------------------------------------------------
void Scene2D_SIDE::setup() {
    ofSetRandomSeed(42);
    localTime = 0.0f;
    
    // Chargement des images de fond
    imgJar.load("JAR.jpg"); imgFront.load("FRONT.jpg"); imgCour.load("COUR.jpg"); imgBack.load("BACK.jpg");
    imgSol.load("SOL.jpg"); imgTopJar.load("TOP_JAR.jpg"); imgTopCour.load("TOP_COUR.jpg");

    // Optimisation : Redimensionner les images à la taille des FBO pour alléger le rendu
    if(imgJar.isAllocated()) imgJar.resize(wJar, 784);
    if(imgFront.isAllocated()) imgFront.resize(wFront, 1472);
    if(imgCour.isAllocated()) imgCour.resize(wJar, 1072);
    if(imgBack.isAllocated()) imgBack.resize(wFront, 1472);
    if(imgSol.isAllocated()) imgSol.resize(wSol, hSol);
    if(imgTopJar.isAllocated()) imgTopJar.resize(wTopJar, hTopJar);
    if(imgTopCour.isAllocated()) imgTopCour.resize(wTopCour, hTopCour);

    // Calcul des positions X
    srcX_Jar   = 0;
    srcX_Front = wJar;
    srcX_Cour  = wJar + wFront;
    srcX_Back  = wJar + wFront + wJar;

    totalSceneWidth = srcX_Back + wFront; 

    // --- SETUP CREATURE SYSTEM ---
    creatureSystem.setup();
    float centerX = srcX_Jar + wJar / 2.0f;
    float centerY = 1080.0f; 

    creatureSystem.addDoublePendulum(centerX+400, centerY-200);
    creatureSystem.addWancoCreature(centerX+400, centerY-200);
    creatureSystem.addFluidsCreature(centerX, centerY-700);
    creatureSystem.addCreature(2400 * 0.33, 736);
    creatureSystem.addRipple(2900 * 0.66, 736);
    creatureSystem.addDancingCreature(srcX_Front + wFront/2, 600);
    creatureSystem.addSpringCreature(srcX_Front + wFront/2, 712);
    creatureSystem.addGekoCreature(srcX_Front + wFront/2 - 300, 600);
    creatureSystem.addGekoCreature(centerX, centerY);       // Crée le Gecko Standard
    creatureSystem.addGekoCreature(centerX - 50, centerY);  // Crée le Gecko Agile
    creatureSystem.addGekoCreature(centerX + 50, centerY);  // Crée le Gecko Orbiter

    // --- INIT COLLIDERS & LAYERS ---
    colliderLayer = make_shared<ColliderLayer>();
    float simWidth = 2048.0f;
    float scale = totalSceneWidth / simWidth;
    float simHeight = 900.0f / scale;
    colliderLayer->setup(simWidth, simHeight, scale);

    // --- SETUP SUBSYSTEMS ---
    slimeLayer.setup(totalSceneWidth, 900.0f);
    slimeLayer.setCollider(colliderLayer);
    slimeLayer.setScale(scale);

    sauteursLayer.setup(totalSceneWidth, 900.0f, colliderLayer);
    fishSchoolLayer.setup(totalSceneWidth, 830.0f, colliderLayer);
    poulpeLayer.setup(totalSceneWidth, 1472.0f);
    walkerLayer.setup(totalSceneWidth, 830.0f);
    walkerLayer.setScale(2.0f);

    // --- SETUP GEARS ---
    gearLayer.setup(totalSceneWidth, 1472.0f);

// Ou plus simple, on passe la largeur totale, et le layer gère sa hauteur interne de 1500
lightningLayer.setup(totalSceneWidth, 1500.0f);
lightningLayer.setScale(1); // Utilisez le même scale que pour SlimeLayer ou SauteursLayer

    // --- SETUP PLANTS ---
    plantLayer.setup(totalSceneWidth, 1472.0f); // On les pose au niveau du sol (1472)

    // --- SETUP FLYTRAPS ---
    flytrapLayer.setup(totalSceneWidth, 1472.0f);

    // --- SETUP FLUID FLOOR ---
    // On prend toute la largeur, et une hauteur de 800px en bas (2x)
    // Résolution X réduite (512) pour perf, Y (128)
    fluidFloorLayer.setup(totalSceneWidth, 800.0f, 512, 128);


    // Allocation FBOs
    fboJar.allocate(wJar, 784, GL_RGBA);
    fboFront.allocate(wFront, 1472, GL_RGBA);
    fboCour.allocate(wJar, 1072, GL_RGBA);
    fboBack.allocate(wFront, 1472, GL_RGBA);
    fboSol.allocate(wSol, hSol, GL_RGBA); 
    fboTopJar.allocate(wTopJar, hTopJar, GL_RGBA);
    fboTopCour.allocate(wTopCour, hTopCour, GL_RGBA);

    // View Navigation Init
    viewZoom = (float)ofGetWidth() / totalSceneWidth * 0.5f;
    viewPan.x = (ofGetWidth() - totalSceneWidth * viewZoom) / 2.0f;
    viewPan.y = (ofGetHeight() - hMax * viewZoom) / 2.0f;

    // --- Waypoints Balle ---
    float xFront = srcX_Front + wFront/2.0f; 
    float xCour  = srcX_Cour  + wJar/2.0f;   
    float xBack  = srcX_Back  + wFront/2.0f; 
    float xJar   = srcX_Jar   + wJar/2.0f;   

    ofVec2f pTopJar(xJar, -112);        
    ofVec2f pJar(xJar, 1080);           
    ofVec2f pFront(xFront, 736);        
    ofVec2f pSol(xFront, 2656);         
    ofVec2f pCour(xCour, 936);          
    ofVec2f pTopCour(xCour, -104);      
    ofVec2f pBack(xBack, 736);

    waypoints.clear();
    waypoints.push_back(pTopJar); waypoints.push_back(pJar);    
    waypoints.push_back(pFront);  waypoints.push_back(pSol);    
    waypoints.push_back(pFront);  waypoints.push_back(pCour);   
    waypoints.push_back(pTopCour); waypoints.push_back(pCour);   
    waypoints.push_back(pBack);   
}

//--------------------------------------------------------------
void Scene2D_SIDE::update() {
    // 1. HARD PAUSE : Si désactivé, on coupe tout calcul CPU
    if(!bEnabled) return;

    // 2. Gestion du Temps Local (Pour reprendre l'animation exactement où elle était)
    float fpsRec = 60.0f;
    localTime += 1.0f / fpsRec;

    ofVec2f m = getTransformedMouse();

    // 3. MISE A JOUR CONDITIONNELLE DES LAYERS (Performance Toggles)

if (bDrawLightning) {
    lightningLayer.update(m.x, m.y);
}

    // Touche H : Créatures (Ondes, Jellys)
    if (bDrawCreatures) {
        creatureSystem.update(m);
    }

    if (bDrawWalker) {
        // On mappe la souris Y relative à la zone du bas (1472 - 830)
        // Pour que si la souris est en haut, le walker ne la suive pas forcément en Y (il reste au sol),
        // mais il a besoin du X.
        walkerLayer.update(m.x, m.y);
    }

    // Touche J : Poulpe
    if (bDrawPoulpe) {
        poulpeLayer.setTarget(m.x, m.y);
        poulpeLayer.update(); // PoulpeLayer n'a pas changé de signature (pas hérité encore ?) ou gère ses propres args
    }

    // Touche K : Poissons (Lourd en CPU)
    if (bDrawFish) {
        // FishSchoolLayer a probablement aussi changé si tu as appliqué BaseLayer partout
        // Si ça compile pour fish, c'est bon, sinon ajoute m.x, m.y
        fishSchoolLayer.update(); 
        
        // Interaction Poissons (déplacée ici pour être synchro avec l'affichage)
        float layerY = m.y - (1472 - 830); 
        if(layerY > 0 && layerY < 830) {
            if(ofGetMousePressed(0)) fishSchoolLayer.addSardine(m.x, layerY); 
            if(ofGetMousePressed(2)) fishSchoolLayer.addShark(m.x, layerY); 
        }
    }

    // Touche L : Sauteurs
    if (bDrawSauteurs) {
        // CORRECTION 1 : On passe m.x et m.y
        sauteursLayer.update(m.x, m.y); 
    }

    // Touche M : Slime
    if (bDrawSlime) {
        // Interaction Slime
        /* Note : Si tu as intégré l'interaction DANS SlimeLayer::update comme proposé précédemment,
           tu peux supprimer ce bloc if(MousePressed) ici.
           Sinon, garde-le. Dans le doute, je laisse l'appel update avec arguments.
        */
        /* if (ofGetMousePressed(0)) {
            float offsetY = 1472.0f - 900.0f; 
            float localSlimeY = m.y - offsetY;
            slimeLayer.pour(m.x, localSlimeY, ofRandom(-2, 2), 5.0); 
        } 
        */
        
        // CORRECTION 2 : On passe m.x et m.y
        slimeLayer.update(m.x, m.y);
    }

    // Touche P : Plantes
    if (bDrawPlants) {
        plantLayer.update(m.x, m.y);
    }

    // Touche O : Flytraps
    if (bDrawFlytraps) {
        flytrapLayer.update(m.x, m.y);
    }

    // Touche E : Gears
    if (bDrawGears) {
        gearLayer.update(m.x, m.y);
        
        // INTERACTION GEARS -> FLUID
        if (bDrawFluidFloor) {
            float fluidTopY = 1472.0f - 800.0f; // Position Y du haut du fluide
            
            for(const auto& s : gearLayer.squares) {
                // Si le carré est dans la zone du fluide
                if(s.pos.y > fluidTopY && s.pos.y < 1472.0f) {
                    // Position locale dans le fluide
                    float localX = s.pos.x;
                    float localY = s.pos.y - fluidTopY;
                    
                    // On ajoute une force proportionnelle à la vitesse du carré
                    // On divise par un facteur pour ne pas faire exploser la simu
                    fluidFloorLayer.addForce(localX, localY, s.vel.x * 0.5f, s.vel.y * 0.5f);
                }
            }
        }
    }

    // Touche V : Fluid Floor
    if (bDrawFluidFloor) {
        // On passe la souris relative au layer.
        // Le layer est dessiné à Y = 1472 - 800 = 672.
        fluidFloorLayer.update(m.x, m.y - (1472.0f - 800.0f));
    }

    // 4. Animation Balle (Toujours active sauf si app désactivée)
    if (waypoints.size() > 1) {
        float totalDuration = 12.0f; 
        
        // UTILISATION DU TEMPS LOCAL
        float time = localTime; 
        
        float cycleTime = fmod(time, totalDuration * 2.0f);
        float val = (cycleTime < totalDuration) ? cycleTime : (2.0f * totalDuration - cycleTime);
        float progress = val / totalDuration;
        
        float scaledProgress = progress * (waypoints.size() - 1);
        int idx = (int)scaledProgress; 
        int nextIdx = idx + 1;         
        if (nextIdx >= waypoints.size()) nextIdx = waypoints.size() - 1;
        
        ballPos = waypoints[idx].getInterpolated(waypoints[nextIdx], scaledProgress - idx);
    }

    // 5. Captures FBO (Toujours actif pour le décor de fond)
    captureSection(fboFront, srcX_Front, 0, imgFront, true);
    captureSection(fboBack,  srcX_Back,  0, imgBack,  true);
    captureSection(fboJar,   srcX_Jar, 688, imgJar, true);
    captureSection(fboCour,  srcX_Cour, 400, imgCour, true);

    // TOP JAR
    captureSection(fboSol, srcX_Front, 1472, imgSol, true);
    captureSection(fboTopJar, srcX_Jar, -912, imgTopJar, true);
    captureSection(fboTopCour, srcX_Cour, -608, imgTopCour, true);
}

//--------------------------------------------------------------
void Scene2D_SIDE::drawDynamicElements() {
    ofVec2f m = getTransformedMouse(); 

    // 1. CREATURES (Touche H)
    if (bDrawCreatures) {
        creatureSystem.draw(m);
    }

    // VISUALISER LE POINT D'ANCRAGE DE L ECLAIR EN ATTENTE
    if (bDrawLightning && bLightningHasStart) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(255, 50, 50); // Rouge
        ofSetLineWidth(2);
        ofDrawCircle(lightningStartPos.x, lightningStartPos.y, 10); // Petit cercle cible        
        // Ligne de prévisualisation vers la souris (optionnel)
        ofSetColor(255, 255, 255, 100);
        ofDrawLine(lightningStartPos, getTransformedMouse());
        ofPopStyle();
    }

    if (bDrawLightning) {
    ofPushMatrix();
    // Positionnement vertical :
    // Si la hauteur simulée est 1500 et qu'on veut que ça touche le sol (1472)
    // On aligne le bas.
    // 1472 est le sol dans le repère global.
    // Le layer fait 1500 de haut (simulé).
    // On translate pour caler le bas du layer sur le bas de la scène
    
   // float layerPixelHeight = 1500.0f * lightningLayer.getScale();    
    // ofTranslate(0, 1472 - layerPixelHeight); // Option A: Posé au sol
    ofTranslate(0, 0); // Option B: Part du plafond (0)
    
    lightningLayer.draw();
    ofPopMatrix();
    }

    // 2. SLIME (Touche M)
    if (bDrawSlime) {
        ofPushMatrix();
        ofTranslate(0, 1472 - 900);
        slimeLayer.draw();
        ofPopMatrix();
    }

    // 3. SAUTEURS & MURS (Touche L)
    if (bDrawSauteurs) {
        ofPushMatrix();
        ofTranslate(0, 1472 - 900);
        if(colliderLayer) colliderLayer->draw();
        sauteursLayer.draw();
        ofPopMatrix();
    }

    // 4. POISSONS (Touche K)
    if (bDrawFish) {
        ofPushMatrix();
        ofTranslate(0, 1472 - 900);
        fishSchoolLayer.draw();
        ofPopMatrix();
    }

    // 5. POULPE (Touche J)
    if (bDrawPoulpe) {
        poulpeLayer.draw();
    }

    // 6. WALKER (Touche N)
    if (bDrawWalker) {
        ofPushMatrix();
        // On le place dans la même zone que les poissons/slime (bas de l'écran)
        // 1472 (hauteur totale) - 830 (hauteur simu walker)
        ofTranslate(0, 1472 - 830); 
        walkerLayer.draw();
        ofPopMatrix();
    }

    // 7. PLANTES (Touche P)
    if (bDrawPlants) {
        plantLayer.draw();
    }

    // 8. FLYTRAPS (Touche O)
    if (bDrawFlytraps) {
        flytrapLayer.draw();
    }

    // 10. GEARS (Touche E) - Juste avant le Fluid Floor pour être derrière
    if (bDrawGears) {
        gearLayer.draw();
    }

    // 9. FLUID FLOOR (Touche V)
    if (bDrawFluidFloor) {
        // Dessiné tout en bas
        fluidFloorLayer.draw(0, 1472.0f - 800.0f);
    }

    // 6. BALLE (Toujours visible)
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofDrawCircle(ballPos.x, ballPos.y, 80); 
    ofPopStyle();

}

//--------------------------------------------------------------
void Scene2D_SIDE::draw() {
    if(!bEnabled) {
        ofBackground(0);
        return;
    }

    ofBackground(0);
    ofPushMatrix();
    ofTranslate(viewPan.x, viewPan.y);
    ofScale(viewZoom);
    ofSetColor(255);

    // Dessin debug de tous les FBOs assemblés
    fboJar.draw(srcX_Jar, hMax - 784);
    fboFront.draw(srcX_Front, hMax - 1472);
    fboCour.draw(srcX_Cour, hMax - 1072);
    fboBack.draw(srcX_Back, hMax - 1472);

    fboTopJar.draw(srcX_Jar, hMax - 784 - 1600); 
    fboSol.draw(srcX_Front, hMax); 
    fboTopCour.draw(srcX_Cour, hMax - 1072 - 1008);

    ofSetColor(255, 255, 255, 50);
    ofDrawLine(0, hMax, totalSceneWidth, hMax);
    ofPopMatrix();

    // Stats
    int nSardines = fishSchoolLayer.getSardineCount();
    string stats = "ECOSYSTEME (Toggle H, J, K, L, M):\n";
    stats += "Creatures [H]: " + ofToString(bDrawCreatures) + "\n";
    stats += "Poulpe    [J]: " + ofToString(bDrawPoulpe) + "\n";
    stats += "Poissons  [K]: " + ofToString(bDrawFish) + " (" + ofToString(nSardines) + ")\n";
    stats += "Sauteurs  [L]: " + ofToString(bDrawSauteurs) + "\n";
    stats += "Slime     [M]: " + ofToString(bDrawSlime) + "\n";
    stats += "Plantes   [P]: " + ofToString(bDrawPlants);
    stats += "\nFlytraps  [O]: " + ofToString(bDrawFlytraps);
    stats += "\nFluidFloor[V]: " + ofToString(bDrawFluidFloor); 
    stats += "\nGears     [E]: " + ofToString(bDrawGears);

    ofDrawBitmapStringHighlight(stats, 20, 30); 
}


//--------------------------------------------------------------
void Scene2D_SIDE::captureSection(ofFbo& targetFbo, float worldX, float worldTopY, ofImage& img, bool bDrawDynamics) {
    targetFbo.begin();
        ofClear(0, 0, 0, 0);
        
        if (bShowTextures && img.isAllocated()) {
            ofSetColor(255, 255, 255, 180);
            img.draw(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
        } else {
            ofNoFill(); ofSetColor(100);
            ofDrawRectangle(0, 0, targetFbo.getWidth(), targetFbo.getHeight());
            ofFill();
        }
        
        if (bDrawDynamics) {
            ofSetColor(255);
            ofPushMatrix();
                ofTranslate(-worldX, -worldTopY);
                drawDynamicElements();
            ofPopMatrix();
        }
        
    targetFbo.end();
}

//--------------------------------------------------------------
// INPUTS
//--------------------------------------------------------------
void Scene2D_SIDE::mouseScrolled(int x, int y, float sx, float sy) {
    ofVec2f mouseBeforeZoom = getTransformedMouse();
    viewZoom = ofClamp(viewZoom + sy * 0.01, 0.01, 2.0);
    ofVec2f mouseAfterZoom = (ofVec2f(x, y) - viewPan) / viewZoom;
    viewPan += (mouseAfterZoom - mouseBeforeZoom) * viewZoom;
}

void Scene2D_SIDE::mousePressed(int x, int y, int button) { 
    lastMouse.set(x, y); 
    
    if(!isSpacePressed) {
        ofVec2f m = getTransformedMouse();
        
        // Interaction Créatures (seulement si actif)
        if(bDrawCreatures) creatureSystem.onPress(m.x, m.y);
       if(bDrawLightning) {
        
        if (!bLightningHasStart) {
            // PREMIER CLIC : On marque le point de départ
            lightningStartPos = m;
            bLightningHasStart = true;
            
        } else {
            // SECOND CLIC : On tire de Start vers M (Souris actuelle)
            lightningLayer.trigger(lightningStartPos, m);
            
            // On reset pour le prochain éclair
            bLightningHasStart = false; 
        }
    }
        // Note: Les interactions Slime et Fish sont gérées dans update() 
        // pour garantir qu'elles ne se produisent que si le layer est actif et mis à jour.
    }
}

void Scene2D_SIDE::mouseReleased(int x, int y, int button) {
    ofVec2f m = getTransformedMouse();
    if(bDrawCreatures) creatureSystem.onRelease(m.x, m.y);
}

void Scene2D_SIDE::mouseDragged(int x, int y, int button) {
    if (isSpacePressed) {
        viewPan += (ofVec2f(x, y) - lastMouse);
    }
    lastMouse.set(x, y);
}

void Scene2D_SIDE::keyPressed(int key) {
    if (key == ' ') isSpacePressed = true; 
    if (key == 'g' || key == 'G') bShowTextures = !bShowTextures;
    
    if (key == 'r' || key == 'R') {
        viewZoom = (float)ofGetWidth() / totalSceneWidth * 0.5f;
        viewPan.x = (ofGetWidth() - totalSceneWidth * viewZoom) / 2.0f;
        viewPan.y = (ofGetHeight() - hMax * viewZoom) / 2.0f;
    }
    

    // Commandes Créatures
    if(key == 'a' || key == 'A') {
        ofVec2f m = getTransformedMouse();
        creatureSystem.addCousinCreature(m.x, m.y);
    }
    // Touche Y : Halo Creature
    if(key == 'y' || key == 'Y') {
        ofVec2f m = getTransformedMouse();
        creatureSystem.addHalo(m.x, m.y);
    }
    if(key == 'd' || key == 'D') creatureSystem.removeLast();
    if(key == 'c' || key == 'C') creatureSystem.clear();

    // --- TOGGLES LAYERS ---
    if(key == 'h' || key == 'H') bDrawCreatures = !bDrawCreatures;
    
    if(key == 'j' || key == 'J') bDrawPoulpe    = !bDrawPoulpe;
    if(key == 'k' || key == 'K') bDrawFish      = !bDrawFish;
    if(key == 'l' || key == 'L') bDrawSauteurs  = !bDrawSauteurs;
    if(key == 'm' || key == 'M') bDrawSlime     = !bDrawSlime;
    if(key == 'n' || key == 'N') bDrawWalker = !bDrawWalker;
    if(key == 'i' || key == 'I') bDrawLightning = !bDrawLightning;
    if(key == 'p' || key == 'P') bDrawPlants    = !bDrawPlants;
    if(key == 'o' || key == 'O') bDrawFlytraps  = !bDrawFlytraps;
    if(key == 'v' || key == 'V') bDrawFluidFloor = !bDrawFluidFloor;
    if(key == 'e' || key == 'E') {
        bDrawGears = !bDrawGears;
        if(!bDrawGears) gearLayer.squares.clear();
    }
    if(key == 'w' || key == 'W') fluidFloorLayer.toggleBackground();


}

void Scene2D_SIDE::keyReleased(int key) {
    if (key == ' ') isSpacePressed = false; 
}

ofVec2f Scene2D_SIDE::getTransformedMouse() {
    float mx = (ofGetMouseX() - viewPan.x) / viewZoom;
    float my = (ofGetMouseY() - viewPan.y) / viewZoom;
    return ofVec2f(mx, my);
}