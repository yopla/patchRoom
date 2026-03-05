#include "InteractionVisualizer.h"
#include "ofApp.h"
#include "ButtonApp.h"
#include "Scene2D_SIDE.h"
#include "RoomApp.h"      // Pour accéder à fluidRing
#include "FluidRing.h"    // Pour accéder aux propriétés de fluidRing
#include <algorithm>

void InteractionVisualizer::draw(shared_ptr<ofApp> mainApp, shared_ptr<Scene2D_SIDE> sceneSide, float roomWidth, float roomDepth, float roomSolDepth) {
    if(!mainApp || !mainApp->buttonApp) return;

    // Dessin des boutons sur le sol (intégré à la scène 3D)
    float d2 = roomDepth / 2.0f;
    float zCenterSol = -d2 + roomSolDepth / 2.0f;
    // On dessine légèrement au dessus du sol (y=2) pour éviter le z-fighting
    mainApp->buttonApp->buttonWindow.drawPreview(0, 2, zCenterSol, roomWidth, roomSolDepth);
    
 // --- Logique de visualisation des interactions ---

    // 1. Récupérer TOUS les boutons (actifs et inactifs) pour la logique de proximité
    float winW = mainApp->buttonApp->buttonWindow.getWidth();
    float winH = mainApp->buttonApp->buttonWindow.getHeight();
    
    if (winW <= 0.001f || winH <= 0.001f) return; // Sécurité division par zéro
    
    vector<pair<glm::vec3, bool>> allButtons;
    for(const auto& b : mainApp->buttonApp->buttonWindow.buttons) {
        ofVec2f center = b.rect.getCenter();
        float pctX = center.x / winW;
        float pctY = center.y / winH;
        
        float x3d = -roomWidth/2.0f + pctX * roomWidth;
        float z3d = (zCenterSol - roomSolDepth/2.0f) + pctY * roomSolDepth;
        
        glm::vec3 pos(x3d, 2.0f, z3d);
        bool isActive = (b.targetAlpha > 100);
        allButtons.push_back({pos, isActive});
    }

    // 2. Lignes jaunes entre les halos et les 3 boutons les plus proches (s'ils sont actifs)
    // Logique miroir de PerceptionSystem pour la cohérence visuelle
    const int maxButtonsToConsider = 3;
    struct BtnDist { float distSq; glm::vec3 pos; bool active; };
    auto compareDist = [](const BtnDist& a, const BtnDist& b){ return a.distSq < b.distSq; };

    // Ligne jaune entre le bouton survolé et les HaloCreatures
    // a) Pour les halos de Scene2D_SIDE
    if(sceneSide && !sceneSide->layerManager.halos.empty()) {
        for(auto& h : sceneSide->layerManager.halos) {
            ofVec3f haloPos = sceneSide->get3DPos(h->pos.x, h->pos.y);
            
            vector<BtnDist> distances;
            for(const auto& btn : allButtons) {
                distances.push_back({glm::distance2(glm::vec3(haloPos), btn.first), btn.first, btn.second});
            }

            size_t n = std::min((size_t)maxButtonsToConsider, distances.size());
            std::partial_sort(distances.begin(), distances.begin() + n, distances.end(), compareDist);

            for(size_t k=0; k<n; ++k) {
                if(distances[k].active) {
                    ofPushStyle(); ofSetColor(255, 255, 0); ofSetLineWidth(1);
                    ofDrawLine(ofVec3f(distances[k].pos), haloPos);
                    ofPopStyle();
                }
            }
        }
    }

    // b) Pour les halos du LightFlyRing
    if(mainApp->roomApp && mainApp->roomApp->bLightFlyRingEnabled) {
        auto& ring = mainApp->roomApp->lightFlyRing;
        for(auto& c : ring.creatures) {
            float u = c->pos.x / ring.fbo.getWidth();
            float v = c->pos.y / ring.fbo.getHeight();
            float angle = u * TWO_PI;
            float yWorld = ring.height - (v * (ring.height + ring.bottomExt));
            ofVec3f haloPos(cos(angle) * ring.radius, yWorld, sin(angle) * ring.radius);

            vector<BtnDist> distances;
            for(const auto& btn : allButtons) {
                distances.push_back({glm::distance2(glm::vec3(haloPos), btn.first), btn.first, btn.second});
            }

            size_t n = std::min((size_t)maxButtonsToConsider, distances.size());
            std::partial_sort(distances.begin(), distances.begin() + n, distances.end(), compareDist);

            for(size_t k=0; k<n; ++k) {
                if(distances[k].active) {
                    ofPushStyle(); ofSetColor(255, 255, 0); ofSetLineWidth(1);
                    ofDrawLine(ofVec3f(distances[k].pos), haloPos);
                    ofPopStyle();
                }
            }
        }
    }

    // 3. Visualisation des autres interactions (Poulpe, Fluides) qui dépendent uniquement des boutons actifs


    vector<ofVec3f> btnPositions = mainApp->buttonApp->buttonWindow.get3DPosForActiveButtons(roomWidth, roomSolDepth, roomDepth);
    
    if(!btnPositions.empty()) {
      

        // Visualisation Poulpe (Trait Vert)
        if (sceneSide && sceneSide->layerManager.bDrawPoulpe) {
            ofVec2f head2D = sceneSide->layerManager.poulpeLayer.getHeadPos();
            ofVec3f head3D = sceneSide->get3DPos(head2D.x, head2D.y);
            
            float minDistSq = FLT_MAX;
            ofVec3f bestBtn;
            bool found = false;
            
            for(const auto& btn : btnPositions) {
                float dSq = head3D.squareDistance(btn);
                if(dSq < minDistSq) {
                    minDistSq = dSq;
                    bestBtn = btn;
                    found = true;
                }
            }
            
            if(found) {
                ofPushStyle();
                ofSetColor(0, 255, 0);
                ofSetLineWidth(2);
                ofDrawLine(head3D, bestBtn);
                ofPopStyle();
            }
        }

        // --- AJOUT : Visualisation Shark (Orange) ---
        if (sceneSide) {
             ofPushStyle();
             ofSetColor(255, 165, 0); // Orange
             ofSetLineWidth(2);
             
             float w2 = roomWidth / 2.0f;
             float d2 = roomDepth / 2.0f;
             float targetY = 1000.0f; // Hauteur moyenne d'apparition

             for(const auto& btnPos : btnPositions) {
                float distFront = abs(btnPos.z - (-d2));
                float distBack = abs(btnPos.z - d2);
                float distJar = abs(btnPos.x - (-w2));
                float distCour = abs(btnPos.x - w2);
                float minDistWall = std::min({distFront, distBack, distJar, distCour});
                
                ofVec3f wallPos;
                if (minDistWall == distFront)      wallPos.set(btnPos.x, targetY, -d2);
                else if (minDistWall == distBack)  wallPos.set(btnPos.x, targetY, d2);
                else if (minDistWall == distJar)   wallPos.set(-w2, targetY, btnPos.z);
                else                               wallPos.set(w2, targetY, btnPos.z);
                
                ofDrawLine(btnPos, wallPos);
             }
             ofPopStyle();
        }

        // --- AJOUT : Visualisation des liens vers les fluides (Cyan) ---
        ofPushStyle();
        ofSetColor(ofColor::cyan);
        ofSetLineWidth(1.5);

        // 1. Liens vers le FluidFloorLayer de Scene2D_SIDE
        if (sceneSide && sceneSide->layerManager.bDrawFluidFloor) {
            // NOTE: yStartSol est hardcodé à 1472.0f dans Scene2D_SIDE::draw()
            float yStartSol = 1472.0f;
            float fluidLayerY_in_Scene = yStartSol - 50.0f; // On vise le bas du mur (sol)
            
            float w2 = roomWidth / 2.0f;
            float d2 = roomDepth / 2.0f;
            
            // Constantes de layout de la Scene2D_SIDE
            const float wFront = sceneSide->wFront;
            const float wJar = sceneSide->wJar;
            const float srcX_Front = wJar;
            const float srcX_Cour = wJar + wFront;
            const float srcX_Back = wJar + wFront + wJar;

            for(auto& btnPos : btnPositions) {
                // Trouver le mur le plus proche
                float distFront = abs(btnPos.z - (-d2));
                float distBack = abs(btnPos.z - d2);
                float distJar = abs(btnPos.x - (-w2));
                float distCour = abs(btnPos.x - w2);
                float minDistWall = std::min({distFront, distBack, distJar, distCour});

                float sceneX = 0;
                
                if (minDistWall == distFront)      sceneX = srcX_Front + ((btnPos.x + w2) / roomWidth) * wFront;
                else if (minDistWall == distBack)  sceneX = srcX_Back + ((w2 - btnPos.x) / roomWidth) * wFront;
                else if (minDistWall == distJar)   sceneX = wJar * ((d2 - btnPos.z) / roomDepth);
                else                               sceneX = srcX_Cour + ((btnPos.z + d2) / roomDepth) * wJar;

                // On ne peut pas utiliser une Y aléatoire pour le dessin, on prend le milieu
                ofVec3f fluidTarget3D = sceneSide->get3DPos(sceneX, fluidLayerY_in_Scene);
                ofDrawLine(btnPos, fluidTarget3D);
            }
        }

        // 2. Liens vers le FluidRing de RoomApp
        if (mainApp->roomApp && mainApp->roomApp->bFluidRingEnabled) {
            auto& ring = mainApp->roomApp->fluidRing;
            float ringY = ring.height - (ring.height + ring.bottomExt) / 2.0f;

            for(auto& btnPos : btnPositions) {
                float angle = atan2(btnPos.z, btnPos.x);
                
                float ringX = cos(angle) * ring.radius;
                float ringZ = sin(angle) * ring.radius;
                
                ofVec3f ringTarget3D(ringX, ringY, ringZ);
                ofDrawLine(btnPos, ringTarget3D);
            }
        }
        ofPopStyle();
    }
}