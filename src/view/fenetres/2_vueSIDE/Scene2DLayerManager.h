#pragma once

#include "ofMain.h"
#include "CreatureSystem.h"
#include "CousinCon.h"
#include "ColliderLayer.h"
#include "SlimeLayer.h"
#include "SauteursLayer.h"
#include "FishSchoolLayer.h"
#include "PoulpeLayer.h"
#include "WalkerLayer.h"
#include "GearLayer.h"
#include "LightningLayer.h"
#include "PlantLayer.h"
#include "FlytrapLayer.h"
#include "FluidFloorLayer.h"
#include "MachineLayer.h"
#include "DiggingCreature.h"
#include "MachineAutoLayer.h"
#include "CurtainCreature.h"

class Scene2DLayerManager {
public:
    void setup(float totalWidth, float jarW, float jarX, float frontW, float frontX);
    void update(const ofVec2f& mouse);
    void draw(const ofVec2f& mouse);
    void keyPressed(int key, const ofVec2f& mouse);
    void mousePressed(const ofVec2f& mouse, int button);
    void mouseReleased(const ofVec2f& mouse, int button);

    // Layer Toggles (publics pour l'UI)
    bool bDrawCreatures = true;
    bool bDrawPoulpe = false;
    bool bDrawFish = false;
    bool bDrawSauteurs = false;
    bool bDrawSlime = false;
    bool bDrawWalker = false;
    bool bDrawLightning = false;
    bool bDrawPlants = false;
    bool bDrawFlytraps = false;
    bool bDrawFluidFloor = false;
    bool bDrawGears = false;
    bool bDrawMachine = false;
    bool bDrawDigging = false;
    bool bDrawMachineAuto = false;
    bool bDrawCurtain = false;
    bool bDrawColliders = false;

    // Layer objects
    CreatureSystem creatureSystem;
    vector<shared_ptr<CousinCon>> cousinCons;
    
    shared_ptr<ColliderLayer> colliderLayer;
    SlimeLayer slimeLayer;
    SauteursLayer sauteursLayer;
    FishSchoolLayer fishSchoolLayer;
    PoulpeLayer poulpeLayer;
    WalkerLayer walkerLayer;
    GearLayer gearLayer;
    LightningLayer lightningLayer;
    PlantLayer plantLayer;
    FlytrapLayer flytrapLayer;
    FluidFloorLayer fluidFloorLayer;
    MachineLayer machineLayer;
    DiggingCreature diggingCreature;
    MachineAutoLayer machineAuto;
    CurtainCreature curtain;

    // State for lightning
    bool bLightningHasStart = false;
    ofVec2f lightningStartPos;

    // For UI
    int getSardineCount();

private:
    float totalSceneWidth;
    ofImage imgConcombre;
    ofImage imgRush;
};