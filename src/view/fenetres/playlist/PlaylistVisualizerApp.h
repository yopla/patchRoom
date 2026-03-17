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

struct ActionButton {
    string name;
    ofRectangle rect;
    std::function<void()> action;
    bool continuous = false;
    std::function<bool()> getState = nullptr;
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
    
    vector<ActionButton> roomActionBtns;
    void setupRoomActionBtns();
    
    vector<ActionButton> globalActionBtns;
    void setupGlobalActionBtns();
    
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
    ofRectangle doubleSpeedBtnRect;
    ofRectangle muteBtnRect;
    ofRectangle crop106BtnRect;
    ofRectangle infinitePauseBtnRect;
    ofRectangle videoInfoBox;

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
    bool bIsDraggingPan = false;
    
    PlaylistTooltipManager tooltipManager;
    
    // Édition et Sauvegarde des positions des boutons
    ofRectangle editBtnRect;
    ofRectangle saveBtnRect;
    ofRectangle loadBtnRect;
    bool bEditMode = false;
    void saveButtonPositions();
    void loadButtonPositions();
    ofRectangle* findButtonAt(ofVec2f pos);
    
    bool bIsSelecting = false;
    bool bIsDraggingGroup = false;
    ofVec2f selectionStart, selectionEnd;
    vector<ofRectangle*> selectedRects;
    vector<ofVec2f> dragOffsets;
    vector<ofRectangle*> getAllInteractableRects();

    float saveFeedbackTimer = -10.0f;
    
    // --- Interface Gemini IA ---
    string apiKeyText = "";
    string themeText = "a surreal jukebox music machine";
    string promptVid1Text = "une bete poilu dans un marais enchanté, slow cinematic movement";
    string promptVid2Text = "A cinematic, haunting surealist video.";
    bool bApiKeyFocused = false;
    bool bThemeFocused = false;
    bool bPromptVid1Focused = false;
    bool bPromptVid2Focused = false;
    ofRectangle apiKeyBox;
    ofRectangle themeBox;
    ofRectangle promptVid1Box;
    ofRectangle promptVid2Box;
    ofRectangle genRoomBtn;
    ofRectangle genVidLastBtn;
    ofRectangle genVid2LastBtn;
    
    ofRectangle modelAccordionBtn;
    bool bModelAccordionOpen = false;
    vector<string> modelOptions = {
        "gemini-3.1-flash-image-preview",
        "gemini-3-pro-image-preview",
        "nano-banana-pro-preview"
    };
    int currentModelIndex = 0;
    vector<ofRectangle> modelOptionRects;
    ofRectangle imageSizeBtn;
    int currentImageSizeIndex = 0;
    vector<string> imageSizeOptions = {"DEFAULT", "2K", "4K"};
    ofRectangle videoResBtn;
    int currentVideoResIndex = 0;
    vector<string> videoResOptions = {"DEFAULT", "4k"};
    ofRectangle genTextToRoomBtn;
    
    // Etats locaux de l'UI pour P et M (fallback si les variables ne sont pas accessibles via atmosphere)
    bool uiStateSphereP = false;
    bool uiStateDiscoM = false;
};