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

    // Durée de la pause sur l'image fixe avant la prochaine vidéo (en frames)
    int pauseDurationFrames = 150;

    // Accesseurs pour le visualiseur
    const vector<VideoInfo>& getVideos() const { return videos; }
    int getCurrentVideoIndex() const { return currentVideoIndex; }
    bool isPaused() const { return bIsPaused; }
    float getVideoPosition() const;
    int getUpcomingVideoIndex() const { return upcomingVideoIndex; }
    void refreshPlaylist(); // Recharge les noms des fichiers
    
    bool forceNextVideoToNode(const string& targetEndNode);
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
    bool bLoopMode = false;
    bool bUserSelectedNext = false;
    bool bSimulate32Videos = false;
    float mockPosition = 0.0f;
    int pauseCounter = 0;
};
