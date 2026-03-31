#pragma once
#include "ofMain.h"
#include "edit/SamController.h"
#include "edit/AnnexeAIManager.h"
#include "goods/RippleController.h"
#include "goods/VolumetricLayerManager.h"
#include "PatteuLayer.h"
#include "DeuPatteuLayer.h"

class AnnexeApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    
    void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void dragEvent(ofDragInfo dragInfo) override;

    void setEnabled(bool enable) { bEnabled = enable; }

    void saveHighResFrame();
    void generateDepthMapAI();
    void generateDepthMapDepthAnything();
    void generateSAMCollider();
    void toggleSamControl();
    void toggleRecording();

    float targetWidth = 2048.0f;
    float targetHeight = 3128.0f;

    ofImage img;

    float viewZoom = 1.0f;
    ofVec2f viewPan;
    ofVec2f lastMouse;
    ofVec2f lastPatteuMouse;
    bool isSpacePressed = false;
    bool isShiftPressed = false;
    
    string warningMessage = "";
    float warningEndTime = 0.0f;
    ofVec2f getTransformedMouse();

    // Recording
    bool bIsRecording = false;
    string recordFolder;
    int recordFrameCount = 0;

    bool bSamControlActive = false;

    // Contrôleurs rendus publics pour garder la compatibilité avec l'UI s'ils sont appelés
    RippleController rippleController;
    VolumetricLayerManager volumManager;
    PatteuLayer patteuLayer;
    DeuPatteuLayer deuPatteuLayer;

private:
    bool bEnabled = true;
    SamController samController;
    AnnexeAiManager aiManager;
    void saveRecordedFrame();
public:
    void resetDepthMap();
};