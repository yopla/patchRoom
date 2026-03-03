#pragma once
#include "ofMain.h"

// Forward declaration pour éviter une dépendance cyclique et alléger les en-têtes
class HaloCreature; 
class ButtonApp;
class Scene2D_SIDE;
class RoomApp;

class PerceptionSystem {
public:
    void setup();
    
    // Nouvelle méthode update de haut niveau qui orchestre tout
    void update(shared_ptr<ButtonApp> buttonApp, 
                shared_ptr<Scene2D_SIDE> sceneSide, 
                shared_ptr<RoomApp> roomApp);

private:
    // Méthode interne pour le calcul de distance
    void updateHalos(vector<shared_ptr<HaloCreature>>& halos, 
                const vector<glm::vec3>& halo3DPositions, 
                const vector<pair<glm::vec3, bool>>& allButtons);

    // Nouvelle méthode pour l'interaction avec les fluides
    void updateFluids(const vector<glm::vec3>& activeButtons3DPositions,
                      shared_ptr<Scene2D_SIDE> sceneSide,
                      shared_ptr<RoomApp> roomApp);

    // Nouvelle méthode pour le Poulpe
    void updatePoulpe(shared_ptr<Scene2D_SIDE> sceneSide, 
                      shared_ptr<RoomApp> roomApp, 
                      const vector<glm::vec3>& activeButtons3DPositions);

    float maxPerceptionRadius;
    int maxButtonsToConsider;
};
