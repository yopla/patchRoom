#pragma once
#include "ofMain.h"
#include "ofJson.h"
#include "PlaylistTextNote.h"
#include "PlaylistVisualFrame.h"
#include "PlaylistViewsUI.h"
#include <vector>
#include <string>
#include <memory>

class PlaylistVisualizerApp;

enum class SeqItemType { NONE, BUTTON, NOTE, FRAME };

struct SeqButton {
    std::string originalName;
    ofRectangle rect;
    ofRectangle labelRect;
    std::vector<std::string> states;
    int currentStateIdx = 0;
};

class PlaylistSequence {
public:
    std::string name;
    std::string filename;
    
    ofVec2f pan{250, 0};
    float zoom = 1.0f;
    
    std::vector<SeqButton> buttons;
    std::vector<std::shared_ptr<PlaylistTextNote>> notes;
    std::vector<std::shared_ptr<PlaylistVisualFrame>> frames;
    PlaylistViewsUI viewsUI;
    
    void save();
    void load();
};

class PlaylistSequenceUI {
public:
    void setup();
    void update(PlaylistVisualizerApp* app);
    void draw(PlaylistVisualizerApp* app);
    
    void drawTopButtons();
    void drawContextMenu();
    
    bool mousePressed(int x, int y, int button, PlaylistVisualizerApp* app);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseScrolled(int x, int y, float scrollY);
    void keyPressed(int key, PlaylistVisualizerApp* app);
    
    bool isActive() const { return activeSeqIdx != -1; }
    void openContextMenu(ofVec2f screenPos, std::string btnName);
    
    std::vector<PlaylistSequence> sequences;
    int activeSeqIdx = -1; 
    
    // Boutons de l'interface Séquence
    ofRectangle exitBtnRect;
    ofRectangle editBtnRect;
    ofRectangle saveBtnRect;
    ofRectangle addNoteBtnRect;
    ofRectangle addFrameBtnRect;
    ofRectangle vuesBtnRect;
    
    // Top buttons dans l'app principale
    std::vector<ofRectangle> topBtnRects;
    
    // Menu Contextuel
    bool bContextMenuOpen = false;
    ofVec2f contextMenuPos;
    std::string contextMenuTarget;
    std::vector<ofRectangle> contextMenuRects;
    
    // Menu Contextuel Interne (Dupliquer / Supprimer)
    bool bSeqContextMenuOpen = false;
    SeqItemType seqContextType = SeqItemType::NONE;
    int seqContextIndex = -1;
    ofRectangle seqDuplicateRect;
    ofRectangle seqDeleteRect;

    // Mode Edit interne
    bool bEditMode = false;
    bool bIsSelecting = false;
    bool bIsDraggingGroup = false;
    ofVec2f selectionStart, selectionEnd;
    std::vector<void*> selectedItems; // Pointeurs vers SeqButton, Note ou Frame
    std::vector<ofVec2f> dragOffsets;
    std::shared_ptr<PlaylistVisualFrame> resizingFrame = nullptr;
    SeqButton* resizingButton = nullptr;
    
    // Navigation interne
    bool isSpacePressed = false;
    bool bIsDraggingPan = false;
    ofVec2f lastMouse;
    std::shared_ptr<PlaylistTextNote> editingNote = nullptr;
    
    std::vector<std::string> getButtonStates(std::string btnName, PlaylistVisualizerApp* app);
    void applyButtonState(std::string btnName, std::string state, PlaylistVisualizerApp* app);
};