#pragma once

#include "ofMain.h"
#include "CreatureSystem.h"
#include "CousinCon.h"
#include "HaloCreature.h"
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
#include "PuyoLayer.h"
#include "BubbleLayer.h"
#include "KaniLayer.h"
#include "Slime2Layer.h"
#include "TeaaLayer.h"
#include "BalletLayer.h"
#include "KundelichLayer.h"
#include "KineShad.h"
#include "MultiPendulumLayer.h"
#include "PancarteLayer.h"
#include "PendulumLayer.h"
#include "PinceLayer.h"
#include "PinceBraLayer.h"
#include "PinceFoireLayer.h"
#include "WhaaLayer.h"
#include "ChainCrea.h"
#include "WallWalkerLayer.h"
#include "SwingLayer.h"
#include "FluidLayer.h"
#include "FluidDeuxLayer.h"
#include "FluidTroisLayer.h"
#include "FireALayer.h"
#include "FireBLayer.h"
#include "FireCLayer.h"
#include "MongolfierLayer.h"
#include "GroPuyoLayer.h"
#include "WalkingSquareLayer.h"
#include "TripodEyeLayer.h"
#include "AvoiderLayer.h"
#include "Avoider2Layer.h"
#include "Avoider4Layer.h"
#include "TourellesLayer.h"
#include "AutoPongLayer.h"
#include "AutoSnakeLayer.h"
#include "EatMapLayer.h"
#include "SurSauteurLayer.h"
#include "Crayon.h"
#include "PaperLightLayer.h"
#include "PhysicSamLayer.h"
#include "AliveLayer.h"

class Scene2DLayerManager {
public:
    void setup(float totalWidth, float jarW, float jarX, float frontW, float frontX);
    void update(const ofVec2f& mouse, float time, bool isSpacePressed = false);
    void draw(const ofVec2f& mouse);
    void keyPressed(int key, const ofVec2f& mouse);
    void mousePressed(const ofVec2f& mouse, int button);
    void mouseReleased(const ofVec2f& mouse, int button);
    void mouseDragged(const ofVec2f& mouse, int button);
    void addCousinCon(float x, float y);
    void addHalo(float x, float y);
    
    string selectedCreatureToSpawn = "Ripple";
    string selectedInteractiveLayer = "";
    void spawnSelectedCreature(float x, float y);
    void removeLastCreature();

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
    bool bDrawPuyo = false;
    bool bDrawBubbles = false;
    bool bDrawKani = false;
    bool bDrawSlime2 = false;
    bool bDrawTeaa = false;
    bool bDrawBallet = false;
    bool bDrawKundelich = false;
    bool bDrawKineShad = false;
    bool bDrawMultiPendulum = false;
    bool bDrawPancarte = false;
    bool bDrawPendulum = false;
    bool bDrawPince = false;
    bool bDrawPinceBra = false;
    bool bDrawPinceFoire = false;
    bool bDrawWhaa = false;
    bool bDrawChainCrea = false;
    bool bDrawWallWalker = false;
    bool bDrawSwing = false;
    bool bDrawFluid = false;
    bool bDrawFluidDeux = false;
    bool bDrawFluidTrois = false;
    bool bDrawFireA = false;
    bool bDrawFireB = false;
    bool bDrawFireC = false;
    bool bDrawMongolfier = false;
    bool bDrawGroPuyo = false;
    bool bDrawWalkingSquare = false;
    bool bDrawTripodEye = false;
    bool bDrawAvoider = false;
    bool bDrawAvoider2 = false;
    bool bDrawAvoider4 = false;
    bool bDrawTourelles = false;
    bool bDrawAutoPong = false;
    bool bDrawAutoSnake = false;
    bool bDrawEatMap = false;
    bool bDrawCrayon = false;
    bool bDrawSurSauteurs = false;
    bool bDrawPaperLight = false;
    bool bDrawPhysicSam = false;
    bool bDrawAlive = false;

    // Layer objects
    CreatureSystem creatureSystem;
    vector<shared_ptr<CousinCon>> cousinCons;
    vector<shared_ptr<HaloCreature>> halos;
    
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
    PuyoLayer puyoLayer;
    BubbleLayer bubbleLayer;
    KaniLayer kaniLayer;
    Slime2Layer slime2Layer;
    TeaaLayer teaaLayer;
    BalletLayer balletLayer;
    KundelichLayer kundelichLayer;
    KineShad kineShadLayer;
    MultiPendulumLayer multiPendulumLayer;
    PancarteLayer pancarteLayer;
    PendulumLayer pendulumLayer;
    PinceLayer pinceLayer;
    PinceBraLayer pinceBraLayer;
    PinceFoireLayer pinceFoireLayer;
    WhaaLayer whaaLayer;
    ChainCrea chainCreaLayer;
    WallWalkerLayer wallWalkerLayer;
    SwingLayer swingLayer;
    FluidLayer fluidLayer;
    FluidDeuxLayer fluidDeuxLayer;
    FluidTroisLayer fluidTroisLayer;
    FireALayer fireALayer;
    FireBLayer fireBLayer;
    FireCLayer fireCLayer;
    MongolfierLayer mongolfierLayer;
    GroPuyoLayer groPuyoLayer;
    WalkingSquareLayer walkingSquareLayer;
    TripodEyeLayer tripodEyeLayer;
    AvoiderLayer avoiderLayer;
    Avoider2Layer avoider2Layer;
    Avoider4Layer avoider4Layer;
    TourellesLayer tourellesLayer;
    AutoPongLayer autoPongLayer;
    AutoSnakeLayer autoSnakeLayer;
    shared_ptr<EatMapLayer> eatMapLayer;
    SurSauteurLayer surSauteurLayer;
    Crayon crayon;
    PaperLightLayer paperLightLayer;
    PhysicSamLayer physicSamLayer;
    AliveLayer aliveLayer;

    // State for lightning
    bool bLightningHasStart = false;
    ofVec2f lightningStartPos;

    // For UI
    int getSardineCount();
        float totalSceneWidth;
    ofImage imgConcombre;
    ofImage imgRush;

private:

};