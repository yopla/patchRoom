#pragma once

#include "ofMain.h"
#include "ofJson.h"
#include "PlaylistTooltipManager.h"
#include <functional>
#include <vector>
#include <string>

class Scene2D_SIDE;
class RoomApp;
class ofApp;

struct LayerToggle {
    string name;
    bool* valuePtr;
    ofRectangle rect;
};

struct CreatureButton {
    string name;
    ofRectangle rect;
};

struct InteractiveButton {
    string name;
    ofRectangle rect;
};

struct RoomToggleBtn {
    string name;
    ofRectangle rect;
    std::function<bool()> getState;
    std::function<void()> toggle;
};

struct ActionButton {
    string name;
    ofRectangle rect;
    std::function<void()> action;
    bool continuous = false;
    std::function<bool()> getState = nullptr;
};

class PlaylistControlsUI {
public:
    void setupLayerToggles(Scene2D_SIDE* scene2D);
    void setupRoomToggles(RoomApp* roomApp, ofApp* mainAppPtr);
    void setupRoomActionBtns(RoomApp* roomApp);
    void setupGlobalActionBtns(ofApp* mainAppPtr);
    void clearAllCreatures(Scene2D_SIDE* scene2D);
    
    void draw(ofApp* mainAppPtr);
    bool mousePressed(ofVec2f worldM, Scene2D_SIDE* scene2D);
    void handleContinuousActions(ofVec2f worldM);
    string getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager);
    
    void saveSettings(ofJson& pt);
    void loadSettings(const ofJson& pt);
    
    vector<ofRectangle*> getInteractableRects();
    ofRectangle* findButtonAt(ofVec2f pos);

    vector<LayerToggle> layerToggles;
    vector<CreatureButton> creatureButtons;
    int selectedCreatureIndex = 0;
    ofRectangle clearAllCreaturesBtn;
    ofRectangle undoCreatureBtn;
    ofRectangle resetEatMapBtn;
    ofRectangle resetCollidersBtn;
    
    vector<InteractiveButton> interactiveButtons;
    int selectedInteractiveIndex = -1;

    vector<InteractiveButton> mainBrushButtons;
    int selectedMainBrushIndex = -1;

    vector<RoomToggleBtn> roomToggles;
    vector<ActionButton> roomActionBtns;
    vector<ActionButton> globalActionBtns;
    
    bool uiStateSphereP = false;
    bool uiStateDiscoM = false;
    bool uiStateRecPreview = false;
    bool uiStateRecCanvas = false;
};