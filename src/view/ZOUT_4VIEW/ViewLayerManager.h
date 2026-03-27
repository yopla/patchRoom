#pragma once
#include "ofMain.h"

struct CropLayer {
    ofRectangle sourceRect; 
    ofVec2f destPos;        
    float rotation = 0.0;
    float scale = 1.0;
    bool useCenterAnchor = false; 
    string name;
};

class ViewLayerManager {
public:
    void addLayer(string name, float srcX, float srcY, float w, float h, float dstX, float dstY, bool center = false);
    void draw(ofTexture& tex);
    void keyPressed(int key);

    vector<CropLayer> layers;
    int selectedIndex = 0;
};