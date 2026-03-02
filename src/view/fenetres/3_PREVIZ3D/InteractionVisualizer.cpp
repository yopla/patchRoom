#include "InteractionVisualizer.h"
#include "ofApp.h"
#include "../ButtonApp.h"
#include "../2_vueSIDE/Scene2D_SIDE.h"

void InteractionVisualizer::draw(shared_ptr<ofApp> mainApp, shared_ptr<Scene2D_SIDE> sceneSide, float roomWidth, float roomDepth, float roomSolDepth) {
    if(!mainApp || !mainApp->buttonApp) return;

    // Dessin des boutons sur le sol (intégré à la scène 3D)
    float d2 = roomDepth / 2.0f;
    float zCenterSol = -d2 + roomSolDepth / 2.0f;
    
    // On dessine légèrement au dessus du sol (y=2) pour éviter le z-fighting
    mainApp->buttonApp->buttonWindow.drawPreview(0, 2, zCenterSol, roomWidth, roomSolDepth);
    
    // Ligne jaune entre le bouton survolé et les HaloCreatures
    vector<ofVec3f> btnPositions = mainApp->buttonApp->buttonWindow.get3DPosForActiveButtons(roomWidth, roomSolDepth, roomDepth);
    
    if(!btnPositions.empty()) {
        if(sceneSide && !sceneSide->layerManager.halos.empty()) {
            for(auto& btnPos : btnPositions) {
                for(auto& h : sceneSide->layerManager.halos) {
                    ofVec3f haloPos = sceneSide->get3DPos(h->pos.x, h->pos.y);
                    ofPushStyle();
                    ofSetColor(255, 255, 0);
                    ofSetLineWidth(1);
                    ofDrawLine(btnPos, haloPos);
                    ofPopStyle();
                }
            }
        }

        // Visualisation des liens vers les Halos du LightFlyRing
        if(mainApp->roomApp && mainApp->roomApp->bLightFlyRingEnabled) {
            auto& ring = mainApp->roomApp->lightFlyRing;
            float fboW = ring.fbo.getWidth();
            float fboH = ring.fbo.getHeight();
            float totalH = ring.height + ring.bottomExt;

            for(auto& btnPos : btnPositions) {
                for(auto& c : ring.creatures) {
                    // Conversion coordonnées FBO (2D) -> Monde (3D Cylindrique)
                    float u = c->pos.x / fboW;
                    float v = c->pos.y / fboH;
                    
                    float angle = u * TWO_PI;
                    float yWorld = ring.height - (v * totalH);
                    float xWorld = cos(angle) * ring.radius;
                    float zWorld = sin(angle) * ring.radius;
                    
                    ofVec3f haloPos(xWorld, yWorld, zWorld);
                    
                    ofPushStyle();
                    ofSetColor(255, 255, 0);
                    ofSetLineWidth(1);
                    ofDrawLine(btnPos, haloPos);
                    ofPopStyle();
                }
            }
        }
    }
}