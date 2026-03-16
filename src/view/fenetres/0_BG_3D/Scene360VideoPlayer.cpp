#include "Scene360VideoPlayer.h"
#include "AtmosphereSystem.h" // Inclusion complète ici pour l'implémentation

void Scene360VideoPlayer::setup(AtmosphereSystem* atm, const string& videoFolderPath) {
    atmosphere = atm;
    if (!atmosphere) {
        ofLogError("Scene360VideoPlayer") << "Le pointeur vers AtmosphereSystem est nul !";
        return;
    }

    ofDirectory dir(videoFolderPath);
    if (!dir.exists()) {
        ofLogError("Scene360VideoPlayer") << "Dossier vidéo non trouvé : " << videoFolderPath;
        return;
    }

    dir.allowExt("mp4");
    dir.allowExt("mov");
    dir.listDir();

    for (int i = 0; i < dir.size(); i++) {
        VideoInfo info;
        info.path = dir.getPath(i);
        info.index = videos.size(); // L'index dans le vecteur 'videos'
        parseFilename(dir.getName(i), info.startFrame, info.endFrame);

        if (!info.startFrame.empty() && !info.endFrame.empty()) {
            videos.push_back(info);
            // On stocke l'index de la vidéo dans la map, pas l'index du dossier
            videosByStartFrame[info.startFrame].push_back(info.index);
        }
    }

    ofLogNotice("Scene360VideoPlayer") << "Chargé " << videos.size() << " vidéos.";
}

void Scene360VideoPlayer::parseFilename(const string& filename, string& start, string& end) {
    string nameWithoutExt = ofFilePath::getBaseName(filename);
    vector<string> parts = ofSplitString(nameWithoutExt, "-");
    if (parts.size() >= 2) {
        start = parts[0];
        end = parts[1];
    } else {
        ofLogWarning("Scene360VideoPlayer") << "Impossible de parser le nom de fichier : " << filename;
    }
}

void Scene360VideoPlayer::update() {
    if (!bIsActive || !atmosphere || currentVideoIndex == -1) {
        return;
    }

    // Vérifie si la vidéo est terminée
    if (atmosphere->bIsVideo && atmosphere->video360.isLoaded() && atmosphere->video360.getIsMovieDone()) {
        atmosphere->holdLastFrame(); // On capture la dernière image avant de changer
        playNextVideo();
    }
}

void Scene360VideoPlayer::start() {
    if (videos.empty()) {
        ofLogWarning("Scene360VideoPlayer") << "Aucune vidéo à lire.";
        return;
    }
    bIsActive = true;
    // Commence par une vidéo aléatoire
    int randomIndex = ofRandom(videos.size());
    playVideo(randomIndex);
    ofLogNotice("Scene360VideoPlayer") << "Lecture démarrée.";
}

void Scene360VideoPlayer::stop() {
    bIsActive = false;
    currentVideoIndex = -1;
    if (atmosphere) {
        // Revient à la texture par défaut
        atmosphere->loadTexture("GAB0/VR0.jpg");
    }
    ofLogNotice("Scene360VideoPlayer") << "Lecture arrêtée, retour à la texture par défaut.";
}

void Scene360VideoPlayer::toggle() {
    if (bIsActive) {
        stop();
    } else {
        start();
    }
}

void Scene360VideoPlayer::playNextVideo() {
    if (currentVideoIndex < 0 || currentVideoIndex >= videos.size()) {
        // Index invalide, on recommence avec une vidéo aléatoire
        start();
        return;
    }

    const string& endFrame = videos[currentVideoIndex].endFrame;
    
    auto it = videosByStartFrame.find(endFrame);
    if (it != videosByStartFrame.end() && !it->second.empty()) {
        // On a trouvé des vidéos qui peuvent suivre
        const vector<int>& possibleNext = it->second;
        int randomIndexInList = ofRandom(possibleNext.size());
        int nextVideoIndex = possibleNext[randomIndexInList];
        playVideo(nextVideoIndex);
    } else {
        // Aucune vidéo ne commence par cette image de fin, on en joue une complètement au hasard
        ofLogWarning("Scene360VideoPlayer") << "Aucune vidéo trouvée commençant par '" << endFrame << "'. Lecture d'une vidéo aléatoire.";
        start();
    }
}

void Scene360VideoPlayer::playVideo(int videoIndex) {
    if (videoIndex < 0 || videoIndex >= videos.size()) return;

    currentVideoIndex = videoIndex;
    const auto& videoInfo = videos[currentVideoIndex];
    
    if (atmosphere) {
        ofLogNotice("Scene360VideoPlayer") << "Lecture de : " << videoInfo.path;
        atmosphere->loadTexture(videoInfo.path);
    }
}
