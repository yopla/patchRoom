#pragma once
#include "ofMain.h"

// Forward-déclaration pour éviter d'inclure l'en-tête complet ici
class AtmosphereSystem;

class Scene360VideoPlayer {
public:
    struct VideoInfo {
        string path;
        string startFrame;
        string endFrame;
        int index;
    };

    void setup(AtmosphereSystem* atmosphere, const string& videoFolderPath);
    void startPlaylist(const string& videoFolderPath);
    void update();
    void start();
    void stop();
    void toggle();
    
    void toggleLoopMode();
    bool isLoopMode() const { return bLoopMode; }

    void toggleSimulate32Videos();
    bool isSimulating32Videos() const { return bSimulate32Videos; }

    void toggleDoubleSpeed();
    bool isDoubleSpeed() const { return bDoubleSpeed; }

    void toggleMute();
    bool isMuted() const { return bMuted; }

    void toggleCrop106() { bCrop106 = !bCrop106; }
    bool isCrop106() const { return bCrop106; }

    // Durée de la pause sur l'image fixe avant la prochaine vidéo (en frames)
    int pauseDurationFrames = 150;
    
    // Pause infinie
    bool isInfinitePause() const { return bInfinitePause; }
    void toggleInfinitePause() { bInfinitePause = !bInfinitePause; }

    // Accesseurs pour le visualiseur
    const vector<VideoInfo>& getVideos() const { return videos; }
    int getCurrentVideoIndex() const { return currentVideoIndex; }
    bool isPaused() const { return bIsPaused; }
    float getVideoPosition() const;
    int getUpcomingVideoIndex() const { return upcomingVideoIndex; }
    void refreshPlaylist(); // Recharge les noms des fichiers
    
    bool planPathToNode(const string& targetEndNode);
    const vector<int>& getPlannedPath() const { return plannedPath; }
    bool isUserSelectedNext() const { return bUserSelectedNext; }

    bool isActive() const { return bIsActive; }

private:
    void loadPlaylist(const string& videoFolderPath);
    void playNextVideo();
    void playVideo(int videoIndex);
    void parseFilename(const string& filename, string& start, string& end);
    void determineNextVideo();

    AtmosphereSystem* atmosphere = nullptr;
    bool bIsActive = false;

    vector<VideoInfo> videos;
    map<string, vector<int>> videosByStartFrame;
    int currentVideoIndex = -1;
    string folderPath;
    int upcomingVideoIndex = -1;
    
    bool bIsPaused = false;
    bool bInfinitePause = false;
    bool bLoopMode = false;
    bool bDoubleSpeed = false;
    bool bMuted = false;
    bool bUserSelectedNext = false;
    bool bSimulate32Videos = false;
    bool bCrop106 = true; // Grossit l'image de 106% pour matcher le raccord avec la vidéo
    float mockPosition = 0.0f;
    float lastVideoPosition = 0.0f; // Sauvegarde de la position pour détecter les boucles forcées
    int pauseCounter = 0;
    vector<int> plannedPath;
};
