#pragma once
#include "ofMain.h"
#include "ColliderLayer.h" // Inclusion du collider partagé
#include "LayerSystem.h" // Inclure le fichier créé au dessus

enum ParticleType {
    SARDINE,
    SHARK
};

struct Boid {
    ofVec2f pos; 
    ofVec2f vel; 
    ofVec2f acc; 
    
    ParticleType type;
    int life = 1;
    ofColor color;
    
    Boid* neighbor = nullptr;
    float distSqToNeighbor = 0;
    float dxToNeighbor = 0;
    float dyToNeighbor = 0;

    int mapX = -1;
    int mapY = -1;
};

class FishSchoolLayer : public BaseLayer {
public:
    // On passe le pointeur vers les colliders dans le setup
    void setup(float realSceneWidth, float targetRealHeight, shared_ptr<ColliderLayer> colliders);
    
    void update(float mouseX, float mouseY) override {
        update(); 
    }

    void update();
    
    void mousePressed(float x, float y, int button) override {
         float layerY = y - (1472 - 830); 
         if(layerY > 0 && layerY < 830) {
            if(button == 0) addSardine(x, layerY); 
            if(button == 2) addShark(x, layerY); 
        }
    }

    
   void draw() override; // Déjà bon

    void addShark(float realMouseX, float realMouseY);
    void addSardine(float realMouseX, float realMouseY);

    int getSardineCount();
    int getSharkCount();

private:
    float simWidth;
    float simHeight;
    float scale; 

    float RANGE = 50.0f; 
    float DRAG = 0.92f; 

    vector<Boid> particles;
    int maxParticles = 1500; 
    
    vector<vector<vector<Boid*>>> grid;
    int cols, rows;

    // Référence vers les colliders partagés
    shared_ptr<ColliderLayer> colliderLayer;

    void updateGrid();
    void calculateNeighbors();
    void moveParticles();
    
    void behaviorSardine(Boid& p);
    void behaviorShark(Boid& p);
    
    // Gestion physique
    void avoidWalls(Boid& p);   // Utilise colliderLayer

    void thrust(Boid& p, float f, float r);
    void guide(Boid& p, float pull, float push);
    void escape(Boid& p, float f);
    void chase(Boid& p, float f);
    void wall(Boid& p, float push, float r); 
    void limitSpeed(Boid& p, float minSpeed, float maxSpeed);
};