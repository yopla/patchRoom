#pragma once
#include "ofxOsc.h"

// Forward declaration pour éviter les inclusions circulaires
class ofApp; 

class OscManager {
public:
    void setup(std::string host, int sendPort, int receivePort);
    void update(ofApp* app);

    // Fonctions d'envoi spécifiques
    void sendFrameNum(ofApp* app);
    void sendHoverState(bool state, float radius, float elevation, float azimuth);

private:
    void checkAndSendHoverState(ofApp* app);

    ofxOscSender sender;
    ofxOscReceiver receiver;
    bool lastHoverState = false; // Pour ne pas spammer l'OSC
};