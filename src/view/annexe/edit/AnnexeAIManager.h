#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

class AnnexeAiManager {
public:
    // Ces méthodes effectuent l'inférence et sauvegardent l'image.
    // Elles retournent le message de succès ou d'avertissement pour l'UI.
    string generateDepthMapAI(const ofImage& img);
    string generateDepthMapDepthAnything(const ofImage& img);
    string generateSAMCollider(const ofImage& img);
};