#pragma once
#include "ofMain.h"

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

    void saveHighResFrame();
    void generateDepthMapAI();
    void generateSAMCollider();
    void toggleSamControl();
    void setupRipple();
    void toggleRecording();

    float targetWidth = 2048.0f;
    float targetHeight = 3128.0f;

    ofImage img;

    float viewZoom = 1.0f;
    ofVec2f viewPan;
    ofVec2f lastMouse;
    bool isSpacePressed = false;
    
    string warningMessage = "";
    float warningEndTime = 0.0f;
    ofVec2f getTransformedMouse();

    // Recording
    bool bIsRecording = false;
    string recordFolder;
    int recordFrameCount = 0;

    // Ripple Effect
    void updateRipple();
    void createRippleAt(int x, int y);
    void processRipples();
    void renderRipples();
    bool bRippleEffect = false;
    ofImage rippleOutputImage;
    vector<int> rippleBuffer1;
    vector<int> rippleBuffer2;
    int rippleCols = 0, rippleRows = 0;
    float rippleBufferScale = 0.5f;
    int rippleSize = 5;
        bool bSamControlActive = false;


private:
    // SAM Interactive Control
    vector<ofVec2f> samPoints;
    vector<int> samLabels; // 0: bg, 1: fg, 2: box_tl, 3: box_br
    ofImage samPreviewMask;
    bool bSamMaskGenerated = false;

    ofRectangle samSaveBtn;
    ofRectangle samResetBtn;

    bool bSamIsDragging = false;
    ofVec2f samDragStart;
    float samMousePressTime = 0;

    void saveRecordedFrame();
    void runSamInference();
    void saveSamSegmentation();
    void resetSamSelection();
};