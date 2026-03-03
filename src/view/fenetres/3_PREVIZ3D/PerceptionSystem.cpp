#include "PerceptionSystem.h"
#include "HaloCreature.h"
#include "ButtonApp.h"
#include "Scene2D_SIDE.h"
#include "RoomApp.h"
#include "LightFlyRing.h"
#include "FluidFloorLayer.h"
#include "FluidRing.h"
#include <algorithm>

void PerceptionSystem::setup() {
    maxPerceptionRadius = 1500;//radius;
    maxButtonsToConsider = 3;//maxButtons;
}

void PerceptionSystem::update(shared_ptr<ButtonApp> buttonApp, 
                            shared_ptr<Scene2D_SIDE> sceneSide, 
                            shared_ptr<RoomApp> roomApp) {
    
    if (!buttonApp) return;

    // 1. Récupérer les positions 3D de TOUS les boutons (actifs et inactifs)
    float rW = 2400.0f;
    float rD = 2624.0f;
    float rSolD = 2368.0f;
    
    if (roomApp) {
        rW = roomApp->roomWidth;
        rD = roomApp->roomDepth;
        rSolD = roomApp->roomSolDepth;
    }

    vector<pair<glm::vec3, bool>> allButtons;
    vector<glm::vec3> activeButtons;

    float winW = buttonApp->buttonWindow.getWidth();
    float winH = buttonApp->buttonWindow.getHeight();

    for(const auto& b : buttonApp->buttonWindow.buttons) {
        ofVec2f center = b.rect.getCenter();
        
        float pctX = center.x / winW;
        float pctY = center.y / winH;
        
        float d2 = rD / 2.0f;
        float zCenterSol = -d2 + rSolD / 2.0f;
        
        float x3d = -rW/2.0f + pctX * rW;
        float z3d = (zCenterSol - rSolD/2.0f) + pctY * rSolD;
        
        glm::vec3 pos(x3d, 2.0f, z3d);
        bool isActive = (b.targetAlpha > 100); // Seuil arbitraire pour considérer actif (hovered)
        
        allButtons.push_back({pos, isActive});
        if(isActive) activeButtons.push_back(pos);
    }


    // Interaction avec les fluides
    updateFluids(activeButtons, sceneSide, roomApp);

    // Interaction Poulpe
    updatePoulpe(sceneSide, roomApp, activeButtons);



    // 2. Mettre à jour la perception pour les halos de la scène 2D (Scene2D_SIDE)
    if (sceneSide && !sceneSide->layerManager.halos.empty()) {
        vector<glm::vec3> haloPositions2D;
        for (auto& halo : sceneSide->layerManager.halos) {
            haloPositions2D.push_back(sceneSide->get3DPos(halo->pos.x, halo->pos.y));
        }
        updateHalos(sceneSide->layerManager.halos, haloPositions2D, allButtons);
    }

    // 3. Mettre à jour la perception pour les halos de l'anneau lumineux (LightFlyRing)
    if (roomApp && roomApp->bLightFlyRingEnabled && !roomApp->lightFlyRing.creatures.empty()) {
        auto& ring = roomApp->lightFlyRing;
        vector<glm::vec3> haloPositionsRing;
        
        float fboW = ring.fbo.getWidth();
        float fboH = ring.fbo.getHeight();
        float totalH = ring.height + ring.bottomExt;

        for (auto& creature : ring.creatures) {
             // Conversion coordonnées FBO (2D) -> Monde (3D Cylindrique)
            float u = creature->pos.x / fboW;
            float v = creature->pos.y / fboH;
            float angle = u * TWO_PI;
            float yWorld = ring.height - (v * totalH);
            float xWorld = cos(angle) * ring.radius;
            float zWorld = sin(angle) * ring.radius;
            haloPositionsRing.push_back(glm::vec3(xWorld, yWorld, zWorld));
        }
        updateHalos(ring.creatures, haloPositionsRing, allButtons);
    }



    
}

void PerceptionSystem::updateHalos(vector<shared_ptr<HaloCreature>>& halos, 
                            const vector<glm::vec3>& halo3DPositions, 
                            const vector<pair<glm::vec3, bool>>& allButtons) {
    
    if (halos.size() != halo3DPositions.size()) return;

    // Pour chaque halo, on vérifie s'il est influencé par un bouton actif PARMI les 3 plus proches
    for (int i = 0; i < halos.size(); ++i) {
        auto& halo = halos[i];
        const auto& haloPos3D = halo3DPositions[i];

        // Liste des distances vers TOUS les boutons
        struct BtnDist {
            float distSq;
            bool active;
        };
        vector<BtnDist> distances;
        distances.reserve(allButtons.size());

        for(const auto& btn : allButtons) {
            float d2 = glm::distance2(haloPos3D, btn.first);
            distances.push_back({d2, btn.second});
        }

        // On ne garde que les N plus proches (actifs ou non)
        size_t n = std::min((size_t)maxButtonsToConsider, distances.size());
        std::partial_sort(distances.begin(), distances.begin() + n, distances.end(), 
            [](const BtnDist& a, const BtnDist& b){ return a.distSq < b.distSq; });

        bool isTriggered = false;
        float radiusSq = maxPerceptionRadius * maxPerceptionRadius;

        // On vérifie si l'un des N plus proches est ACTIF et dans le rayon
        for(size_t k=0; k<n; ++k) {
            if(distances[k].active && distances[k].distSq < radiusSq) {
                isTriggered = true;
                break; // Un seul bouton suffit pour déclencher l'effet
            }
        }
        
        // On met à jour l'état du halo
        halo->setHovering(isTriggered);
    }
}



void PerceptionSystem::updateFluids(const vector<glm::vec3>& activeButtons3DPositions,
                                    shared_ptr<Scene2D_SIDE> sceneSide,
                                    shared_ptr<RoomApp> roomApp) {
    if (activeButtons3DPositions.empty()) {
        return;
    }

    // --- 1. Interaction avec Scene2D_SIDE FluidFloorLayer ---
    if (sceneSide && sceneSide->layerManager.bDrawFluidFloor) {
        float rW = 2400.0f;
        float rD = 2624.0f;
        if (roomApp) {
            rW = roomApp->roomWidth;
            rD = roomApp->roomDepth;
        }
        
        float w2 = rW / 2.0f;
        float d2 = rD / 2.0f;

        // Constantes de layout de la Scene2D_SIDE
        const float wFront = sceneSide->wFront;
        const float wJar = sceneSide->wJar;
        float srcX_Front = sceneSide->wJar;
        const float srcX_Cour = wJar + wFront;
        const float srcX_Back = wJar + wFront + wJar;

        for (const auto& btnPos : activeButtons3DPositions) {
            // Trouver le mur le plus proche
            float distFront = abs(btnPos.z - (-d2));
            float distBack = abs(btnPos.z - d2);
            float distJar = abs(btnPos.x - (-w2));
            float distCour = abs(btnPos.x - w2);
            float minDistWall = std::min({distFront, distBack, distJar, distCour});

            float sceneX = 0;
            
            if (minDistWall == distFront)      sceneX = srcX_Front + ((btnPos.x + w2) / rW) * wFront;
            else if (minDistWall == distBack)  sceneX = srcX_Back + ((w2 - btnPos.x) / rW) * wFront;
            else if (minDistWall == distJar)   sceneX = 0 + ((d2 - btnPos.z) / rD) * wJar;
            else                               sceneX = srcX_Cour + ((btnPos.z + d2) / rD) * wJar;

            // Interaction avec Y aléatoire
            float fluidY = ofRandom(sceneSide->layerManager.fluidFloorLayer.height);
            
            // d. Créer une force (réduite pour éviter l'instabilité)
            ofVec2f force(ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f));
            force.normalize();
            force *= 5.0f; // Renforcé (était 0.5f)

            // e. Appliquer la force
            sceneSide->layerManager.fluidFloorLayer.addForce(sceneX, fluidY, force.x, force.y);
        }
    }

    // --- 2. Interaction avec RoomApp FluidRing ---
    if (roomApp && roomApp->bFluidRingEnabled) {
        auto& fluid = roomApp->fluidRing.fluid;

        for (const auto& btnPos : activeButtons3DPositions) {
            // a. Calculer l'angle du centre de la pièce vers le bouton
            float angle = atan2(btnPos.z, btnPos.x);
            if (angle < 0) angle += TWO_PI;
            
            // b. Convertir l'angle en coordonnée de texture 'u'
            float u = angle / TWO_PI;
            
            // c. Mapper 'u' à la coordonnée X de la simulation de fluide
            float fluidX = u * fluid.width;
            float fluidY = ofRandom(fluid.height);
            ofVec2f force(ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f));
            force.normalize();
            force *= 8.0f; // Renforcé (était 2.0f)
            fluid.addForce(fluidX, fluidY, force.x, force.y);
        }
    }
}

void PerceptionSystem::updatePoulpe(shared_ptr<Scene2D_SIDE> sceneSide, 
                                    shared_ptr<RoomApp> roomApp, 
                                    const vector<glm::vec3>& activeButtons3DPositions) {
    if (!sceneSide || !sceneSide->layerManager.bDrawPoulpe || activeButtons3DPositions.empty()) return;

    float rW = 2400.0f;
    float rD = 2624.0f;
    
    if (roomApp) {
        rW = roomApp->roomWidth;
        rD = roomApp->roomDepth;
    }

    // Mapping constants from Scene2D_SIDE
    float srcX_Front = sceneSide->wJar; // 2624
    float wFront = sceneSide->wFront;   // 2400
    float yStartSol = 1472.0f;
    ofVec2f poulpeHead = sceneSide->layerManager.poulpeLayer.getHeadPos();
    ofVec3f head3D = sceneSide->get3DPos(poulpeHead.x, poulpeHead.y);
    
    float minDistSq = FLT_MAX;
    glm::vec3 bestBtn3D;
    bool found = false;

    for (const auto& btn3D : activeButtons3DPositions) {
        ofVec3f btnOf(btn3D.x, btn3D.y, btn3D.z);
        float dSq = head3D.squareDistance(btnOf);
        
        if (dSq < minDistSq) {
            minDistSq = dSq;
            bestBtn3D = btn3D;
            found = true;
        }
    }

    if (found) {
        float w2 = rW / 2.0f;
        float d2 = rD / 2.0f;

        // Distances to walls from the button
        float distFront = abs(bestBtn3D.z - (-d2));
        float distBack = abs(bestBtn3D.z - d2);
        float distJar = abs(bestBtn3D.x - (-w2));
        float distCour = abs(bestBtn3D.x - w2);

        float minDistWall = std::min({distFront, distBack, distJar, distCour});

        float targetX = 0;
        float targetY = 0;

        // Scene2D layout constants
        const float heightFrontBack = 1472.0f;
        const float heightCour = 1072.0f;
        const float heightJar = 784.0f;
        const float wFront = sceneSide->wFront;
        const float wJar = sceneSide->wJar;
        const float srcX_Front = wJar;
        const float srcX_Cour = wJar + wFront;
        const float srcX_Back = wJar + wFront + wJar;

        // Use Perlin noise for a smoothly moving random height
        float time = 0.0f;
        if(roomApp) time = roomApp->localTime;
        else if(sceneSide) time = sceneSide->localTime;

        float noise_val = ofNoise(time * 0.5f, bestBtn3D.x * 0.01f);
        float random_y3d = 0;

        if (minDistWall == distFront) {
            random_y3d = noise_val * heightJar;//heightFrontBack;
            float u = (bestBtn3D.x + w2) / rW;
            targetX = srcX_Front + u * wFront;
            targetY = heightFrontBack - random_y3d;
        } else if (minDistWall == distBack) {
            random_y3d = noise_val * heightJar;//heightFrontBack;
            float u = (w2 - bestBtn3D.x) / rW;
            targetX = srcX_Back + u * wFront;
            targetY = heightFrontBack - random_y3d;
        } else if (minDistWall == distJar) {
            random_y3d = noise_val * heightJar;
            float u = (d2 - bestBtn3D.z) / rD;
            targetX = wJar * u; // srcX_Jar is 0
            targetY = 1472.0f - random_y3d;
        } else { // distCour
            random_y3d = noise_val * heightCour;
            float u = (bestBtn3D.z + d2) / rD;
            targetX = srcX_Cour + u * wJar;
            targetY = 1472.0f - random_y3d;
        }
        sceneSide->layerManager.poulpeLayer.setTarget(targetX, targetY);
    }
}
