#pragma once

#include "ofMain.h"
#include "ofJson.h"


class GeminiImageGenerator {
public:
    void setup(string apiKey);
    void generateImage(string prompt);
    void generateVideo(string prompt); // Nouvelle fonction pour Veo
    void listModels(); // Nouvelle fonction pour lister les modèles disponibles
    void update(); // Appeler dans ofApp::update()
    
    bool isGenerating() const { return bIsLoading; }
    bool hasNewImage() const { return bNewImageAvailable; }
    bool hasNewVideo() const { return bNewVideoAvailable; }
    
    // Récupère l'image et reset le flag hasNewImage
    ofImage& getImage();
    string getVideoPath(); // Récupère le chemin de la vidéo générée

    void urlResponse(ofHttpResponse & response);
    
private:
    string apiKey;
    string apiUrl;
    string videoApiUrl; // URL spécifique pour la génération vidéo
    
    bool bIsLoading;
    bool bIsPolling;      // Si on est en train d'attendre la vidéo
    string operationName; // L'ID du job renvoyé par Google
    float lastPollTime;   // Pour ne pas spammer l'API
    void pollOperation(); // Fonction interne pour vérifier l'état

    bool bNewImageAvailable;
    bool bNewVideoAvailable;
    string videoFilePath;
    
    ofImage generatedImage;
    
    // Pour gérer les callbacks HTTP
    bool bRegistered;
    ofURLFileLoader loader;
};