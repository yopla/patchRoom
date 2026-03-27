#include "AnnexeTooltipManager.h"

AnnexeTooltipManager::AnnexeTooltipManager() {}

void AnnexeTooltipManager::setup() {
    texts["EDIT"] = "Active/Desactive le mode edition pour deplacer ou redimensionner les elements.";
    texts["SAVE"] = "Sauvegarde la disposition actuelle des boutons et cadres.";
    texts["LOAD"] = "Recharge la derniere sauvegarde de l'interface.";
    texts["+ NOTE"] = "Ajoute une note textuelle modifiable sur l'espace de travail.";
    texts["+ FRAME"] = "Ajoute un cadre visuel pour regrouper ou encadrer des elements.";
    texts["SEARCH"] = "Ouvre la barre de recherche pour trouver un element (Raccourci: Shift+F).";
    texts["FOCUS ANNEXE"] = "Ramene les fenetres Annexe et Annexe Player au premier plan.";
    texts["SOLO ANNEXE"] = "Isole la fenetre Annexe en cachant temporairement toutes les autres vues principales.";
    texts["SAVE FRAME"] = "Sauvegarde une image en haute resolution de la frame actuelle dans le dossier export/.";
    texts["GEN AI"] = "Genere une Depth Map par IA (MiDaS) a partir de l'image de l'Annexe et l'exporte.";
    texts["GEN SAM"] = "Genere un masque de segmentation par IA (EfficientSAM) a partir de l'image de l'Annexe et l'exporte.";
    texts["DEPTH ANY"] = "Genere une Depth Map alternative avec le modele DepthAnything et l'exporte.";
    texts["RIPPLE FX"] = "Active un effet d'ondulation sur l'image. Cliquez sur l'image pour creer des vagues.";
    texts["RECORD VIDEO"] = "Enregistre une sequence d'images en haute resolution dans un dossier dedie dans export/.";
    texts["SAM CONTROL"] = "Active l'interface de controle interactif pour la segmentation avec SAM.";
}

std::string AnnexeTooltipManager::getTooltipText(const std::string& key) {
    if(texts.find(key) != texts.end()) {
        return texts[key];
    }
    return ""; 
}

void AnnexeTooltipManager::drawTooltip(const std::string& text, float x, float y) {
    if(text.empty()) return;
    
    int maxCharsPerLine = 40;
    std::string formattedText = "";
    std::vector<std::string> words = ofSplitString(text, " ", true, true);
    std::string currentLine = "";
    int maxLineLength = 0;
    int lineCount = 0;

    for(auto& w : words) {
        if(currentLine.length() + w.length() + 1 > maxCharsPerLine) {
            if(!currentLine.empty()) {
                if (currentLine.back() == ' ') currentLine.pop_back();
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
    
    float charWidth = 8.0f;
    float lineHeight = 15.0f; 
    float boxWidth = maxLineLength * charWidth + 16.0f;
    float boxHeight = lineCount * lineHeight + 16.0f;
    
    float drawX = x + 15;
    float drawY = y + 15;
    
    if (drawX + boxWidth > ofGetWidth()) drawX = x - boxWidth - 5;
    if (drawY + boxHeight > ofGetHeight()) drawY = y - boxHeight - 5;
    if (drawX < 5) drawX = 5;
    if (drawY < 15) drawY = 15;
    
    ofPushStyle();
    ofDrawBitmapStringHighlight(formattedText, drawX, drawY, ofColor(20, 20, 20, 220), ofColor(255));
    ofPopStyle();
}