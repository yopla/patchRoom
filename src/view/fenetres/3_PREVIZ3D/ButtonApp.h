#pragma once
#include "ofMain.h"
#include "ButtonWindow.h"

class ButtonApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
    void keyPressed(int key) override;
    void keyReleased(int key) override;
    
    ButtonWindow buttonWindow;
    
    // Navigation
    ofVec2f pan;
    float zoom;
    bool isSpacePressed;
    ofVec2f lastMouse;
    
    ofVec2f getTransformedMouse(int x, int y);
};