#pragma once
#include "ofMain.h"
#include <deque>
#include "AnnexeTooltipManager.h"
#include "PlaylistSearchBar.h"
#include "PlaylistTextNote.h"
#include "PlaylistVisualFrame.h"
#include "AnnexeControlsUI.h"

class ofApp;

class AnnexePlayerApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseMoved(int x, int y) override;
    void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;

    void setEnabled(bool enable) { bEnabled = enable; }

    ofApp* mainAppPtr = nullptr;
    bool bEnabled = true;
    ofVec2f pan;
    float zoom;
    bool isSpacePressed = false;
    ofVec2f lastMouse;
    bool bIsDraggingPan = false;

    AnnexeTooltipManager tooltipManager;
    PlaylistSearchBar searchBar;

    AnnexeControlsUI controlsUI;

    ofRectangle editBtnRect, saveBtnRect, loadBtnRect;
    ofRectangle addNoteBtnRect, addFrameBtnRect, searchBtnRect;
    ofRectangle focusAnnexeBtnRect;

    bool bEditMode = false;

    ofRectangle cameraPresetBtns[5];
    ofVec2f presetPans[5];
    float presetZooms[5];

    ofRectangle windowPresetBtns[5];
    ofRectangle windowPresets[5][8]; // 8 fenêtres gérées
    bool windowPresetSaved[5] = {false};
    shared_ptr<ofAppBaseWindow> getAppWindow(int index);

    vector<shared_ptr<PlaylistTextNote>> textNotes;
    shared_ptr<PlaylistTextNote> editingNote = nullptr;
    vector<shared_ptr<PlaylistVisualFrame>> visualFrames;
    shared_ptr<PlaylistVisualFrame> resizingFrame = nullptr;
    ofRectangle* resizingRect = nullptr;

    std::deque<ofJson> undoStack;
    void saveUndoState(); void undo(); ofJson serializeState(); void deserializeState(const ofJson& pt);
    void saveButtonPositions(); void loadButtonPositions(); ofRectangle* findButtonAt(ofVec2f pos);
    
    bool bIsSelecting = false; bool bIsDraggingGroup = false; ofVec2f selectionStart, selectionEnd;
    vector<ofRectangle*> selectedRects; vector<ofVec2f> dragOffsets;
    vector<ofRectangle*> getAllInteractableRects();
    vector<SearchableButton> getAllSearchableButtons();

    float saveFeedbackTimer = -10.0f;
    string hoveredTooltip = "";
    ofVec2f getTransformedMouse(int x, int y);
};