#pragma once
#include "ofMain.h"
#include "Scene2DLayerManager.h"
#include "ofxOpenCv.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

class Scene2DSamController {
public:
    void setup();
    void toggle(const ofImage& overlayImg, string& warningMessage, float& warningEndTime);
    void drawPreview(float viewZoom, const ofImage& overlayImg, const ofVec2f& currentMouseImgSpace);
    void drawUI();

    bool mousePressed(int x, int y, ofVec2f worldM, const ofImage& overlayImg, Scene2DLayerManager& layerManager, string& warningMessage, float& warningEndTime);
    void mouseReleased(ofVec2f worldM, const ofImage& overlayImg, string& warningMessage, float& warningEndTime);

    void convertToPhysicsBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime);
    void convertToGearsBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime);
    void convertToSoftBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime);
    void convertToAliveBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime);
    
    void reset();
    void saveSegmentation(string& warningMessage, float& warningEndTime);
    void runInference(const ofImage& overlayImg, string& warningMessage, float& warningEndTime);

    bool isActive() const { return bIsActive; }
    bool isDragging() const { return bIsDragging; }

private:
    bool bIsActive = false;
    bool bDrawBoxMode = false;
    bool bDrawPointMode = false;
    
    vector<ofVec2f> points;
    vector<int> labels;
    ofImage previewMask;
    bool bMaskGenerated = false;

    bool bIsDragging = false;
    ofVec2f dragStart; // Dans l'espace de l'image (0 à overlayWidth)
    
    ofRectangle saveBtn, resetBtn, pointModeBtn, boxModeBtn, toPhysicBtn, toGearsBtn, toSoftBtn, toAliveBtn;
};