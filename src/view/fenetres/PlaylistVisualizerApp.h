#pragma once
#include "ofMain.h"
#include "Scene360VideoPlayer.h"
#include <set>
#include <map>
#include <functional>
#include "PlaylistTooltipManager.h"

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

class PlaylistVisualizerApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void mousePressed(int x, int y, int button);
    void dragEvent(ofDragInfo dragInfo);
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;

    Scene360VideoPlayer* player = nullptr;
    bool* bDrawScene360VideoPtr = nullptr;
    
    ofApp* mainAppPtr = nullptr;
    ofRectangle viewBtns[4];
    bool viewHidden[4] = {false, false, false, false};
    ofRectangle moveV3Btn;
    
    ofRectangle wxcvbBtns[6];
    string wxcvbNames[6] = {"MAIN", "W:ROOM", "X:ZENI", "C:SCEN", "V:PREV", "B:BTNS"};
    int wxcvbKeys[6] = {0, 'w', 'x', 'c', 'v', 'b'}; // 0 signifie qu'il n'y a pas de raccourci clavier standard direct pour M
    
    ofRectangle focusBtns[6];
    string focusNames[6] = {"->MAIN", "->W", "->X", "->C", "->V", "->B"};
    
    ofRectangle gabBtns[3];
    
    RoomApp* roomApp = nullptr;
    vector<RoomToggleBtn> roomToggles;
    void setupRoomToggles();
    
    Scene2D_SIDE* scene2D = nullptr;
    vector<LayerToggle> layerToggles;
    void setupLayerToggles();

    bool bEnabled = true;
    void setEnabled(bool enable) { bEnabled = enable; }

    vector<CreatureButton> creatureButtons;
    int selectedCreatureIndex = 0;
    ofRectangle clearAllCreaturesBtn;
    void clearAllCreatures();
    
    vector<InteractiveButton> interactiveButtons;
    int selectedInteractiveIndex = -1;

    ofRectangle loopButtonRect;
    ofRectangle toggleButtonRect;
    ofRectangle simButtonRect;

    ofRectangle pauseAccordionBtn;
    bool bPauseAccordionOpen = false;
    vector<int> pauseOptions = {0, 30, 60, 150, 300, 600};
    vector<ofRectangle> pauseOptionRects;

    struct VisualNode {
        string name;
        ofVec2f pos;
    };

    std::map<string, VisualNode> nodes;
    vector<string> deadEnds;

    ofVec2f getTransformedMouse(int x, int y);

    // Navigation
    ofVec2f pan;
    float zoom;
    bool isSpacePressed;
    ofVec2f lastMouse;
    
    PlaylistTooltipManager tooltipManager;
};