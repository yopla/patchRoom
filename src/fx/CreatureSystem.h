#pragma once
#include "ofMain.h"
#include "JellyCreature.h"
#include "RippleCreature.h"
#include "SpringCreature.h"
#include "DoublePendulumCreature.h" // <--- 1. AJOUTER L'INCLUDE
#include "FluidsCreature.h"
#include "WancoCreature.h"
#include "DancingCreature.h"
#include "GekoManager.h" // <--- Remplacer GekoCreature.h par ça
#include "CousinCreature.h" // <--- AJOUT
#include "HaloCreature.h" // <--- AJOUT
#include "BreakableCreature.h" // <--- AJOUT


#include "LayerSystem.h"

class CreatureSystem {
public:
    void setup();
    void update(ofVec2f mouseWorld);
    void draw(ofVec2f mouseWorld);
    
    void onPress(float x, float y);
    void onRelease(float x, float y);
    void removeLast();
    void clear();

    void addDancingCreature(float x, float y);
    void addRandomCreature(float x, float y);
    void addCreature(float x, float y);
    void addRipple(float x, float y, float angle = 0.0f);
    void addSpringCreature(float x, float y);
    void addFluidsCreature(float x, float y);
    void addWancoCreature(float x, float y);
    void addGekoCreature(float x, float y);
    void addDoublePendulum(float x, float y); // <--- 2. AJOUTER LA METHODE
    void addCousinCreature(float x, float y); // <--- AJOUT
    void addHalo(float x, float y); // <--- AJOUT
    void addBreakableCreature(float x, float y); // <--- AJOUT

    ofImage sharedImage;
    ofImage sharedImage2;
    
    std::vector<std::unique_ptr<JellyCreature>> creatures;
    std::vector<std::unique_ptr<RippleCreature>> ripples;
    std::vector<std::unique_ptr<SpringCreature>> springs;
    std::vector<std::unique_ptr<FluidsCreature>> fluids;
    
    GekoManager gekoManager;

std::vector<std::unique_ptr<WancoCreature>> wancos; 
    std::vector<std::shared_ptr<DancingCreature>> dancingCreatures;
    std::vector<std::unique_ptr<DoublePendulumCreature>> pendulums; // <--- 3. AJOUTER LE VECTEUR
    std::vector<std::unique_ptr<CousinCreature>> cousins; // <--- AJOUT
    std::vector<std::unique_ptr<HaloCreature>> halos; // <--- AJOUT
    std::vector<std::unique_ptr<BreakableCreature>> breakables; // <--- AJOUT
};