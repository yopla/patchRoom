#include "CreatureSystem.h"

//--------------------------------------------------------------
void CreatureSystem::setup() {
    // 1. Chargement de l'image partagée 1
    bool loaded = sharedImage.load("IMG_REF/00029-2795021161.png");
    if (!loaded) {
        sharedImage.allocate(400, 400, OF_IMAGE_COLOR);
        ofPixels & pixels = sharedImage.getPixels();
        for(int i = 0; i < pixels.size(); i += 3){
            pixels[i] = ofRandom(100, 255);     
            pixels[i+1] = ofRandom(100, 200);   
            pixels[i+2] = ofRandom(200, 255);   
        }
        sharedImage.update();
    }

    // 2. Chargement de l'image partagée 2
    bool loaded2 = sharedImage2.load("IMG_REF/vmf.png");
     if (!loaded2) {
        sharedImage2.allocate(400, 400, OF_IMAGE_COLOR);
        ofPixels & pixels = sharedImage2.getPixels();
        for(int i = 0; i < pixels.size(); i += 3){
            pixels[i] = ofRandom(100, 255);     
            pixels[i+1] = ofRandom(100, 200);   
            pixels[i+2] = ofRandom(200, 255);   
        }
        sharedImage2.update();
    }
}

//--------------------------------------------------------------
void CreatureSystem::update(ofVec2f mouseWorld, float time) {
    
    // Mise à jour de toutes les listes
    
    for(auto& w : wancos) {
        w->update(mouseWorld.x, mouseWorld.y);
    }

    for(auto& c : dancingCreatures) {
        c->update(mouseWorld.x, mouseWorld.y, time);
    }

    for(auto& r : ripples) {
        r->checkInput(mouseWorld.x, mouseWorld.y);
        r->update();
    }

    for(auto& s : springs) {
        s->update(mouseWorld.x, mouseWorld.y);
    }

    for(auto& c : creatures) { // Jellys
        c->update(mouseWorld.x, mouseWorld.y);
    }

    for(auto& f : fluids) {
        f->update(mouseWorld.x, mouseWorld.y);
    }
    

gekoManager.update(mouseWorld.x, mouseWorld.y);    
    
    // Nouveau : Double Pendule
    for(auto& p : pendulums) {
        p->update(mouseWorld.x, mouseWorld.y);
    }
    
    for(auto& c : cousins) {
        c->update(mouseWorld.x, mouseWorld.y);
    }
    
    for(auto& h : halos) {
        h->update(time);
    }
    
    for(auto& b : breakables) {
        b->update(mouseWorld.x, mouseWorld.y);
    }

    for(auto& o : otaries) {
        o->update(mouseWorld.x, mouseWorld.y);
    }
    
    // Nettoyage automatique : on retire les créatures qui sont brisées et dont tous les morceaux ont disparu
    breakables.erase(std::remove_if(breakables.begin(), breakables.end(),
        [](const std::unique_ptr<BreakableCreature>& b){ return b->isFinished(); }),
        breakables.end());
}

//--------------------------------------------------------------
void CreatureSystem::draw(ofVec2f mouseWorld) {
    // Ordre de rendu (Z-Index) du fond vers l'avant
    
    // 1. Fond : Ondes
    for(const auto& r : ripples) {
        r->draw();
    }

    // 2. Plan intermédiaire : Créatures diverses
    for(const auto& w : wancos) w->draw();
    for(const auto& c : dancingCreatures) c->draw();
    for(const auto& f : fluids) f->draw();
   gekoManager.draw();
    for(const auto& h : halos) h->draw(); // <--- AJOUT

    // 3. Plan physique : Springs et Pendules
    for(const auto& s : springs) s->draw();
    for(const auto& p : pendulums) p->draw(); // <--- Nouveau

    // 4. Premier plan : Jellys (Méduses)
    for(const auto& c : creatures) {
        c->draw(mouseWorld.x, mouseWorld.y); 
    }
    
    // 5. Cousins (Premier plan aussi)
    for(const auto& c : cousins) c->draw();
    
    // 6. Breakables
    for(const auto& b : breakables) b->draw();

    // 7. Otaries
    for(const auto& o : otaries) o->draw();
}

//--------------------------------------------------------------
void CreatureSystem::onPress(float x, float y) {
    bool handled = false;

    // 1. Jellys (Priorité Haute - Premier plan)
    for (int i = creatures.size() - 1; i >= 0; i--) {
        if (creatures[i]->isInside(x, y)) {
            creatures[i]->onPress(x, y);
            handled = true;
            break; 
        }
    }

    // 2. Springs
    if (!handled) {
        for (int i = springs.size() - 1; i >= 0; i--) {
            if (springs[i]->isInside(x, y)) {
                springs[i]->onPress(x, y);
                handled = true;
                break;
            }
        }
    }
    
    // 3. Double Pendules
    if (!handled) {
        for (int i = pendulums.size() - 1; i >= 0; i--) {
            if (pendulums[i]->isInside(x, y)) {
                pendulums[i]->onPress(x, y);
                handled = true;
                break;
            }
        }
    }
    
    // 4. Cousins
    if (!handled) {
        for (int i = cousins.size() - 1; i >= 0; i--) {
            if (cousins[i]->isInside(x, y)) {
                cousins[i]->onPress(x, y);
                handled = true;
                break;
            }
        }
    }
    
    // 5. Breakables
    if (!handled) {
        for (int i = breakables.size() - 1; i >= 0; i--) {
            if (breakables[i]->isInside(x, y)) {
                breakables[i]->onPress(x, y);
                handled = true;
                break;
            }
        }
    }
    
    // Note: Les autres créatures (Geko, Fluids...) n'ont pas d'interaction définie ici
}

//--------------------------------------------------------------
void CreatureSystem::onRelease(float x, float y) {
    // On relâche tout le monde pour éviter les bugs de drag
    for(const auto& c : creatures) c->onRelease(x, y);
    for(const auto& s : springs) s->onRelease(x, y);
    for(const auto& p : pendulums) p->onRelease(x, y); // <--- Nouveau
    for(const auto& c : cousins) c->onRelease(x, y);
    for(const auto& b : breakables) b->onRelease(x, y);
}

//--------------------------------------------------------------
// AJOUTS DE CRÉATURES
//--------------------------------------------------------------

void CreatureSystem::addCreature(float x, float y) {
    // Méduse (Jelly)
    float w = 500; float h = 500;
    creatures.push_back(std::make_unique<JellyCreature>(x - w/2, y - h/2, w, h, sharedImage2));
}

void CreatureSystem::addRandomCreature(float x, float y) {
    addRipple(x,y);
}

void CreatureSystem::addRipple(float x, float y, float angle) {
    float w = 500; float h = 500;
    auto r = std::make_unique<RippleCreature>(x - w/2, y - h/2, w, h, sharedImage);
    r->rotation = angle;
    ripples.push_back(std::move(r));
}

void CreatureSystem::addSpringCreature(float x, float y) {
    float w = 400; float h = 400;
    springs.push_back(std::make_unique<SpringCreature>(x - w/2, y - 50));
}

void CreatureSystem::addDancingCreature(float x, float y) {
    dancingCreatures.push_back(make_shared<DancingCreature>(x, y));
}

void CreatureSystem::addWancoCreature(float x, float y) {
    wancos.push_back(std::make_unique<WancoCreature>(x, y, sharedImage));
}

void CreatureSystem::addFluidsCreature(float x, float y) {
    float w = 500; float h = 500;
    fluids.push_back(std::make_unique<FluidsCreature>(x - w/2, y - h/2, w, h, sharedImage));
}

void CreatureSystem::addGekoCreature(float x, float y) {
   gekoManager.addGeko(x, y); // Délègue au manager
}

void CreatureSystem::addDoublePendulum(float x, float y) {
    pendulums.push_back(std::make_unique<DoublePendulumCreature>(x, y));
}

void CreatureSystem::addCousinCreature(float x, float y) {
    cousins.push_back(std::make_unique<CousinCreature>(x, y));
}

void CreatureSystem::addHalo(float x, float y) {
    halos.push_back(std::make_unique<HaloCreature>(x, y));
}

void CreatureSystem::addBreakableCreature(float x, float y) {
    float w = 400; float h = 400;
    breakables.push_back(std::make_unique<BreakableCreature>(x - w/2, y - h/2, w, h, &sharedImage));
}

void CreatureSystem::addOtarieCreature(float x, float y) {
    auto o = std::make_unique<OtarieCreature>(x, y);
    if(colliderLayer) o->setCollider(colliderLayer);
    otaries.push_back(std::move(o));
}

void CreatureSystem::setCollider(shared_ptr<ColliderLayer> c) {
    colliderLayer = c;
    for(auto& o : otaries) {
        o->setCollider(c);
    }
}

//--------------------------------------------------------------
// GESTION LISTE
//--------------------------------------------------------------

void CreatureSystem::removeLast() {
    // Priorité de suppression
    if (!creatures.empty()) {
        creatures.pop_back();
    } else if (!springs.empty()) {
        springs.pop_back();
    } else if (!halos.empty()) {
        halos.pop_back();
    } else if (!cousins.empty()) {
        cousins.pop_back();
    } else if (!pendulums.empty()) {
        pendulums.pop_back();
    } else if (!breakables.empty()) {
        breakables.pop_back();
    } else if (!otaries.empty()) {
        otaries.pop_back();
    } else if (!gekoManager.gekos.empty()) { // Accès direct au vecteur si public
        gekoManager.removeLast();
    } else if (!dancingCreatures.empty()) {
        dancingCreatures.pop_back();
    } else if (!wancos.empty()) {
        wancos.pop_back();
    } else if (!fluids.empty()) {
        fluids.pop_back();
    } else if (!ripples.empty()) {
        ripples.pop_back();
    }
}

void CreatureSystem::clear() {
    creatures.clear(); 
    springs.clear();
    ripples.clear();
    fluids.clear();
    wancos.clear();
    dancingCreatures.clear();
gekoManager.clear();
    pendulums.clear();
    cousins.clear();
    halos.clear();
    breakables.clear();
    otaries.clear();
}