#include "GekoManager.h"

void GekoManager::update(float mx, float my) {
    // 1. Mise à jour individuelle (Mouvement vers la souris)
    for(auto& g : gekos) {
        g->update(mx, my);
    }

    // 2. GESTION DES COLLISIONS (Répulsion)
    // On compare chaque gecko avec tous les autres une seule fois
    for(int i = 0; i < gekos.size(); i++) {
        for(int j = i + 1; j < gekos.size(); j++) {
            
            // Accès direct grâce au 'friend class'
            GekoBase* g1 = gekos[i].get();
            GekoBase* g2 = gekos[j].get();

            glm::vec2 dir = g1->bodyPos - g2->bodyPos;
            float dist = glm::length(dir);
            
            // Distance minimum = Rayon 1 + Rayon 2 + Marge de sécurité (ex: 15px)
            // bodySize est le rayon approximatif du corps
            float minDistance = g1->bodySize + g2->bodySize + 55.0f;

            if (dist < minDistance && dist > 0.001f) {
                // Ils se chevauchent !
                
                // Calcul de la profondeur de la collision
                float overlap = minDistance - dist;
                
                // Direction normalisée de la poussée
                glm::vec2 pushDir = glm::normalize(dir);
                
                // Force de séparation (0.5 pour être assez ferme, mais fluide)
                glm::vec2 separationForce = pushDir * overlap * 0.15f; 

                // On pousse g1 dans un sens et g2 dans l'autre (Action/Réaction)
                g1->bodyVel += separationForce;
                g2->bodyVel -= separationForce;
                
                // Optionnel : On peut aussi forcer un peu la position directement 
                // pour éviter qu'ils ne "vibrent" s'ils sont très coincés.
                // g1->bodyPos += pushDir * overlap * 0.05f;
                // g2->bodyPos -= pushDir * overlap * 0.05f;
            }
        }
    }
}

void GekoManager::draw() {
    for(auto& g : gekos) {
        g->draw();
    }
}

void GekoManager::addGeko(float x, float y) {
    int type = typeCounter % 3;
    
    if (type == 0) {
        gekos.push_back(std::make_unique<GekoStandard>(x, y));
    } else if (type == 1) {
        gekos.push_back(std::make_unique<GekoAgile>(x, y));
    } else {
        gekos.push_back(std::make_unique<GekoOrbiter>(x, y));
    }
    
    typeCounter++;
}

void GekoManager::clear() {
    gekos.clear();
}

void GekoManager::removeLast() {
    if(!gekos.empty()) gekos.pop_back();
}