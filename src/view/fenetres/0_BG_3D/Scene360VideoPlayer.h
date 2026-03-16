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
    void update();
    void start();
    void stop();
    void toggle();

    bool isActive() const { return bIsActive; }

private:
    void playNextVideo();
    void playVideo(int videoIndex);
    void parseFilename(const string& filename, string& start, string& end);

    AtmosphereSystem* atmosphere = nullptr;
    bool bIsActive = false;

    vector<VideoInfo> videos;
    map<string, vector<int>> videosByStartFrame;
    int currentVideoIndex = -1;
};
