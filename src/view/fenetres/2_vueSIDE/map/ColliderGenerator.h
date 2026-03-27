#pragma once
#include "ofMain.h"

class ColliderGenerator {
public:
    // Analyse l'image overlay pour générer les contours et les sauvegarder
    static void generateAndSave(const ofImage& overlayImg, string outputPath);
    
    // Analyse l'image overlay via un modèle d'IA local (MiDaS Small) pour la profondeur
    static void generateWithAI(const ofImage& inputImg, string outputPath);
    
    // Analyse l'image overlay via le modèle EfficientSAM
    static void generateWithSAM(const ofImage& inputImg, string outputPath);
    
    // Analyse l'image overlay via le modèle Dexined (Edge Detection)
    static void generateWithDexined(const ofImage& inputImg, string outputPath);

private:
    static void saveFile(const ofPixels& pixels, string outputPath);
};