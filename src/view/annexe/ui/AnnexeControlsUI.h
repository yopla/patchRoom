#pragma once
#include "ofMain.h"
#include "ofJson.h"
#include "AnnexeTooltipManager.h"

class ofApp;

class AnnexeControlsUI {
public:
    void setup();
    void draw(ofApp* mainAppPtr);
    bool mousePressed(ofVec2f worldM, ofApp* mainAppPtr);
    void handleContinuousActions(ofVec2f worldM, ofApp* mainAppPtr);
    string getTooltip(ofVec2f worldM, AnnexeTooltipManager& tooltipManager);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);

    ofRectangle soloAnnexeBtnRect;
    ofRectangle saveFrameBtnRect;
    ofRectangle genAIBtnRect;
    ofRectangle genDepthAnythingBtnRect;
    ofRectangle genSAMBtnRect;
    ofRectangle rippleBtnRect;
    ofRectangle recordVideoBtnRect;
    ofRectangle samControlBtnRect;
    
    ofRectangle layerVolumBtnRect;
    ofRectangle depthMapBtnRect;
    ofRectangle resetDepthMapBtnRect;
    ofRectangle rotUpBtnRect;
    ofRectangle rotDownBtnRect;
    ofRectangle rotLeftBtnRect;
    ofRectangle rotRightBtnRect;
    ofRectangle resetRotBtnRect;
    bool bIsSoloActive = false;
    
    ofRectangle patteuBtnRect;
    ofRectangle patteuDropRect;
    ofRectangle patteuResetBtnRect;
    ofRectangle patteuIntensitySliderRect;
    ofRectangle patteuHardnessSliderRect;
    ofRectangle patteuBrushSizeSliderRect;
    
    ofRectangle deuPatteuBtnRect;
    ofRectangle deuPatteuDropFgRect;
    ofRectangle deuPatteuDropBgRect;
    ofRectangle deuPatteuResetFgBtnRect;
    ofRectangle deuPatteuResetBgBtnRect;
    ofRectangle deuPatteuIntensitySliderRect;
    ofRectangle deuPatteuHardnessSliderRect;
    ofRectangle deuPatteuRevealSliderRect;
    ofRectangle deuPatteuBrushSizeSliderRect;

private:
    bool saved_bDrawMain = true;
    bool saved_bDrawRoom = true;
    bool saved_bDrawZenit = false;
    bool saved_bDrawScene2D = true;
    bool saved_bPreviewPaused = false;
    bool saved_bDrawButtons = true;
};