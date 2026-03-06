#pragma once
#include "ofMain.h"

// 1. L'INTERFACE (Le moule)
class BaseLayer {
public:
    virtual ~BaseLayer() {}
    
    // On force tous les layers à accepter la souris
    virtual void update(float mouseX, float mouseY, float time) = 0;
    virtual void draw() = 0;

    // Gestion de l'état
    bool bActive = true;
    char toggleKey = 0; 
    string name = "Layer";

    virtual void mousePressed(float x, float y, int button) {}
    virtual void mouseReleased(float x, float y, int button) {}
};

// 2. LE MANAGER
class LayerManager {
public:
    vector<shared_ptr<BaseLayer>> layers;

    void addLayer(shared_ptr<BaseLayer> layer, string name, char key) {
        layer->name = name;
        layer->toggleKey = key;
        layers.push_back(layer);
    }

    void update(float mouseX, float mouseY, float time) {
        for(auto& l : layers) {
            if(l->bActive) l->update(mouseX, mouseY, time);
        }
    }

    void draw() {
        for(auto& l : layers) {
            if(l->bActive) l->draw();
        }
    }

    void keyPressed(int key) {
        for(auto& l : layers) {
            if(l->toggleKey != 0 && (key == l->toggleKey || key == toupper(l->toggleKey))) {
                l->bActive = !l->bActive;
                ofLog() << "Layer " << l->name << (l->bActive ? " ON" : " OFF");
            }
        }
    }

    void mousePressed(float x, float y, int button) {
        for(auto& l : layers) {
            if(l->bActive) l->mousePressed(x, y, button);
        }
    }

    void mouseReleased(float x, float y, int button) {
        for(auto& l : layers) {
            if(l->bActive) l->mouseReleased(x, y, button);
        }
    }
};