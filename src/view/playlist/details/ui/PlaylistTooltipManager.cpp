#include "PlaylistTooltipManager.h"

PlaylistTooltipManager::PlaylistTooltipManager() {}

void PlaylistTooltipManager::setup() {
    // Vues
    texts["V1"] = "Affiche ou cache la vue 1";
    texts["V2"] = "Affiche ou cache la vue 2";
    texts["V3"] = "Affiche ou cache la vue 3";
    texts["V4"] = "Affiche ou cache la vue 4";
    texts["->V1"] = "Deplace la fenetre V1 vers l'ecran secondaire a droite";
    texts["->V2"] = "Deplace la fenetre V2 vers l'ecran secondaire a droite";
    texts["->V3"] = "Deplace la fenetre V3 vers l'ecran secondaire a droite";
    texts["->V4"] = "Deplace la fenetre V4 vers l'ecran secondaire a droite";
    texts["V1 WIN"] = "Affiche ou masque completement la fenetre de la V1 au niveau du systeme d'exploitation (GLFW)";
    texts["V2 WIN"] = "Affiche ou masque completement la fenetre de la V2 au niveau du systeme d'exploitation (GLFW)";
    texts["V3 WIN"] = "Affiche ou masque completement la fenetre de la V3 au niveau du systeme d'exploitation (GLFW)";
    texts["V4 WIN"] = "Affiche ou masque completement la fenetre de la V4 au niveau du systeme d'exploitation (GLFW)";
    
    texts["REC V1"] = "Enregistre la fenetre V1 (Dossier export/view_...)";
    texts["REC V2"] = "Enregistre la fenetre V2 (Dossier export/view_...)";
    texts["REC V3"] = "Enregistre la fenetre V3 (Dossier export/view_...)";
    texts["REC V4"] = "Enregistre la fenetre V4 (Dossier export/view_...)";
    texts["FMT_BTN"] = "Bascule le format d'enregistrement (PNG preserve la transparence, JPG est plus leger)";
    texts["QUAL_BTN"] = "Bascule la qualite d'enregistrement de l'image (BEST, HIGH, MED, LOW, WORST)";
    texts["ARRANGE_WINS"] = "Repartit dynamiquement les fenetres (MAIN, WXCVB) sur l'ecran.";
    
    // WXCVB
    texts["MAIN"] = "Affiche ou cache le rendu de la fenetre Master (Principale)";
    texts["W:ROOM"] = "Affiche ou cache la fenetre de la Room 3D";
    texts["X:ZENI"] = "Affiche ou cache la fenetre de la vue Zenith";
    texts["C:SCEN"] = "Affiche ou cache la fenetre de la Scene 2D";
    texts["V:PREV"] = "Met en pause/reprend l'affichage de la fenetre Preview";
    texts["B:BTNS"] = "Affiche ou cache la fenetre des boutons OSC";
    
    texts["->MAIN"] = "Amene la fenetre Master au premier plan";
    texts["->W"] = "Amene la fenetre Room 3D au premier plan";
    texts["->X"] = "Amene la fenetre de la vue Zenith au premier plan";
    texts["->C"] = "Amene la fenetre de la Scene 2D au premier plan";
    texts["->V"] = "Amene la fenetre Preview au premier plan";
    texts["->B"] = "Amene la fenetre des boutons OSC au premier plan";
    
    // GAB
    texts["GAB 0"] = "Change le niveau de transparence global du gabarit";
    texts["GAB 1"] = "Active/Desactive le fond gabarit dans la Room 3D";
    texts["GAB 2"] = "Change le mode d'affichage du fond dans la Scene 2D";
    texts["GAB 3"] = "Change l'affichage de l'image superposee dans la Scene 2D (Touche H)";
    
    // Room
    texts["Sol 3D"] = "Active le sol ondulant en 3D dans la Room";
    texts["Oscillate"] = "Fait tanguer la Room comme sur un bateau";
    texts["Kraken"] = "Fait apparaitre les tentacules du Kraken a l'interieur";
    texts["Ext Kraken"] = "Fait apparaitre les tentacules du Kraken a l'exterieur";
    texts["Cloud Ring"] = "Affiche une sphere de nuages volumetriques";
    texts["Liq Sphere"] = "Affiche une sphere d'eau liquide flottante";
    texts["Jelly Sphere"] = "Affiche une sphere de Jellies interactifs (Touche 0)";
    texts["Color Cop"] = "Affiche une sphere texturee que des petits vaisseaux peuvent detruire par morceaux en cliquant dessus.";
    texts["Scene360Vid"] = "Affiche la video 360 en cours sur la sphere environnementale";
    texts["Beam Proj"] = "Affiche le faisceau volumetrique du videoprojecteur virtuel";
    texts["AtmoPreview"] = "Active ou desactive la vue de sphere d'environnement 360 en preview (reste rendu)";
    texts["Use Texture"] = "Bascule entre une texture couleur et un damier/texture chargee";
    texts["Show Roof"] = "Affiche ou cache le plafond de la piece";
    texts["Respire"] = "Fait osciller legerement la camera verticalement (respiration)";
    texts["Ripples"] = "Affiche un effet de vagues d'eau au sol";
    texts["Worms"] = "Affiche des vers lumineux rampants sur les murs";
    texts["Wing Worms"] = "Affiche des vers volants dans la piece";
    texts["Fluid Ring"] = "Affiche l'anneau de portail fluide";
    texts["Light Fly"] = "Affiche un essaim de petites lumieres volantes";
    texts["Alpha Cur"] = "Rend le curseur carre de la Room plus transparent";
    texts["Alpha Wall"] = "Affiche ou cache les murs de la piece (transparence)";
    texts["Gen 360"] = "Genere une image equirectangulaire 360 de la piece actuelle";
    texts["Gen 360 Full"] = "Genere une image 360 en utilisant les textures actuelles (FBOs Scene 2D) au lieu des images par defaut.";
    texts["360fullW"] = "Genere une image 360 de TOUTE la scene 3D par rendu Cubemap (Inclut le Kraken, Plan Colle, etc.)";
    texts["Show Beams"] = "Affiche ou cache les 3 faisceaux de projection (Touche T)";
    texts["Plan Colle"] = "Affiche ou cache le plan de collage de projection (Touche N)";
    texts["Cur Reflet"] = "Bascule le debordement du curseur carre sur les murs adjacents";
    texts["Prev Interact"] = "Touche [B] : Active/Desactive la visualisation des interactions (RoomPreview)";
    texts["Prev Cursor"] = "Touche [S] : Affiche/Cache le curseur projete (RoomPreview)";
    texts["Clear Flys [X]"] = "Supprime toutes les lumieres/lucioles du LightFlyRing (Touche X)";
    texts["Cam Lock [L]"] = "Verrouille la camera au centre de la piece pour un point de vue 360 (Touche L)";
    
    // Player
    texts["LOOP"] = "Lit la video actuelle en boucle sans passer a la suivante";
    texts["PLAY"] = "Demarre ou arrete la lecture de la playlist";
    texts["SIMU"] = "Simule de fausses videos pour tester la logique du graphe nodal";
    texts["SPEED_X2"] = "Double la vitesse de lecture de la video en cours (x2).";
    texts["MUTE"] = "Coupe ou reactive le son de la video en cours.";
    texts["106CROP"] = "Grossit l'image de pause fixe a 106% pour cropper les bords et correspondre au cadrage video.";
    texts["USE_DISK_IMGS"] = "Utilise ou non les images fixes du disque (.png, .jpg) pendant les pauses et les fondus.";
    texts["PAUSE"] = "Definit le temps (en frames) d'arret sur la derniere image";
    texts["FADE"] = "Definit la duree (en frames) du fondu entre la derniere image de la video et l'image de pause";
    texts["HOLD_FRAME"] = "Maintient indefiniment la derniere image de la video en pause (Ignore le compteur de frames).";
    texts["VIDEO_INFO"] = "Affiche les informations sur la video en cours. Deplacable en mode EDIT.";
    texts["CLEAR_CREATURES"] = "Supprime toutes les creatures de la Scene 2D";
    texts["UNDO_SCENE2D_CREATURE"] = "Retire la derniere creature ajoutee dans la Scene 2D";
    texts["RESET_EATMAP"] = "Reinitialise la EatMap (restaure les pixels effaces de ce calque).";
    texts["RESET_COLLIDERS"] = "Reinitialise les colliders (restaure les pixels originaux de ce calque).";
    
    // Actions globales
    texts["PAUSE [ESC]"] = "Met en pause ou reprend l'ecoulement du temps global (Touche ESC)";
    texts["SAVE GAB"] = "Sauvegarde une frame du Canvas Master en pleine resolution dans le dossier export/";
    texts["UNDO CREA [D]"] = "Touche [D] : Retire la derniere creature ajoutee globalement sur le Canvas Master";
    texts["CLEAR ALL CREA"] = "Touche [Shift+D] : Supprime toutes les creatures ajoutees globalement sur le Canvas Master";
    texts["REC PREVIEW"] = "Lance ou arrete l'enregistrement des images de la fenetre Preview";
    texts["REC CANVAS"] = "Lance ou arrete l'enregistrement des images du Canvas Master (Vue 3)";
    texts["BLUR SHADER"] = "Active ou desactive le flou du shader";
    texts["EXP SCENE2D"] = "Exporte la frame complete (tout l'environnement 2D assemble)";
    texts["EXP COLLIDER"] = "Exporte uniquement le calque des colliders (sur fond transparent)";
    texts["GEN COLLIDER"] = "Analyse l'overlay de la Scene 2D pour generer et exporter une image de collider (contours blancs)";
    texts["GEN COL IA"] = "Genere une Depth Map par IA (MiDaS) a partir de l'image de l'overlay et l'exporte.";
    texts["GEN SAM COL"] = "Tente de segmenter l'overlay via le modele EfficientSAM";
    texts["GEN DEX COL"] = "Utilise l'IA (Dexined) pour extraire intelligemment les contours (Edge Detection) de l'overlay et generer un collider";
    texts["GEN DEPTH ANY"] = "Genere une Depth Map alternative avec le modele DepthAnything et l'exporte.";
    texts["EXP EATMAP"] = "Exporte uniquement le calque de la EatMap (sur fond transparent)";
    texts["EXP 7 MURS"] = "Decoupe l'image superposee (overlay) en 7 fichiers dans le dossier export/murs2D";
    texts["GAB 3-OFF-3 [G]"] = "Touche [G] : Configure les gabarits : Master a 3 (10%), Room en OFF, Scene 2D a 3 (Rien).";
    texts["BTN WORMS"] = "Active ou desactive les vers fluo dans la fenetre des boutons OSC.";
    texts["GEN_ROOM_360"] = "Touche [Shift+L] : Genere une image 360 a partir d'un export de la Room et du theme actuel.";
    texts["GEN_VID_LAST"] = "Touche [Shift+K] : Genere une video a partir de la derniere image 360 generee (gen360_last.jpg).";
    texts["GEN_VID_2_LAST"] = "Touche [Shift+J] : Genere une video a partir des deux dernieres images 360 generees (gen360.jpg et gen360_last.jpg).";
    texts["API_KEY_BOX"] = "Cliquez pour editer. Collez (Cmd/Ctrl+V) votre clef API Gemini ici. Elle ne sera pas sauvegardee.";
    texts["THEME_BOX"] = "Cliquez pour editer le theme utilise pour la generation d'image a partir de la Room.";
    texts["PROMPT_VID1_BOX"] = "Cliquez pour editer le prompt pour la generation video depuis la derniere image.";
    texts["PROMPT_VID2_BOX"] = "Cliquez pour editer le prompt pour la generation video depuis 2 images (interpolation).";
    texts["MODEL_ACCORDION"] = "Choisit le modele de generation d'image 360";
    texts["IMG_SIZE_BTN"] = "Definit la taille de l'image generee (Par defaut, 2K, 4K)";
    texts["VID_RES_BTN"] = "Definit la resolution video generee (Par defaut, 4k)";
    texts["GEN_TEXT_ROOM"] = "Genere une image 360 a partir du texte (sans reference)";
    texts["SAM CONTROL"] = "Active l'interface de controle interactif pour la segmentation avec SAM dans Scene2D.";
    texts["SAM SAVE"] = "Sauvegarde le masque SAM interactif de Scene2D dans export/.";
    texts["SAM RESET"] = "Reinitialise la selection de points SAM de Scene2D.";
    
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