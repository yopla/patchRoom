#include "PlaylistTooltipManager.h"

PlaylistTooltipManager::PlaylistTooltipManager() {}

void PlaylistTooltipManager::setup() {
    // Vues
    texts["V1"] = "Active/Désactive l'affichage de la Vue 1.";
    texts["V2"] = "Active/Désactive l'affichage de la Vue 2.";
    texts["V3"] = "Active/Désactive l'affichage de la Vue 3.";
    texts["V4"] = "Active/Désactive l'affichage de la Vue 4.";
    texts["->V1"] = "Déplace la fenêtre de la Vue 1 (Rapprochement auto sur l'écran).";
    texts["->V2"] = "Déplace la fenêtre de la Vue 2 (Rapprochement auto sur l'écran).";
    texts["->V3"] = "Déplace la fenêtre de la Vue 3 (Rapprochement auto sur l'écran).";
    texts["->V4"] = "Déplace la fenêtre de la Vue 4 (Rapprochement auto sur l'écran).";
    texts["V1 WIN"] = "Masque physiquement la fenêtre de la Vue 1 au niveau de l'OS.";
    texts["V2 WIN"] = "Masque physiquement la fenêtre de la Vue 2 au niveau de l'OS.";
    texts["V3 WIN"] = "Masque physiquement la fenêtre de la Vue 3 au niveau de l'OS.";
    texts["V4 WIN"] = "Masque physiquement la fenêtre de la Vue 4 au niveau de l'OS.";
    
    texts["REC V1"] = "Enregistre la fenêtre Vue 1 (export/view_X_...).";
    texts["REC V2"] = "Enregistre la fenêtre Vue 2 (export/view_X_...).";
    texts["REC V3"] = "Enregistre la fenêtre Vue 3 (export/view_X_...).";
    texts["REC V4"] = "Enregistre la fenêtre Vue 4 (export/view_X_...).";
    texts["FMT_BTN"] = "Format de capture (PNG avec transparence ou JPG plus léger).";
    texts["QUAL_BTN"] = "Qualité de compression des images enregistrées (BEST = Lent mais parfait).";
    texts["ARRANGE_WINS"] = "Réorganise automatiquement les fenêtres principales (MAIN, W, X, C, V, B) en grille.";
    
    // WXCVB
    texts["MAIN"] = "Affiche/Cache le rendu du Canvas Master (Fenêtre principale).";
    texts["W:ROOM"] = "Affiche/Cache le rendu de la Room 3D (Touche W).";
    texts["X:ZENI"] = "Affiche/Cache le rendu de la vue Zénithale (Touche X).";
    texts["C:SCEN"] = "Affiche/Cache le rendu de la Scène 2D (Touche C).";
    texts["V:PREV"] = "Met en pause la fenêtre Preview pour économiser les perfs (Touche V).";
    texts["B:BTNS"] = "Affiche/Cache le rendu de la fenêtre des boutons OSC (Touche B).";
    
    texts["->MAIN"] = "Focus : Ramène la fenêtre Master au premier plan.";
    texts["->W"] = "Focus : Ramène la fenêtre Room 3D au premier plan.";
    texts["->X"] = "Focus : Ramène la fenêtre vue Zénithale au premier plan.";
    texts["->C"] = "Focus : Ramène la fenêtre Scène 2D au premier plan.";
    texts["->V"] = "Focus : Ramène la fenêtre Preview au premier plan.";
    texts["->B"] = "Focus : Ramène la fenêtre des boutons OSC au premier plan.";
    
    // GAB
    texts["GAB 0"] = "GAB 0 (Master) : Gère l'opacité globale du fond de la vue Master.";
    texts["GAB 1"] = "GAB 1 (Room) : Alterne l'affichage de l'arrière-plan 3D (Opaque, Transparent, etc.).";
    texts["GAB 2"] = "GAB 2 (Scène 2D) : Modifie le fond de la Scène 2D (Opaque, Transp, Wireframe).";
    texts["GAB 3"] = "GAB 3 (Overlay 2D) : Règle l'opacité du calque Overlay dans la Scène 2D.";
    texts["GAB 4"] = "GAB 4 (Zénith) : Modifie le fond de la vue Zénithale.";
    
    // Room
    texts["Sol 3D"] = "Active le sol ondulant en 3D dans la Room.";
    texts["Tangage"] = "Fait tanguer la Room entière comme sur un bateau.";
    texts["Kraken In"] = "Fait apparaître les tentacules du Kraken à l'intérieur de la Room.";
    texts["Kraken Ex"] = "Fait apparaître les tentacules du Kraken à l'extérieur de la Room.";
    texts["Nuages"] = "Affiche une sphère de nuages volumétriques flottante.";
    texts["Eau Sphere"] = "Affiche une sphère d'eau liquide flottante.";
    texts["Jelly Sph"] = "Affiche une sphère de Jellies interactifs (Touche 0).";
    texts["Vaisseaux"] = "Affiche une sphère texturée destructible par des petits vaisseaux.";
    texts["Faisceau"] = "Affiche le faisceau volumétrique du vidéoprojecteur virtuel.";
    texts["Prev 360"] = "Affiche la sphère d'environnement 360 dans la vue de prévisualisation.";
    texts["Texture ON"] = "Bascule entre la texture chargée (ON) et le damier par défaut (OFF) pour la sphère.";
    texts["Box Checker"] = "Affiche une boîte texturée en damier juste derrière les murs de la Room.";
    texts["JupyterBox"] = "Affiche une simulation de mini-jeu de stratégie sur les murs de la Room.";
    texts["G.O.L Box"] = "Affiche une simulation du Jeu de la Vie (Game of Life chromatique) sur les murs.";
    texts["GOL Motion"] = "Affiche une simulation du Jeu de la Vie avec particules animées (2PClm).";
    texts["AutoSnake Box"] = "Affiche une simulation de Snake autonome 3D sur la boite.";
    texts["Snake Interact"] = "Active l'interaction au clic sur la boite 3D pour placer le prochain objectif du serpent.";
    texts["AutoS Reset"] = "Reinitialise le AutoSnake.";
    texts["AutoS Pause"] = "Met en pause ou reprend le AutoSnake.";
    texts["Plafond"] = "Affiche ou cache le plafond 3D de la pièce.";
    texts["Respiration"] = "Ajoute un léger mouvement d'oscillation verticale à la caméra (respiration).";
    texts["Vagues"] = "Affiche un effet de vagues d'eau virtuelles au sol.";
    texts["Vers Murs"] = "Fait ramper des vers lumineux sur les murs de la Room.";
    texts["Vers Vol"] = "Fait voler des vers lumineux à l'intérieur de la pièce.";
    texts["Portail"] = "Affiche l'anneau de portail fluide 3D.";
    texts["Lucioles"] = "Affiche un essaim de petites lumières volantes au centre.";
    texts["Cur Alpha"] = "Rend le curseur carré projeté plus transparent.";
    texts["Gen 360"] = "Génère une image 360 (équirectangulaire) de la pièce actuelle avec les images par défaut.";
    texts["Gen 360 F"] = "Génère une image 360 en utilisant les textures actuelles (FBOs Scene 2D) projetées sur les murs.";
    texts["Gen 360 W"] = "Génère une image 360 complète de toute la scène 3D (Inclut Kraken, Plan Collé, etc.).";
    texts["Beams"] = "Affiche ou cache les faisceaux de projection 3D (Touche T).";
    texts["Plan Colle"] = "Affiche ou cache le plan de projection collé aux murs (Touche N).";
    texts["Cur Reflet"] = "Active/Désactive le débordement du curseur sur les murs adjacents (Reflets).";
    texts["Prev Inter"] = "Visualise la zone d'interaction dans la fenêtre RoomPreview (Touche B).";
    texts["Prev Curs"] = "Affiche le curseur projeté dans la fenêtre RoomPreview (Touche S).";

    texts["Reset Cam [R]"] = "Réinitialise la position et l'angle de la caméra 3D globale.";
    texts["Lock Cam [L]"] = "Verrouille la caméra au centre de la pièce pour un point de vue 360 (Touche L).";
    texts["Sphere [P]"] = "Affiche la sphère d'environnement 3D avec texture (Touche P).";
    texts["Disco [M]"] = "Affiche une boule à facettes (Disco Ball) au centre (Touche M).";
    texts["Sph. 360 [O]"] = "Bascule la sphère d'environnement en mode projection vidéo 360 (Touche O).";
    texts["Auto Rot [I]"] = "Active/Désactive la rotation automatique de l'environnement 360 (Touche I).";
    texts["Del Flys [X]"] = "Supprime toutes les lucioles (Light Flys) présentes dans la scène.";
    
    texts["Jupy Reset"] = "Relance la simulation JupyterBox et redistribue les 60 bases aléatoirement.";
    texts["Jupy Pause"] = "Met en pause ou reprend l'écoulement du temps pour le combat de JupyterBox.";
    texts["G.O.L Reset"] = "Réinitialise le Jeu de la Vie avec une nouvelle répartition aléatoire.";
    texts["GOLM Reset"] = "Réinitialise le Jeu de la Vie animé avec une nouvelle répartition.";
    texts["GOLM Pause"] = "Met en pause ou reprend l'animation de GOL Motion.";
    
    // Player
    texts["LOOP"] = "Joue la vidéo actuelle en boucle continue sans passer à la suivante.";
    texts["PLAY"] = "Joue ou met en pause l'enchaînement automatique de la playlist.";
    texts["SIMU"] = "Passe le lecteur en mode simulation (sans charger les vraies vidéos) pour tester le graphe.";
    texts["SPEED_X2"] = "Joue la vidéo deux fois plus vite (Accélération matérielle si supportée).";
    texts["MUTE"] = "Coupe ou active le son de la vidéo 360 en cours de lecture.";
    texts["106CROP"] = "Agrandit légèrement l'image fixe de pause (106%) pour éviter les bordures noires.";
    texts["USE_DISK_IMGS"] = "Utilise les images .jpg/.png du dossier au lieu de geler la dernière frame vidéo.";
    texts["PAUSE"] = "Définit le temps d'attente (en frames) sur la dernière image avant de changer de vidéo.";
    texts["FADE"] = "Définit la durée (en frames) du fondu enchaîné entre la vidéo et l'image de pause.";
    texts["HOLD_FRAME"] = "Gèle indéfiniment la lecture sur l'image actuelle (Ignore la durée de pause).";
    texts["VIDEO_INFO"] = "Informations sur la vidéo en cours. Déplacez ce bloc en mode EDIT.";
    texts["CLEAR_CREATURES"] = "Supprime toutes les creatures de la Scene 2D";
    texts["UNDO_SCENE2D_CREATURE"] = "Retire la derniere creature ajoutee dans la Scene 2D";
    texts["RESET_EATMAP"] = "Reinitialise la EatMap (restaure les pixels effaces de ce calque).";
    texts["RESET_COLLIDERS"] = "Reinitialise les colliders (restaure les pixels originaux de ce calque).";
    
    // Actions globales
    texts["PAUSE [ESC]"] = "Met en pause ou reprend l'écoulement du temps global (Touche ESC).";
    texts["SAVE MASTER"] = "Sauvegarde une frame du Canvas Master (Vue Principale) en pleine résolution dans le dossier export/.";
    texts["UNDO CREA [Z]"] = "Touche [Z] : Retire la dernière créature ajoutée globalement sur le Canvas Master.";
    texts["CLEAR CREA"] = "Touche [Shift+Z] : Supprime TOUTES les créatures ajoutées globalement sur le Canvas Master.";
    texts["REC PREVIEW"] = "Lance ou arrête l'enregistrement vidéo des images de la fenêtre Preview.";
    texts["REC MASTER"] = "Lance ou arrête l'enregistrement vidéo des images du Canvas Master (Dossier export/canvas_...).";
    texts["BLUR VIEWS"] = "Active ou désactive l'effet de flou global (Shader) sur les fenêtres des vues.";
    texts["EXP 2D FULL"] = "Exporte une image de la Scène 2D complète (Tous les calques assemblés).";
    texts["EXP 2D COL"] = "Exporte une image contenant uniquement les colliders (Sur fond transparent).";
    texts["GEN COL IMG"] = "Génère un collider (contours blancs) par analyse d'image basique de l'Overlay 2D.";
    texts["GEN COL IA"] = "Génère une Depth Map (MiDaS IA) à partir de l'image de l'Overlay 2D.";
    texts["GEN COL SAM"] = "Génère un collider en segmentant l'Overlay 2D avec l'IA EfficientSAM.";
    texts["GEN COL DEX"] = "Génère un collider via détection intelligente des contours (Dexined IA).";
    texts["GEN DEPTH"] = "Génère une Depth Map de l'Overlay 2D avec l'IA DepthAnything V2.";
    texts["EXP EATMAP"] = "Exporte uniquement le calque EatMap (Zone d'effacement) sur fond transparent.";
    texts["GAB 3-0-3"] = "Touche [G] : Règle les opacités : Master à 10%, Room désactivée, Scène 2D transparente.";
    texts["BTN WORMS"] = "Affiche des vers fluorescents décoratifs dans la fenêtre des boutons OSC.";
    texts["EXP 7 MURS"] = "Découpe l'image Overlay 2D en 7 fichiers séparés correspondant aux murs (export/murs2D).";
    texts["SAM CONTROL"] = "Active le contrôle interactif (clic pour points) de l'IA de segmentation SAM dans la Scène 2D.";
    texts["SAM SAVE"] = "Sauvegarde le masque SAM interactif de Scene2D dans export/.";
    texts["SAM RESET"] = "Reinitialise la selection de points SAM de Scene2D.";

    texts["API_KEY_BOX"] = "Cliquez pour éditer. Collez (Cmd/Ctrl+V) votre clef API Gemini/Google ici. Elle n'est pas sauvegardée pour votre sécurité.";
    texts["THEME_BOX"] = "Texte définissant le thème pour la transformation de la Room 3D en image 360.";
    texts["PROMPT_VID1_BOX"] = "Prompt pour générer une vidéo à partir de la dernière image 360 générée.";
    texts["PROMPT_VID2_BOX"] = "Prompt pour générer une vidéo d'interpolation entre les deux dernières images 360.";
    texts["GEN_ROOM_360"] = "Touche [Shift+L] : Génère une image 360 en utilisant une capture de la Room comme référence.";
    texts["GEN_VID_LAST"] = "Touche [Shift+K] : Anime la dernière image 360 générée pour en faire une vidéo.";
    texts["GEN_VID_2_LAST"] = "Touche [Shift+J] : Génère une vidéo transitionnant entre les deux dernières images 360.";
    texts["MODEL_ACCORDION"] = "Sélectionne le modèle d'IA utilisé pour la génération d'images 360.";
    texts["IMG_SIZE_BTN"] = "Définit la résolution de l'image 360 générée (Défaut, 2K, 4K).";
    texts["VID_RES_BTN"] = "Définit la résolution de la vidéo générée (Défaut, 4K).";
    texts["GEN_TEXT_ROOM"] = "Génère une image 360 à partir de texte uniquement (Aucune image de référence).";

    // Interface principale PlaylistVisualizer
    texts["EDIT"] = "Mode Édition : Cliquez pour déplacer ou redimensionner les boutons de l'interface.";
    texts["SAVE"] = "Sauvegarde la disposition actuelle de l'interface dans un fichier JSON.";
    texts["LOAD"] = "Recharge la dernière disposition sauvegardée de l'interface.";
    texts["+ NOTE"] = "Crée une nouvelle note de texte modifiable sur le canevas.";
    texts["+ FRAME"] = "Crée un nouveau cadre visuel pour organiser vos boutons.";
    texts["SEARCH"] = "Recherche un bouton par son nom (Touche Shift+F).";
    texts["HIST"] = "Ouvre l'historique des actions pour annuler des déplacements de caméra.";
    texts["VUES"] = "Gère les signets de points de vue (Pan/Zoom) de l'interface.";
    texts["POS_WIN"] = "Sauvegarde et restaure la position de toutes les fenêtres de l'application.";
    texts["FOCUS_ANNEXES"] = "Ramène au premier plan les fenêtres Annexes (si elles sont ouvertes).";
    texts["TOGGLE_ANNEXES"] = "Affiche ou masque les fenêtres Annexes (Désactive leur rendu si masquées).";
    
    // Interactifs
    texts["INT_AddGroPuyo"] = "Touche [A] ou Clic : Ajoute un GroPuyo a la position du curseur.";
    texts["INT_AddPuyo"] = "Touche [A] ou Clic : Ajoute un Puyo a la position du curseur.";
    texts["INT_AddBubble"] = "Touche [A] ou Clic : Ajoute une Bulle a la position du curseur.";
    texts["INT_TargetPoulpe"] = "Touche [A] ou Clic / Maintenu (Shift pour bloquer la vue) : Attire le Poulpe a la position du curseur.";
    texts["INT_Sardine"] = "Touche [A] ou Clic / Maintenu (Shift pour bloquer la vue) : Ajoute une Sardine a la position du curseur.";
    texts["INT_Shark"] = "Touche [A] ou Clic / Maintenu (Shift pour bloquer la vue) : Ajoute un Requin a la position du curseur.";
    texts["Crayon"] = "Active le pinceau pour dessiner des colliders ou de la EatMap.";
    texts["Alive"] = "Affiche une simulation de mecanismes vivants s'inspirant d'alive.as (Caterpillar)";
    texts["INT_AddAlive"] = "Touche [A] ou Clic : Ajoute une creature Alive a la position du curseur.";
    
    texts["MAIN_BRUSH_MainCrea"] = "Touche [A] sur le Main : Ajoute une Creature standard.";
    texts["MAIN_BRUSH_MainRand"] = "Touche [A] sur le Main : Ajoute une Creature aleatoire.";
    texts["Avoider"] = "Affiche des entites autonomes qui evitent les objets (A faire) et interagissent avec les fluides";
    texts["Avoider2"] = "Affiche des entites autonomes cycliques inspirées de oAnj.as (évitement a faire plus tard)";
    texts["Avoider4"] = "Affiche un mini jeu d'evitement sur 8 directions (inspire de 2Sdk.as)";
    texts["AutoPong"] = "Affiche un mini jeu de pong automatique (inspire de 9iU9.as)";
}

std::string PlaylistTooltipManager::getTooltipText(const std::string& key) {
    if(texts.find(key) != texts.end()) {
        return texts[key];
    }
    return "Active ou desactive l'affichage de : " + key; // Textes generiques (layer 2D & creatures)
}

void PlaylistTooltipManager::drawTooltip(const std::string& text, float x, float y) {
    if(text.empty()) return;
    
    int maxCharsPerLine = 40;
    
    std::string formattedText = "";
    std::vector<std::string> words = ofSplitString(text, " ", true, true);
    std::string currentLine = "";
    
    int maxLineLength = 0;
    int lineCount = 0;

    // Découpage du texte pour un retour à la ligne automatique (Word Wrap)
    for(auto& w : words) {
        if(currentLine.length() + w.length() + 1 > maxCharsPerLine) {
            if(!currentLine.empty()) {
                if (currentLine.back() == ' ') currentLine.pop_back(); // Retire l'espace final
                formattedText += currentLine + "\n";
                if (currentLine.length() > maxLineLength) maxLineLength = currentLine.length();
                lineCount++;
                currentLine = w + " ";
            } else {
                formattedText += w.substr(0, maxCharsPerLine) + "\n";
                lineCount++;
                currentLine = w.substr(maxCharsPerLine) + " ";
            }
        } else {
            currentLine += w + " ";
        }
    }
    if(!currentLine.empty()) {
        if (currentLine.back() == ' ') currentLine.pop_back();
        formattedText += currentLine;
        if (currentLine.length() > maxLineLength) maxLineLength = currentLine.length();
        lineCount++;
    }
    
    // Dimensions estimées de l'infobulle (ofDrawBitmapStringHighlight utilise une police de ~8x15 px)
    float charWidth = 8.0f;
    float lineHeight = 15.0f; 
    float boxWidth = maxLineLength * charWidth + 16.0f;
    float boxHeight = lineCount * lineHeight + 16.0f;
    
    float drawX = x + 15;
    float drawY = y + 15;
    
    // Vérification des bords de la fenêtre et repositionnement si nécessaire
    if (drawX + boxWidth > ofGetWidth()) drawX = x - boxWidth - 5;
    if (drawY + boxHeight > ofGetHeight()) drawY = y - boxHeight - 5;
    if (drawX < 5) drawX = 5;
    if (drawY < 15) drawY = 15;
    
    ofPushStyle();
    ofDrawBitmapStringHighlight(formattedText, drawX, drawY, ofColor(20, 20, 20, 220), ofColor(255));
    ofPopStyle();
}