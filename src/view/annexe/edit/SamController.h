#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
//#include <opencv2/dnn.hpp>
//#include <opencv2/imgproc.hpp>

class SamController {
public:
    void setup();
    void activate(const ofImage& img);
    void deactivate();

    void draw(float targetWidth, float targetHeight, float viewZoom, float imageWidth, float imageHeight);
    void drawUI();
    void drawDragFeedback(ofVec2f currentMouseInImageSpace, float targetWidth, float targetHeight, float imageWidth, float imageHeight, float viewZoom);

    // Les événements souris prennent des coordonnées dans l'espace de l'image
    // Retourne true si l'événement a été consommé
    bool mousePressed(int x, int y, ofVec2f mouseInImageSpace);
    void mouseDragged(ofVec2f mouseInImageSpace);
    void mouseReleased(ofVec2f mouseInImageSpace, const ofImage& img);

    bool isActive() const { return bIsActive; }
    bool isDragging() const { return bIsDragging; }

    // Pour qu'AnnexeApp puisse afficher les avertissements
    string getWarningMessage();
    void clearWarningMessage();

private:
    void runInference(const ofImage& img);
    void saveSegmentation();
    void reset();

    bool bIsActive = false;

    // État
    vector<ofVec2f> points; // Stockés dans l'espace de l'image
    vector<int> labels; // 0: bg, 1: fg, 2: box_tl, 3: box_br
    ofImage previewMask;
    bool bMaskGenerated = false;

    // UI
    ofRectangle saveBtn;
    ofRectangle resetBtn;

    // Interaction
    bool bIsDragging = false;
    ofVec2f dragStartInImageSpace;
    
    // Communication avec AnnexeApp
    string warningMessage;
};