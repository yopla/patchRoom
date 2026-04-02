#pragma once
#include "ofMain.h"
#include "Scene360VideoPlayer.h"
#include <set>
#include <map>
#include <functional>
#include "PlaylistTooltipManager.h"
#include "PlaylistGeminiUI.h"
#include "PlaylistNodeGraph.h"
#include "PlaylistPlayerUI.h"
#include "PlaylistControlsUI.h"
#include "PlaylistWindowControlsUI.h"
#include "PlaylistTextureUI.h"
#include "PlaylistSearchBar.h"
#include "PlaylistHistoryUI.h"
#include "PlaylistViewsUI.h"
#include "PlaylistWinPosUI.h"
#include "PlaylistTextNote.h"
#include "PlaylistVisualFrame.h"
#include "ImageGraphPlayer.h"
#include "PlaylistSequenceUI.h"
#include <deque>
#include <memory>

class Scene2D_SIDE;
class RoomApp;
class ofApp;

class PlaylistVisualizerApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void mousePressed(int x, int y, int button) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void mouseMoved(int x, int y) override;
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
    Scene2D_SIDE* scene2D = nullptr;

    bool bEnabled = true;
    void setEnabled(bool enable) { bEnabled = enable; }

    PlaylistNodeGraph nodeGraph;

    ofVec2f getTransformedMouse(int x, int y);

    // Navigation
    ofVec2f pan;
    float zoom;
    bool isSpacePressed;
    ofVec2f lastMouse;
    bool bIsDraggingPan = false;
    
    PlaylistTooltipManager tooltipManager;
    
    // --- Nouvelles Zones de Drag & Drop ---
    ofRectangle diagramDropZone;
    
    ofRectangle imageGraphDropZone;
    ImageGraphPlayer imageGraphPlayer;
    
    // Édition et Sauvegarde des positions des boutons
    ofRectangle editBtnRect;
    ofRectangle saveBtnRect;
    ofRectangle loadBtnRect;
    ofRectangle addNoteBtnRect;
    ofRectangle addFrameBtnRect;
    ofRectangle searchBtnRect;
    ofRectangle histBtnRect;
    ofRectangle vuesBtnRect;
    ofRectangle posWinBtnRect;
    bool bEditMode = false;

    shared_ptr<ofAppBaseWindow> getAppWindow(int index);
    
    ofRectangle focusAnnexesBtnRect;
    ofRectangle toggleAnnexesBtnRect;
    ofRectangle createAnnexeBtnRect;
    bool bAnnexesHidden = false;

    vector<shared_ptr<PlaylistTextNote>> textNotes;
    shared_ptr<PlaylistTextNote> editingNote = nullptr;
    
    vector<shared_ptr<PlaylistVisualFrame>> visualFrames;
    shared_ptr<PlaylistVisualFrame> resizingFrame = nullptr;
    ofRectangle* resizingRect = nullptr;
    
    std::deque<ofJson> undoStack;
    void saveUndoState(string actionName = "Action");
    void undo();
    ofJson serializeState();
    void deserializeState(const ofJson& pt);
    
    void saveButtonPositions();
    void loadButtonPositions();
    ofRectangle* findButtonAt(ofVec2f pos);
    
    string pendingVideoFolder = "";
    
    bool bIsSelecting = false;
    bool bIsDraggingGroup = false;
    ofVec2f selectionStart, selectionEnd;
    vector<ofRectangle*> selectedRects;
    vector<ofVec2f> dragOffsets;
    vector<ofRectangle*> getAllInteractableRects();

    float saveFeedbackTimer = -10.0f;
    
    // Composant pour l'UI du Lecteur
    PlaylistPlayerUI playerUI;
    
    // Composant pour l'UI de l'Intelligence Artificielle
    PlaylistGeminiUI geminiUI;
    
    PlaylistControlsUI controlsUI;
    PlaylistWindowControlsUI windowControlsUI;
    PlaylistTextureUI textureUI;
    
    PlaylistSearchBar searchBar;
    PlaylistHistoryUI historyUI;
    PlaylistViewsUI viewsUI;
    PlaylistWinPosUI winPosUI;
    vector<SearchableButton> getAllSearchableButtons();
    
    PlaylistSequenceUI sequenceUI;

    // Indique si un champ de texte est actuellement actif pour bloquer les raccourcis globaux
    bool isTyping() const {
        if (searchBar.isVisible()) return true;
        if (editingNote != nullptr) return true;
        if (geminiUI.bApiKeyFocused || geminiUI.bThemeFocused || geminiUI.bPromptVid1Focused || geminiUI.bPromptVid2Focused) return true;
        if (viewsUI.isEditing()) return true;
        if (winPosUI.isEditing()) return true;
        if (controlsUI.bEditingGolmSeed || controlsUI.bEditingGolSeed) return true;
        return false;
    }
};