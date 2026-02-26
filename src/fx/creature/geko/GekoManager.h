#pragma once
#include "ofMain.h"
#include "GekoUnits.h"

class GekoManager {
public:
    void update(float mx, float my);
    void draw();
    
    // Ajoute un gecko en alternance de type
    void addGeko(float x, float y);
    void clear();
    void removeLast();

    // Conteneur polymorphique
    std::vector<std::unique_ptr<GekoBase>> gekos;


    
    
private:
    int typeCounter = 0; // Pour alterner les types à la création
};