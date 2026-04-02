#pragma once

#include "ofMain.h"
#include "ofJson.h"
#include "PlaylistTooltipManager.h"

class ofApp;

class PlaylistGeminiUI {
public:
    void setup();
    void update(ofApp* mainAppPtr);
    void draw();
    
    bool mousePressed(ofVec2f worldM, ofApp* mainAppPtr);
    bool mouseScrolled(ofVec2f worldM, float scrollY);
    bool keyPressed(int key);
    void unfocusAll();
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);
    string getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager);

    // --- Interface Gemini IA ---
    string apiKeyText = "";
    string themeText = "a surreal jukebox music machine";
    string promptVid1Text = "une bete poilu dans un marais enchanté, slow cinematic movement";
    string promptVid2Text = "A cinematic, haunting surealist video.";
    bool bApiKeyFocused = false;
    bool bThemeFocused = false;
    bool bPromptVid1Focused = false;
    bool bPromptVid2Focused = false;
    int themeScrollOffset = 0;
    int promptVid1ScrollOffset = 0;
    int promptVid2ScrollOffset = 0;
    
    ofRectangle apiKeyBox;
    ofRectangle themeBox;
    ofRectangle promptVid1Box;
    ofRectangle promptVid2Box;
    ofRectangle genRoomBtn;
    ofRectangle genVidLastBtn;
    ofRectangle genVid2LastBtn;
    ofRectangle modelAccordionBtn;
    ofRectangle imageSizeBtn;
    ofRectangle videoResBtn;
    ofRectangle genTextToRoomBtn;
    
    bool bModelAccordionOpen = false;
    int currentModelIndex = 0;
    int currentImageSizeIndex = 0;
    int currentVideoResIndex = 0;
    
    vector<string> modelOptions;
    vector<ofRectangle> modelOptionRects;
    vector<string> imageSizeOptions;
    vector<string> videoResOptions;
};