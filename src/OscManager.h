#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class ofApp; // Forward declaration pour éviter les dépendances circulaires

class OscManager {
public:
    void setup(string host, int sendPort, int receivePort);
    void update(ofApp* app);
    
    // Méthodes d'envoi spécifiques
    void sendHoverState(bool state, float radius, float elevation, float azimuth);

private:
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    void sendFrameNum();
};