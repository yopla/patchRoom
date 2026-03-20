#pragma once
#include "ofMain.h"

class ofApp;
class Scene2D_SIDE;

class InteractionVisualizer {
public:
    void draw(shared_ptr<ofApp> mainApp, shared_ptr<Scene2D_SIDE> sceneSide, float roomWidth, float roomDepth, float roomSolDepth);
};