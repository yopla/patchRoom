#include "Scene360VideoPlayer.h"
#include "AtmosphereSystem.h" // Inclusion complète ici pour l'implémentation

void Scene360VideoPlayer::setup(AtmosphereSystem* atm, const string& videoFolderPath) {
    atmosphere = atm;
    if (!atmosphere) {
        ofLogError("Scene360VideoPlayer") << "Le pointeur vers AtmosphereSystem est nul !";
        return;
    }
    loadPlaylist(videoFolderPath);
}

void Scene360VideoPlayer::startPlaylist(const string& videoFolderPath) {
    if (!atmosphere) {
        ofLogError("Scene360VideoPlayer") << "Le pointeur vers AtmosphereSystem est nul !";
        return;
    }
    loadPlaylist(videoFolderPath);
    
    if (!videos.empty()) {
        start();
    } else {
        ofLogWarning("Scene360VideoPlayer") << "Aucune vidéo valide trouvée dans " << videoFolderPath << ", arrêt du lecteur.";
        stop();
    }
}

void Scene360VideoPlayer::loadPlaylist(const string& videoFolderPath) {
    // On vide les anciennes données
    videos.clear();
    videosByStartFrame.clear();
    currentVideoIndex = -1;
    folderPath = videoFolderPath;
    
    ofDirectory dir(folderPath);
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
    
    ofLogNotice("Scene360VideoPlayer") << "Chargé " << videos.size() << " vidéos depuis " << folderPath;
}

void Scene360VideoPlayer::refreshPlaylist() {
    if (folderPath.empty()) return;
    string currentPath = "";
    if (currentVideoIndex >= 0 && currentVideoIndex < videos.size()) {
        currentPath = videos[currentVideoIndex].path;
    }
    loadPlaylist(folderPath); // recharge et remet currentVideoIndex à -1
    if (!currentPath.empty()) {
        for (int i = 0; i < videos.size(); i++) {
            if (videos[i].path == currentPath) {
                currentVideoIndex = i;
                break;
            }
        }
    }
    determineNextVideo();
}

void Scene360VideoPlayer::toggleSimulate32Videos() {
    bSimulate32Videos = !bSimulate32Videos;
    if (bSimulate32Videos) {
        videos.clear();
        videosByStartFrame.clear();
        currentVideoIndex = -1;
        upcomingVideoIndex = -1;
        mockPosition = 0.0f;
        
        // Génération de 32 fausses vidéos
        for (int i = 0; i < 32; i++) {
            VideoInfo info;
            info.path = "mock_" + ofToString(i) + ".mp4";
            info.index = i;
            
            // Création d'un graphe cohérent (8 noeuds de base pour assurer qu'il n'y a pas de culs-de-sac)
            if (i < 8) {
                info.startFrame = "Node" + ofToString(i);
                info.endFrame = "Node" + ofToString((i + 1) % 8);
            } else {
                info.startFrame = "Node" + ofToString((int)ofRandom(8));
                info.endFrame = "Node" + ofToString((int)ofRandom(8));
            }
            
            videos.push_back(info);
            videosByStartFrame[info.startFrame].push_back(i);
        }
        if (!bIsActive) bIsActive = true;
        playVideo(0);
    } else {
        refreshPlaylist();
    }
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

    if (bSimulate32Videos) {
        mockPosition += 0.005f; // Vitesse de lecture simulée (environ 200 frames)
        if (mockPosition >= 1.0f) {
            mockPosition = 0.0f;
            playNextVideo();
        }
        return;
    }

    // Si on est en pause sur l'image fixe, on décrémente le compteur
    if (bIsPaused) {
        pauseCounter--;
        
        // --- NOUVEAU : Préchargement intelligent ---
        // Dès la première frame de pause, on lance le chargement de la vidéo suivante.
        // Le léger gel du chargement est masqué car l'écran est figé sur l'image de pause.
        if (pauseCounter == pauseDurationFrames - 1) {
            playNextVideo();
        }
        
        if (atmosphere) {
            // On force la pause en continu car ofVideoPlayer peut mettre quelques frames à charger
            if (atmosphere->bIsVideo) {
                atmosphere->video360.setVolume(0.0f); // Coupe le son pendant le préchargement caché
                if (atmosphere->video360.isLoaded()) {
                    atmosphere->video360.setPaused(true);
                }
            }
            // Force l'affichage de l'image fixe pendant toute la durée de la pause
            atmosphere->bShowLastFrame = true;
        }

        if (pauseCounter <= 0) {
            bIsPaused = false;
            // Fin de la pause, on relance la vidéo préchargée
            if (atmosphere) {
                if (atmosphere->bIsVideo && atmosphere->video360.isLoaded()) {
                    atmosphere->video360.firstFrame(); // Force le retour à la frame 0 pour compenser l'avancée du chargement asynchrone
                    atmosphere->video360.setPaused(false);
                    atmosphere->video360.setVolume(1.0f); // Rétablit le son au démarrage
                }
            }
        }
        return; // On ne fait rien d'autre pendant la pause
    }

    bool bMovieDone = false;
    if (atmosphere->bIsVideo && atmosphere->video360.isLoaded()) {
        bMovieDone = atmosphere->video360.getIsMovieDone();
        if (!bMovieDone) {
            int curFrame = atmosphere->video360.getCurrentFrame();
            int totFrames = atmosphere->video360.getTotalNumFrames();
            // Détection robuste de la fin : si on est à la toute dernière frame
            if ((totFrames > 0 && curFrame >= totFrames - 1) || atmosphere->video360.getPosition() >= 0.995f) {
                bMovieDone = true;
            }
        }
    }

    if (bMovieDone) {
        const string& endFrame = videos[currentVideoIndex].endFrame;
        
        ofImage endImage;
        bool bImageLoaded = false;
        string foundPath = "";
        
        // On teste toutes les extensions courantes (y compris avec majuscules)
        vector<string> extensions = {".png", ".jpg", ".jpeg", ".PNG", ".JPG", ".JPEG"};
        for (const string& ext : extensions) {
            string testPath = ofFilePath::join(folderPath, endFrame + ext);
            if (ofFile(testPath).exists()) {
                foundPath = testPath;
                break;
            }
        }
        
        if (!foundPath.empty()) {
            if (endImage.load(foundPath)) {
                bImageLoaded = true;
                ofLogNotice("Scene360VideoPlayer") << "Image fixe chargée pour la pause : " << foundPath;
            } else {
                ofLogError("Scene360VideoPlayer") << "Fichier trouve sur le disque mais impossible a charger (format invalide/corrompu ?) : " << foundPath;
            }
        }
        
        if (bImageLoaded) {
            if (!atmosphere->lastFrameFbo.isAllocated() || 
                atmosphere->lastFrameFbo.getWidth() != endImage.getWidth() || 
                atmosphere->lastFrameFbo.getHeight() != endImage.getHeight()) {
                ofFbo::Settings s;
                s.width = endImage.getWidth();
                s.height = endImage.getHeight();
                s.internalformat = GL_RGB;
                atmosphere->lastFrameFbo.allocate(s);
            }
            atmosphere->lastFrameFbo.begin();
            ofClear(0);
            ofPushStyle();
            ofSetColor(255);
            endImage.draw(0, 0);
            ofPopStyle();
            atmosphere->lastFrameFbo.end();
            atmosphere->bShowLastFrame = true;
        } else {
            atmosphere->holdLastFrame(); // On capture la dernière image si aucune image trouvée
        }
        
        if (pauseDurationFrames > 0 && !bLoopMode) {
            bIsPaused = true;
            pauseCounter = pauseDurationFrames;
            if (atmosphere && atmosphere->bIsVideo) {
                atmosphere->video360.setVolume(0.0f); // Coupe immédiatement le son de l'ancienne vidéo
            }
            refreshPlaylist(); // Recharge les noms à chaque image fixe
        } else {
            refreshPlaylist(); // Recharge même si pas de délai de pause
            playNextVideo(); // Enchaînement immédiat si pauseDurationFrames == 0
        }
    }
}

void Scene360VideoPlayer::start() {
    if (videos.empty()) {
        ofLogWarning("Scene360VideoPlayer") << "Aucune vidéo à lire.";
        return;
    }
    bIsActive = true;
    bIsPaused = false;
    // Commence par une vidéo aléatoire
    int randomIndex = ofRandom(videos.size());
    playVideo(randomIndex);
    ofLogNotice("Scene360VideoPlayer") << "Lecture démarrée.";
}

void Scene360VideoPlayer::stop() {
    bIsActive = false;
    currentVideoIndex = -1;
    bIsPaused = false;
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

void Scene360VideoPlayer::toggleLoopMode() {
    bLoopMode = !bLoopMode;
    if (!bUserSelectedNext) {
        upcomingVideoIndex = -1; // Force le calcul d'une nouvelle destination
    }
    determineNextVideo();
}

void Scene360VideoPlayer::playNextVideo() {
    if (currentVideoIndex < 0 || currentVideoIndex >= videos.size()) {
        // Index invalide, on recommence avec une vidéo aléatoire
        start();
        return;
    }

    if (upcomingVideoIndex != -1) {
        playVideo(upcomingVideoIndex);
    } else {
        // Aucune vidéo ne commence par cette image de fin, on en joue une complètement au hasard
        ofLogWarning("Scene360VideoPlayer") << "Aucune vidéo suivante pre-calculee. Lecture d'une vidéo aléatoire.";
        start();
    }
}

void Scene360VideoPlayer::playVideo(int videoIndex) {
    if (videoIndex < 0 || videoIndex >= videos.size()) return;

    currentVideoIndex = videoIndex;
    const auto& videoInfo = videos[currentVideoIndex];
    
    bUserSelectedNext = false; // Réinitialise le forçage manuel une fois que la vidéo démarre
    upcomingVideoIndex = -1;   // Réinitialise la prédiction pour le tour suivant
    
    if (bSimulate32Videos) {
        mockPosition = 0.0f;
    } else if (atmosphere) {
        ofLogNotice("Scene360VideoPlayer") << "Lecture de : " << videoInfo.path;
        atmosphere->loadTexture(videoInfo.path);
        
        if (bIsPaused) {
            atmosphere->video360.setVolume(0.0f);
        } else {
            atmosphere->video360.setVolume(1.0f);
        }
    }
    determineNextVideo();
}

float Scene360VideoPlayer::getVideoPosition() const {
    if (bSimulate32Videos) return mockPosition;
    if (bIsPaused) return 1.0f;
    if (atmosphere && atmosphere->bIsVideo && atmosphere->video360.isLoaded()) {
        return atmosphere->video360.getPosition();
    }
    return 0.0f;
}

bool Scene360VideoPlayer::forceNextVideoToNode(const string& targetEndNode) {
    if (currentVideoIndex < 0 || currentVideoIndex >= videos.size()) return false;
    
    const string& endFrame = videos[currentVideoIndex].endFrame;
    auto it = videosByStartFrame.find(endFrame);
    
    if (it != videosByStartFrame.end()) {
        vector<int> candidates;
        for (int idx : it->second) {
            if (videos[idx].endFrame == targetEndNode) {
                candidates.push_back(idx);
            }
        }
        if (!candidates.empty()) {
            upcomingVideoIndex = candidates[ofRandom(candidates.size())];
            bUserSelectedNext = true;
            return true;
        }
    }
    return false; // Ce noeud n'est pas atteignable depuis la fin de la vidéo actuelle
}

void Scene360VideoPlayer::determineNextVideo() {
    if (currentVideoIndex < 0 || currentVideoIndex >= videos.size()) {
        upcomingVideoIndex = -1;
        bUserSelectedNext = false;
        return;
    }

    const string& endFrame = videos[currentVideoIndex].endFrame;
    
    if (bUserSelectedNext && upcomingVideoIndex != -1) {
        if (videos[upcomingVideoIndex].startFrame == endFrame) {
            return; // On conserve le choix de l'utilisateur
        } else {
            bUserSelectedNext = false;
        }
    }
    
    auto it = videosByStartFrame.find(endFrame);
    vector<int> possibleNext;
    
    if (it != videosByStartFrame.end() && !it->second.empty()) {
        if (bLoopMode) {
            for (int idx : it->second) {
                if (videos[idx].endFrame == endFrame) possibleNext.push_back(idx);
            }
            if (possibleNext.empty()) possibleNext = it->second; // Fallback
        } else {
            possibleNext = it->second;
        }
    }
    
    if (possibleNext.empty()) {
        upcomingVideoIndex = -1;
        return;
    }
    
    bool keepCurrent = false;
    if (upcomingVideoIndex != -1) {
        for (int idx : possibleNext) if (idx == upcomingVideoIndex) { keepCurrent = true; break; }
    }
    
    if (!keepCurrent) {
        // On groupe d'abord par destination (nœud de fin) pour ne pas favoriser un chemin
        // simplement parce qu'il possède de multiples variations.
        map<string, vector<int>> pathsByDestination;
        for (int idx : possibleNext) {
            pathsByDestination[videos[idx].endFrame].push_back(idx);
        }
        
        vector<string> destinations;
        for (auto& pair : pathsByDestination) {
            destinations.push_back(pair.first);
        }
        
        // On choisit une destination au hasard, puis une variation au hasard pour ce trajet
        string chosenDestination = destinations[ofRandom(destinations.size())];
        vector<int>& variations = pathsByDestination[chosenDestination];
        upcomingVideoIndex = variations[ofRandom(variations.size())];
    }
}
