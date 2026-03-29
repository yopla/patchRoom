#pragma once
#include "ofMain.h"

class VolumetricLayerManager {
public:
    void draw(float targetWidth, float targetHeight);
    void buildMesh(float targetWidth, float targetHeight);
    void reset();

    // Renvoient un message d'avertissement en cas de besoin, ou une chaîne vide.
    string loadDepthMap(const string& path, const ofImage& fallbackImg, float targetWidth, float targetHeight);
    string loadLayerVolum(const string& path, const ofImage& fallbackImg, float targetWidth, float targetHeight);

    bool bLayerVolumActive = false;
    bool bDepthMapActive = false;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotSpeed = 1.5f;
    float extrusionIntensity = 300.0f;

    ofImage layerVolumImg;
    ofImage depthMapImg;
private:
    ofVboMesh volumetricMesh;
};