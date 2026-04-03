#include "OscManager.h"
#include "ofApp.h"
#include "RoomApp.h" // Nécessaire pour app->roomApp
#include "ProjectionSystem.h" // Nécessaire pour app->roomApp->projection
#include "ViewApp.h" // Nécessaire pour app->viewApps[i]->moveWindow

// Déclaration de la fonction externe définie dans ButtonWindow.cpp
void setButtonOscState(int id, bool state);


void OscManager::setup(string host, int sendPort, int receivePort){
    sender.setup(host, sendPort);
    receiver.setup(receivePort);
    ofLog() << "OscManager listening on port " << receivePort << " | sending to " << host << ":" << sendPort;
}


// Fonction centralisée de traitement des messages reçus
void OscManager::processOscMessage(ofxOscMessage& mess, ofApp* app) {
    string address = mess.getAddress();

    // Commande: /roomApp [0 ou 1] OU "on"/"off"
    if(address == "/roomApp"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT){
            state = mess.getArgAsFloat(0) > 0;
        }
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING){
            state = (mess.getArgAsString(0) == "on");
        }
        
        app->bDrawRoom = state;
        if(app->roomApp) app->roomApp->setEnabled(state);
    }

    // Commande: /scene2D [0 ou 1]
    else if(address == "/scene2D"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        app->bDrawScene2D = state;
        if(app->scene2D) app->scene2D->setEnabled(state);
    }

    // Commande: /scene2DZenit [0 ou 1]
    else if(address == "/scene2DZenit"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        app->bDrawZenit = state;
        if(app->sceneZenit) app->sceneZenit->setEnabled(state);
    }

    // Commande: /scene2D/fish [0 ou 1]
    else if(address == "/scene2D/fish"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->scene2D) app->scene2D->layerManager.bDrawFish = state;
    }

    // Commande: /scene2D/fluidFloor [0 ou 1]
    else if(address == "/scene2D/fluidFloor"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->scene2D) app->scene2D->layerManager.bDrawFluidFloor = state;
    }

    // Commande: /scene2D/ballet [0 ou 1]
    else if(address == "/scene2D/ballet"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->scene2D) {
            app->scene2D->layerManager.bDrawBallet = state;
            app->scene2D->layerManager.balletLayer.bActive = state;
        }
    }

    // Commande: /scene360/playlist [folderName]
    else if(address == "/scene360/playlist") {
        if(mess.getNumArgs() > 0 && app->roomApp) {
            string folder = "";
            if(mess.getArgType(0) == OFXOSC_TYPE_STRING) {
                folder = mess.getArgAsString(0);
            }

            // Si le dossier est "0" ou vide, on arrête le lecteur
            if (folder == "0" || folder.empty()) {
                app->roomApp->scene360VideoPlayer.stop();
                app->roomApp->bDrawScene360Video = false;
            } else {
                // Sinon, on lance la playlist depuis ce dossier
                app->roomApp->scene360VideoPlayer.startPlaylist(folder);
                app->roomApp->bDrawScene360Video = true;
            }
        }
    }
    // Commande: /FluidRing [0 ou 1]
    else if(address == "/FluidRing"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->roomApp) {
            app->roomApp->bFluidRingEnabled = state;
            app->roomApp->fluidRing.setTargetAlpha(app->roomApp->bFluidRingEnabled ? 1.0f : 0.0f);

        }
    }

    // Commande: /LightFlyRing [0 ou 1]
    else if(address == "/LightFlyRing"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->roomApp) {
            app->roomApp->bLightFlyRingEnabled = state;
        }
    }

    // Commande: /LightFlyRing/addLight [u] [v]
    // u et v sont des valeurs normalisées (généralement entre 0 et 1)
    else if(address == "/LightFlyRing/addLight"){
        if(mess.getNumArgs() >= 2 && app->roomApp){
            float u = (mess.getArgType(0) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(0) : mess.getArgAsFloat(0);
            float v = (mess.getArgType(1) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(1) : mess.getArgAsFloat(1);
            app->roomApp->lightFlyRing.addLightAt(u, v);
        }
    }

    // Commande: /CloudRing [0 ou 1]
    else if(address == "/CloudRing"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->roomApp) {
            app->roomApp->bDrawCloudRing = state;
        }
    }

    // Commande: /Tuyau3D [0 ou 1]
    else if(address == "/Tuyau3D"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->roomApp) {
            app->roomApp->bDrawTuyau = state;
        }
    }

    // Commande: /LiquidSphere [0 ou 1]
    else if(address == "/LiquidSphere"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->roomApp) {
            app->roomApp->bDrawLiquidSphere = state;
        }
    }





    
    // Commande: /time [float]
    else if(address == "/time"){
        if(mess.getArgType(0) == OFXOSC_TYPE_FLOAT || mess.getArgType(0) == OFXOSC_TYPE_INT32) {
            // On interprète l'argument comme un numéro de FRAME (plus de conversion)
            app->oscTime = mess.getArgAsFloat(0);
        }
    }

    // Commande: /pause [0 ou 1] OU "on"/"off" (toggle si pas d'argument)
    else if(address == "/pause"){
        bool state = !app->bGlobalPause; // Toggle par défaut
        if(mess.getNumArgs() > 0){
            if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) {
                state = mess.getArgAsFloat(0) > 0;
            } else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) {
                state = (mess.getArgAsString(0) == "on");
            }
        }
        
        app->bGlobalPause = state;
        if(app->bGlobalPause) {
            app->oscTime = app->localTime; // Synchronisation du temps lors de la pause
        }
    }

    // ViewApps positions
    else if(address == "/viewApp1/pos"){
        if(app->viewApps.size() > 0 && mess.getNumArgs() == 2) app->viewApps[0]->moveWindow(mess.getArgAsInt(0), mess.getArgAsInt(1));
    }
    else if(address == "/viewApp2/pos"){
        if(app->viewApps.size() > 1 && mess.getNumArgs() == 2) app->viewApps[1]->moveWindow(mess.getArgAsInt(0), mess.getArgAsInt(1));
    }
    else if(address == "/viewApp3/pos"){
        if(app->viewApps.size() > 2 && mess.getNumArgs() == 2) app->viewApps[2]->moveWindow(mess.getArgAsInt(0), mess.getArgAsInt(1));
    }
    else if(address == "/viewApp4/pos"){
        if(app->viewApps.size() > 3 && mess.getNumArgs() == 2) app->viewApps[3]->moveWindow(mess.getArgAsInt(0), mess.getArgAsInt(1));
    }

    // Commande: /button/[id] [state] (ex: /button/1 1 ou /button/12 on)
    else if(address.find("/button/") == 0){
        string idStr = address.substr(8); // Longueur de "/button/"
        int id = ofToInt(idStr);
        bool state = false;
        if(mess.getNumArgs() > 0){
            if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) 
                state = mess.getArgAsFloat(0) > 0;
            else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) 
                state = (mess.getArgAsString(0) == "on");
        }
        setButtonOscState(id, state);
    }
    






    
    // Commande Spéciale: Ajout de créature
    else if(address == "/MainCanevas/addRandomCreature"){
        if(mess.getNumArgs() >= 2){
            float x = (mess.getArgType(0) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(0) : mess.getArgAsFloat(0);
            float y = (mess.getArgType(1) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(1) : mess.getArgAsFloat(1);
            app->creatureSystem.addRandomCreature(x, y);
        }
    }


    // Commande: /scene2D/addCousinSauteur
    else if(address == "/scene2D/addCousinSauteur"){
        if(mess.getNumArgs() >= 2 && app->scene2D){
            float x = (mess.getArgType(0) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(0) : mess.getArgAsFloat(0);
            float y = (mess.getArgType(1) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(1) : mess.getArgAsFloat(1);
            app->scene2D->layerManager.creatureSystem.addCousinSauteur(x, y);
        }
    }

    // Commande: /scene2D/addCousinCon
    else if(address == "/scene2D/addCousinCon"){
        if(mess.getNumArgs() >= 2 && app->scene2D){
            float x = (mess.getArgType(0) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(0) : mess.getArgAsFloat(0);
            float y = (mess.getArgType(1) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(1) : mess.getArgAsFloat(1);
            app->scene2D->layerManager.addCousinCon(x, y);
        }
    }

    // Commande: /scene2D/addHalo
    else if(address == "/scene2D/addHalo"){
        if(mess.getNumArgs() >= 2 && app->scene2D){
            float x = (mess.getArgType(0) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(0) : mess.getArgAsFloat(0);
            float y = (mess.getArgType(1) == OFXOSC_TYPE_INT32) ? (float)mess.getArgAsInt(1) : mess.getArgAsFloat(1);
            app->scene2D->layerManager.addHalo(x, y);
        }
    }

    // Commande: /show360 [0 ou 1]
    else if(address == "/show360"){
        bool state = false;
        if(mess.getArgType(0) == OFXOSC_TYPE_INT32 || mess.getArgType(0) == OFXOSC_TYPE_FLOAT) state = mess.getArgAsFloat(0) > 0;
        else if(mess.getArgType(0) == OFXOSC_TYPE_STRING) state = (mess.getArgAsString(0) == "on");

        if(app->roomApp) {
            app->roomApp->atmosphere.bShow360 = state;
            if(state) {
                app->roomApp->atmosphere.bShowSphere = false;
                app->roomApp->atmosphere.bShowDiscoBall = false;
            }
        }
    }
}


void OscManager::sendHoverState(bool state, float radius, float elevation, float azimuth){
    ofxOscMessage mHover;
    mHover.setAddress("/plan/hover");
    mHover.addIntArg(state ? 1 : 0);
    mHover.addFloatArg(radius);
    mHover.addFloatArg(elevation); 
    mHover.addFloatArg(azimuth);
    sender.sendMessage(mHover, false);
}

void OscManager::checkAndSendHoverState(ofApp* app) {
    // On s'assure que tout est initialisé
    if(!app || !app->roomApp) return;

    // On récupère l'état actuel du survol via les nouvelles méthodes d'accès
    bool isHovering = app->roomApp->projection.isPlanColleHovered();
    
    // On envoie le message seulement si l'état a changé (optimisation)
    if(isHovering != lastHoverState) {
        float radius = app->roomApp->projection.getPlanColleRadius();
        float elevation = app->roomApp->projection.getPlanColleElevation();
        float azimuth = app->roomApp->projection.getPlanColleAzimuth();
        
        sendHoverState(isHovering, radius, elevation, azimuth);
       
        lastHoverState = isHovering; // On met à jour l'état pour la prochaine frame
    }
}





void OscManager::update(ofApp* app){
    // 1. Envoi systématique du numéro de frame
    if(app && app->localTime != lastLocalTime) {
        sendFrameNum(app);
        lastLocalTime = app->localTime;
    }

    // 2. Envoi conditionnel de l'état du survol
    checkAndSendHoverState(app);

    // CORRECTION : localTime est maintenant en frames, on arrondit juste
    int currentFrame = (int)round(app->localTime);

    // 3. Traitement des messages reçus
    while(receiver.hasWaitingMessages()){
        ofxOscMessage mess;
        receiver.getNextMessage(mess);
        
        if(mess.getAddress() == "/timed") {
            // Format: /timed [frame] [address] [args...]
            if(mess.getNumArgs() >= 2) {
                // Robustesse : Accepte Int ou Float pour le numéro de frame
                int targetFrame = (mess.getArgType(0) == OFXOSC_TYPE_INT32) ? mess.getArgAsInt(0) : (int)mess.getArgAsFloat(0);
                string targetAddr = mess.getArgAsString(1);
                
                // Reconstruction du message cible
                ofxOscMessage mStored;
                mStored.setAddress(targetAddr);
                for(int i=2; i<mess.getNumArgs(); i++){
                    if(mess.getArgType(i) == OFXOSC_TYPE_INT32) mStored.addIntArg(mess.getArgAsInt(i));
                    else if(mess.getArgType(i) == OFXOSC_TYPE_FLOAT) mStored.addFloatArg(mess.getArgAsFloat(i));
                    else if(mess.getArgType(i) == OFXOSC_TYPE_STRING) mStored.addStringArg(mess.getArgAsString(i));
                }
                
                // Deduplication: On vérifie si le message existe déjà pour cette frame
                bool duplicate = false;
                for(auto& it : bufferOscTimed) {
                    if(it.first == targetFrame && areMessagesEqual(it.second, mStored)) {
                        duplicate = true;
                        break;
                    }
                }
                
                if(!duplicate) {
                    bufferOscTimed.push_back({targetFrame, mStored});
                }
            }
        } else {
            // Message immédiat
            processOscMessage(mess, app);
        }
    }
    
    // 4. Traitement du buffer Timed
    for(auto it = bufferOscTimed.begin(); it != bufferOscTimed.end(); ) {
        int frame = it->first;
        
        // MODIFICATION : On utilise <= pour gérer les sauts de frames (lag ou dt > 1 frame)
        // Si on utilisait < pour supprimer, on perdrait les événements sautés.
        if(frame <= currentFrame) {
            processOscMessage(it->second, app);
            it = bufferOscTimed.erase(it);
        }
        else {
            // Message futur -> On garde
            ++it;
        }
    }
}



// Helper pour comparer deux messages OSC (Deduplication)
bool OscManager::areMessagesEqual(const ofxOscMessage& a, const ofxOscMessage& b) {
    if(a.getAddress() != b.getAddress()) return false;
    if(a.getNumArgs() != b.getNumArgs()) return false;
    for(int i=0; i<a.getNumArgs(); i++) {
        if(a.getArgType(i) != b.getArgType(i)) return false;
        if(a.getArgType(i) == OFXOSC_TYPE_INT32) {
            if(a.getArgAsInt(i) != b.getArgAsInt(i)) return false;
        } else if(a.getArgType(i) == OFXOSC_TYPE_FLOAT) {
            if(abs(a.getArgAsFloat(i) - b.getArgAsFloat(i)) > 0.0001f) return false;
        } else if(a.getArgType(i) == OFXOSC_TYPE_STRING) {
            if(a.getArgAsString(i) != b.getArgAsString(i)) return false;
        }
    }
    return true;
}



void OscManager::sendFrameNum(ofApp* app){
    ofxOscMessage m;
    m.setAddress("/frame");
    m.addIntArg(ofGetFrameNum());
    sender.sendMessage(m, false);

    if(app) {
        ofxOscMessage mLocal;
        mLocal.setAddress("/localFrame");
        // CORRECTION : Envoi direct de la frame locale
        mLocal.addIntArg((int)round(app->localTime));
        sender.sendMessage(mLocal, false);
    }
}
