#pragma once
#include "ofMain.h"
#include "RoomApp.h"
#include "Scene2D_SIDE.h" // <--- Nécessaire pour le shared_ptr
#include "Scene2DZenit.h" // <--- AJOUT

class CanvasManager {
public:
    // Initialise le FBO et charge le gabarit
    void setup(int w, int h);
    void update(); 

    void setPaused(bool bPause);
    // NOUVEAU : Pour gérer le drag & drop
    void loadFile(string path);

void drawBackground(shared_ptr<RoomApp> room, 
                        shared_ptr<Scene2D_SIDE> scene2D, 
                        shared_ptr<Scene2DZenit> sceneZenit, 
                        int gabAlpha,
                        bool bDrawRoom,    // <--- Ajout
                        bool bDrawZenit,   // <--- Ajout (Attention à l'ordre des args)
                        bool bDrawScene2D  // <--- Ajout
                        );

                        

    ofFbo canvas;
    float width, height;

private:
    ofImage imgFullGab;
    // NOUVEAU : Lecteur vidéo et état
    ofVideoPlayer videoPlayer;
    bool bIsVideo = false; // true si le fichier chargé est une vidéo
};