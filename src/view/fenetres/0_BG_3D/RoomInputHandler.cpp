#include "RoomInputHandler.h"
#include "RoomApp.h" // Include the full header here for implementation details

void RoomInputHandler::setup(RoomApp* owner) {
    app = owner;
}

void RoomInputHandler::update() {
    if (!app || !app->bEnabled) return;

    updateKeyStates();
    handleCameraAndProjection();
    updateFluidRingInteraction();
    updateLightFlyInteraction(); // <--- AJOUT
    updateLiquidSphereInteraction(); // <--- AJOUT
    updateJellySphereInteraction(); // <--- AJOUT
    
    // This was in RoomApp::update()
    app->projection.checkMouseIntersection(app->camGlobal);
    app->cursorSquare.updateRaycast(app->camGlobal, app->walls);
}

void RoomInputHandler::updateKeyStates() {
    bLeftShiftPressed  = ofGetKeyPressed(OF_KEY_LEFT_SHIFT);
    bRightShiftPressed = ofGetKeyPressed(OF_KEY_RIGHT_SHIFT);
    bSpacePressed = ofGetKeyPressed(' ');
    bTabPressed = ofGetKeyPressed(OF_KEY_TAB);
    bLPressed = app->bLockCameraCenter;
}

void RoomInputHandler::handleCameraAndProjection() {
    // This logic was in RoomApp::draw()
    // 1. Gestion de la Caméra (EasyCam)
    if(bLeftShiftPressed || bSpacePressed || bTabPressed) {
        app->camGlobal.disableMouseInput();
    } else {
        app->camGlobal.enableMouseInput();
    }

    // Blocage au centre pour s'aligner avec les textures (Vue panoramique)
    if(bLPressed) {
        app->camGlobal.setDistance(0);
        app->camGlobal.setPosition(0, 600, 0); // Centre de la pièce / Rig
    }

    // 2. Gestion du Projecteur
    if(ofGetMousePressed(0)) { 
        if (bLeftShiftPressed || bRightShiftPressed) {
            app->projection.updateTarget(app->camGlobal, app->walls);
        }
        if (bSpacePressed) {
            app->projection.updateTarget2(app->camGlobal, app->walls);
        }
        if (bTabPressed) {
            app->projection.updateTarget3(app->camGlobal, app->walls);
        }
    }
}

void RoomInputHandler::updateFluidRingInteraction() {
    // This logic was in RoomApp::update()
    if (!app->bFluidRingEnabled && app->fluidRing.globalAlpha <= 0.0f) return;

    float localX = -1000.0f;
    float localY = -1000.0f;

    // Note: bFluidRingEnabled est une variable membre publique (bool) à ajouter à la classe RoomApp.
    ofVec3f rayOrigin = app->camGlobal.getPosition();
    ofVec3f rayDir = app->camGlobal.screenToWorld(ofVec3f(ofGetMouseX(), ofGetMouseY(), 0)) - rayOrigin;
    rayDir.normalize();

    float R = app->fluidRing.radius;
    float A = rayDir.x * rayDir.x + rayDir.z * rayDir.z;
    float B = 2 * (rayOrigin.x * rayDir.x + rayOrigin.z * rayDir.z);
    float C = rayOrigin.x * rayOrigin.x + rayOrigin.z * rayOrigin.z - R * R;
    float delta = B*B - 4*A*C;

    if(delta >= 0) {
        float t1 = (-B - sqrt(delta)) / (2*A);
        float t2 = (-B + sqrt(delta)) / (2*A);
        
        float t = -1;
        if (t1 > 0) t = t1;
        else if (t2 > 0) t = t2;

        if(t > 0) {
            ofVec3f hit = rayOrigin + rayDir * t;
            if(hit.y <= app->fluidRing.height && hit.y >= -app->fluidRing.bottomExt) {
                float angle = atan2(hit.z, hit.x);
                if(angle < 0) angle += TWO_PI;
                float u = angle / TWO_PI;
                float v = (app->fluidRing.height - hit.y) / (app->fluidRing.height + app->fluidRing.bottomExt);
                
                localX = u * app->fluidRing.fluid.width;
                localY = v * app->fluidRing.fluid.height;
            }
        }
    }
    
    app->fluidRing.fluid.update(localX, localY);
}

void RoomInputHandler::updateLightFlyInteraction() {
    if (!app->bLightFlyRingEnabled) return;

    float u = -1.0f;
    float v = -1.0f;

    ofVec3f rayOrigin = app->camGlobal.getPosition();
    ofVec3f rayDir = app->camGlobal.screenToWorld(ofVec3f(ofGetMouseX(), ofGetMouseY(), 0)) - rayOrigin;
    rayDir.normalize();

    float R = app->lightFlyRing.radius;
    float A = rayDir.x * rayDir.x + rayDir.z * rayDir.z;
    float B = 2 * (rayOrigin.x * rayDir.x + rayOrigin.z * rayDir.z);
    float C = rayOrigin.x * rayOrigin.x + rayOrigin.z * rayOrigin.z - R * R;
    float delta = B*B - 4*A*C;

    if(delta >= 0) {
        float t1 = (-B - sqrt(delta)) / (2*A);
        float t2 = (-B + sqrt(delta)) / (2*A);
        
        float t = -1;
        if (t1 > 0) t = t1;
        else if (t2 > 0) t = t2;

        if(t > 0) {
            ofVec3f hit = rayOrigin + rayDir * t;
            if(hit.y <= app->lightFlyRing.height && hit.y >= -app->lightFlyRing.bottomExt) {
                cursor3DPos = hit;
                float angle = atan2(hit.z, hit.x);
                if(angle < 0) angle += TWO_PI;
                u = angle / TWO_PI;
                v = (app->lightFlyRing.height - hit.y) / (app->lightFlyRing.height + app->lightFlyRing.bottomExt);
            }
        }
    }
    
    app->lightFlyRing.setInteraction(u, v);
}

void RoomInputHandler::updateLiquidSphereInteraction() {
    if (!app->bDrawLiquidSphere) return;

    float localX = -1000.0f;
    float localY = -1000.0f;

    ofVec3f rayOrigin = app->camGlobal.getPosition();
    ofVec3f rayDir = app->camGlobal.screenToWorld(ofVec3f(ofGetMouseX(), ofGetMouseY(), 0)) - rayOrigin;
    rayDir.normalize();

    ofVec3f center = app->liquidSphereRing.center;
    float R = app->liquidSphereRing.radius;
    ofVec3f oc = rayOrigin - center;

    float a = rayDir.lengthSquared(); // RayDir normalisé
    float b = 2.0f * oc.dot(rayDir);
    float c = oc.lengthSquared() - R * R;
    float delta = b * b - 4 * a * c;

    if(delta >= 0) {
        float t1 = (-b - sqrt(delta)) / (2.0f * a);
        float t2 = (-b + sqrt(delta)) / (2.0f * a);
        
        float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
        if(t > 0) {
            ofVec3f hit = rayOrigin + rayDir * t;
            ofVec3f localHit = (hit - center).getNormalized();
            
            // 1. Inversion de la rotation Y pour s'aligner avec le rendu visuel
            localHit.rotate(90.0f, ofVec3f(0, 1, 0));
            
            float v = acos(localHit.y) / PI;
            float phi = atan2(localHit.z, localHit.x);
            if(phi < 0) phi += TWO_PI;
            float u = phi / TWO_PI;
                
            localX = u * app->liquidSphereRing.fluid.width;
            localY = v * app->liquidSphereRing.fluid.height;
        }
    }
    
    app->liquidSphereRing.fluid.update(localX, localY);
}

void RoomInputHandler::updateJellySphereInteraction() {
    jellyLocalX = -1000.0f;
    jellyLocalY = -1000.0f;

    if (!app->bDrawJellySphere) return;

    ofVec3f rayOrigin = app->camGlobal.getPosition();
    ofVec3f rayDir = app->camGlobal.screenToWorld(ofVec3f(ofGetMouseX(), ofGetMouseY(), 0)) - rayOrigin;
    rayDir.normalize();

    ofVec3f center = app->jellySphereRing.center;
    float R = app->jellySphereRing.radius;
    ofVec3f oc = rayOrigin - center;

    float a = rayDir.lengthSquared(); 
    float b = 2.0f * oc.dot(rayDir);
    float c = oc.lengthSquared() - R * R;
    float delta = b * b - 4 * a * c;

    if(delta >= 0) {
        float t1 = (-b - sqrt(delta)) / (2.0f * a);
        float t2 = (-b + sqrt(delta)) / (2.0f * a);
        
        float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
        if(t > 0) {
            ofVec3f hit = rayOrigin + rayDir * t;
            ofVec3f localHit = (hit - center).getNormalized();
            localHit.rotate(90.0f, ofVec3f(0, 1, 0));
            
            float v = acos(localHit.y) / PI;
            float phi = atan2(localHit.z, localHit.x);
            if(phi < 0) phi += TWO_PI;
            float u = phi / TWO_PI;
                
            jellyLocalX = u * app->jellySphereRing.fbo.getWidth();
            jellyLocalY = v * app->jellySphereRing.fbo.getHeight();
        }
    }
}

void RoomInputHandler::keyPressed(int key) {
    if (!app) return;
    // This logic was in RoomApp::keyPressed()
    if(key == '1') {
        app->bDrawUndulatingFloor = !app->bDrawUndulatingFloor;
    }
    if(key == '2') {
        app->bOscillateRoom = !app->bOscillateRoom;
    }
    if(key == '3') { // <--- AJOUT
        app->bDrawKraken = !app->bDrawKraken;
        if(app->bDrawKraken) {
            app->kraken.start(app->localTime);
        }
    }
    if(key == '4') { // <--- AJOUT
        app->bDrawExternalKraken = !app->bDrawExternalKraken;
        if (app->bDrawExternalKraken) {
            app->externalKraken.start(app->localTime);
        }
    }
    if(key == '5') { // <--- AJOUT GENERATION 360
        app->generateEquirectangularImage();
    }
    if(key == '6') { // <--- AJOUT TOGGLE CLOUD RING
        app->bDrawCloudRing = !app->bDrawCloudRing;
    }
    if(key == '7') { // <--- AJOUT TOGGLE LIQUID SPHERE
        app->bDrawLiquidSphere = !app->bDrawLiquidSphere;
    }
    if(key == '8') { // <--- AJOUT TOGGLE LECTEUR VIDEO 360
        app->bDrawScene360Video = !app->bDrawScene360Video;
        app->scene360VideoPlayer.toggle();
    }
    if(key == '9') { // <--- AJOUT TOGGLE LOOP VIDEO 360
        app->scene360VideoPlayer.toggleLoopMode();
    }
    if(key == '0') { // <--- AJOUT TOGGLE JELLY SPHERE
        app->bDrawJellySphere = !app->bDrawJellySphere;
        if (!app->bDrawJellySphere) {
            app->jellySphereRing.clearJellies(); // Nettoie quand on ferme
        }
    }

    if(key == 'l' || key == 'L') {
        app->bLockCameraCenter = !app->bLockCameraCenter;
        if (!app->bLockCameraCenter) {
            app->camGlobal.setDistance(4000);
            app->camGlobal.setPosition(2000, 2500, 3000);
            app->camGlobal.lookAt(ofVec3f(0, 600, 0));
        }
    }

    if(key == 'g' || key == 'G') {
        if (app->wallAlpha > 50.0f) {
            app->wallAlpha = 0.0f;
        } else {
            app->wallAlpha = 100.0f;
        }
    }
    if(key == 'f' || key == 'F') app->bDrawBeam = !app->bDrawBeam;
    if(key == 'b' || key == 'B') app->bDrawAtmosphere = !app->bDrawAtmosphere;
    if(key == 't' || key == 'T') app->bUseTexture = !app->bUseTexture; // Changé de L vers T pour libérer la touche L
    
    if(key == 'r' || key == 'R') {
        app->camGlobal.setDistance(4000);
        app->camGlobal.setPosition(2000, 2500, 3000);
        app->camGlobal.lookAt(ofVec3f(0, 600, 0));
    }
    if(key == 'a' || key == 'A') app->bShowRoof = !app->bShowRoof;
    if(key == 'u' || key == 'U') app->respire = !app->respire;
    if(key == 'k' || key == 'K') app->bDrawRipples = !app->bDrawRipples;
    if(key == 'v' || key == 'V') app->bDrawWorms = !app->bDrawWorms;
    if(key == 'w' || key == 'W') app->bDrawWingedWorms = !app->bDrawWingedWorms;

    // Touche 's' pour la transparence du curseur
    if(key == 's' || key == 'S') {
        // Note: bLowAlpha est une variable membre publique (bool) à ajouter à la classe CursorSquareSystem.
        app->cursorSquare.bLowAlpha = !app->cursorSquare.bLowAlpha;
    }
    // Touche 'c' pour activer/désactiver l'interaction avec le FluidRing
    if(key == 'c' || key == 'C') {
        // Note: bFluidRingEnabled est une variable membre publique (bool) à ajouter à la classe RoomApp.
        app->bFluidRingEnabled = !app->bFluidRingEnabled;
        app->fluidRing.setTargetAlpha(app->bFluidRingEnabled ? 1.0f : 0.0f);
    }
    
    // --- COMMANDES LIGHT FLY RING ---
    if(key == 'h' || key == 'H') {
        app->bLightFlyRingEnabled = !app->bLightFlyRingEnabled;
    }
    if(key == 'x' || key == 'X') {
        app->lightFlyRing.clearLights();
    }
    if(key == 'y' || key == 'Y') {
        // Ajoute une lumière à la position d'interaction actuelle
        if(app->bLightFlyRingEnabled && app->lightFlyRing.isInteracting) {
            // On récupère les coordonnées UV calculées dans updateLightFlyInteraction
            // via une astuce ou en recalculant, mais ici on utilise la pos stockée dans le ring
            app->lightFlyRing.addLightAt(app->lightFlyRing.interactPos.x / app->lightFlyRing.fbo.getWidth(), app->lightFlyRing.interactPos.y / app->lightFlyRing.fbo.getHeight());
            lastCreatedHalo3DPos = cursor3DPos;
           // app->lightFlyRing.addLightAt(0.1,0);
        }
    }

    // Delegation
    app->projection.keyPressed(key);
    app->atmosphere.keyPressed(key);
}

void RoomInputHandler::keyReleased(int key) {
    // No specific logic here, but the method is available for future use.
}

void RoomInputHandler::mouseDragged(int x, int y, int button) {
    // No specific logic here, EasyCam handles it.
}

void RoomInputHandler::mousePressed(int x, int y, int button) {
    if (!app || button != 0) return;

    if (app->bDrawCloudRing) {
        // --- RAYCAST CLOUD RING (SPHERE) ---
        ofVec3f rayOrigin = app->camGlobal.getPosition();
        ofVec3f rayDir = app->camGlobal.screenToWorld(ofVec3f(x, y, 0)) - rayOrigin;
        rayDir.normalize();

        ofVec3f center = app->cloudRing.center;
        float R = app->cloudRing.radius;
        ofVec3f oc = rayOrigin - center;

        float a = rayDir.lengthSquared(); // Vaut 1 car rayDir est normalisé
        float b = 2.0f * oc.dot(rayDir);
        float c = oc.lengthSquared() - R * R;
        float delta = b * b - 4 * a * c;

        if(delta >= 0) {
            float t1 = (-b - sqrt(delta)) / (2.0f * a);
            float t2 = (-b + sqrt(delta)) / (2.0f * a);
            
            float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
            if(t > 0) {
                ofVec3f hit = rayOrigin + rayDir * t;
                ofVec3f localHit = (hit - center).getNormalized();
                
                // 1. Inversion de la rotation Y de 90° ajoutée au rendu
                localHit.rotate(90.0f, ofVec3f(0, 1, 0));
                
                // Conversion du point 3D en coordonnées UV de la sphère
                float v = acos(localHit.y) / PI;
                float phi = atan2(localHit.z, localHit.x);
                if(phi < 0) phi += TWO_PI;
                float u = phi / TWO_PI;
                    
                // On lance l'onde de choc UV à la position cliquée !
                app->cloudRing.addRipple(u, v);
            }
        }
    }
    if (app->bDrawJellySphere && jellyLocalX >= 0) {
        app->jellySphereRing.mousePressed(jellyLocalX, jellyLocalY);
    }
}

void RoomInputHandler::mouseReleased(int x, int y, int button) {
    if (!app || button != 0) return;
    if (app->bDrawJellySphere) {
        app->jellySphereRing.mouseReleased(jellyLocalX, jellyLocalY);
    }
}

void RoomInputHandler::dragEvent(ofDragInfo dragInfo) {
    if (!app) return;
    // This logic was in RoomApp::dragEvent()
    if(dragInfo.files.size() > 0){
        string file = dragInfo.files[0];
        bool sphereActive = false;
        
        ofFile f(file);
        if(f.isDirectory()) {
            if(app->scene360VideoPlayer.isSimulating32Videos()) {
                app->scene360VideoPlayer.toggleSimulate32Videos();
            }
            app->scene360VideoPlayer.startPlaylist(file);
            app->bDrawScene360Video = true; // S'assure que la vidéo est allumée visuellement
            ofLogNotice("RoomInputHandler") << "Nouvelle playlist chargee via drag&drop : " << file;
            return;
        }

        if(app->bDrawAtmosphere && app->atmosphere.bShow360) {
            app->atmosphere.loadTexture(file);
            sphereActive = true;
        }
        if(app->bDrawCloudRing) {
            app->cloudRing.loadTexture(file);
            sphereActive = true;
        }
        if(app->bDrawLiquidSphere) {
            app->liquidSphereRing.loadTexture(file);
            sphereActive = true;
        }
        if(app->bDrawJellySphere) {
            app->jellySphereRing.loadTexture(file);
            sphereActive = true;
        }
        
        if(!sphereActive) {
            ofLogNotice("RoomInputHandler") << "pas de sphere active";
        }
    }
}