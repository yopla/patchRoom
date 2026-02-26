#pragma once
#include "ofMain.h"
#include "LayerSystem.h"

// Structure d'un éclair individuel
struct Bolt {
    ofVec2f start;      // Départ (Ciel)
    ofVec2f end;        // Arrivée (Souris)
    float life;         // Vie de 1.0 à 0.0
    float decay;        // Vitesse de disparition
    float thickness;    // Épaisseur
    ofColor color;      // Couleur de la lueur
    vector<ofVec2f> points; // Points du tracé (recalculés pour vibrer)
};
class LightningLayer : public BaseLayer {
public:
    void setup(float w, float h);
    void update(float mouseX, float mouseY) override;
    void draw() override;
    
    // MODIFICATION : On prend maintenant le point de départ et d'arrivée
    void trigger(ofVec2f p1, ofVec2f p2);

    void setScale(float s) { scale = s; }
    float getScale() { return scale; }

private:
    // ... (Reste identique)
    void createBoltPoints(ofVec2f p1, ofVec2f p2, float displacement, vector<ofVec2f>& pts);
    
    float width, height;
    float scale = 1.0f;
    vector<Bolt> bolts; 
    ofFbo glowFbo; 
};