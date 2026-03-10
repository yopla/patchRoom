#pragma once

#include "ofMain.h"
#include "ofJson.h"


class GeminiImageGenerator {
public:
    void setup(string apiKey);
    void generateImage(string prompt);
    void generateImage360(string prompt); // Nouvelle fonction pour image 360
    void generateNano360(string prompt); // Nouvelle fonction pour Nano Banana
    void generateImage360FromImage(string prompt, string imagePath); // Nouvelle fonction Image-to-Image 360
    void generateVideo(string prompt); // Nouvelle fonction pour Veo
    void listModels(); // Nouvelle fonction pour lister les modèles disponibles
    void update(); // Appeler dans ofApp::update()
    
    bool isGenerating() const { return bIsLoading; }
    bool hasNewImage() const { return bNewImageAvailable; }
    bool hasNew360Image() const { return bNew360ImageAvailable; }
    bool hasNewVideo() const { return bNewVideoAvailable; }
    
    // Récupère l'image et reset le flag hasNewImage
    ofImage& getImage();
    void clearImage();
    string get360ImagePath(); // Récupère le chemin de l'image 360 générée
    string getVideoPath(); // Récupère le chemin de la vidéo générée
void generateVideoFromImage(string prompt, string imagePath);
    void generateVideoFromDeuxImages(string prompt, string imagePath1, string imagePath2);
    void urlResponse(ofHttpResponse & response);
    
private:
    string apiKey;
    string apiUrl;
    string api360Url; // URL spécifique pour la génération 360
    string videoApiUrl; // URL spécifique pour la génération vidéo
    string nanoApiUrl; // URL spécifique pour Nano Banana
    
    bool bIsLoading;
    bool bIsPolling;      // Si on est en train d'attendre la vidéo
    bool bIsRequest360;   // Flag pour savoir si c'est une demande 360
    string operationName; // L'ID du job renvoyé par Google
    float lastPollTime;   // Pour ne pas spammer l'API
    void pollOperation(); // Fonction interne pour vérifier l'état

    bool bNewImageAvailable;
    bool bNew360ImageAvailable;
    bool bNewVideoAvailable;
    string videoFilePath;
    string image360FilePath;
    
    ofImage generatedImage;
    
    // Pour gérer les callbacks HTTP
    bool bRegistered;
    ofURLFileLoader loader;
};