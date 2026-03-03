#include "OscManager.h"
#include "ofApp.h"
#include "RoomApp.h" // Nécessaire pour app->roomApp
#include "ProjectionSystem.h" // Nécessaire pour app->roomApp->projection

#include "ViewApp.h" // Nécessaire pour app->viewApps[i]->moveWindow

void OscManager::setup(string host, int sendPort, int receivePort){
    sender.setup(host, sendPort);
    receiver.setup(receivePort);
    ofLog() << "OscManager listening on port " << receivePort << " | sending to " << host << ":" << sendPort;
}

void OscManager::sendFrameNum(ofApp* app){
    ofxOscMessage m;
    m.setAddress("/frame");
    m.addIntArg(ofGetFrameNum());
    sender.sendMessage(m, false);

    if(app) {
        ofxOscMessage mLocal;
        mLocal.setAddress("/localFrame");
        mLocal.addIntArg((int)(app->localTime * (float)APP_FPS));
        sender.sendMessage(mLocal, false);
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
    sendFrameNum(app);

    // 2. Envoi conditionnel de l'état du survol
    checkAndSendHoverState(app);

    // 3. Traitement des messages reçus
    while(receiver.hasWaitingMessages()){
        ofxOscMessage mess;
        receiver.getNextMessage(mess);
        
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

        // Commande: /time [float]
        else if(address == "/time"){
            if(mess.getArgType(0) == OFXOSC_TYPE_FLOAT || mess.getArgType(0) == OFXOSC_TYPE_INT32) {
                // On interprète l'argument comme un numéro de FRAME et on convertit en secondes
                app->oscTime = mess.getArgAsFloat(0) / (float)APP_FPS;
            }
        }

        // Mouse position
        else if(address == "/mouse/position"){
            // float mouseXf = mess.getArgAsFloat(0);
            // float mouseYf = mess.getArgAsFloat(1);
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
    }
}