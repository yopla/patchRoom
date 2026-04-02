#include "PlaylistSearchBar.h"

void PlaylistSearchBar::setup() {
    searchBoxRect.set(ofGetWidth()/2 - 200, 50, 400, 40);
}

void PlaylistSearchBar::toggle(const vector<SearchableButton>& allButtons, ofVec2f currentPan, float currentZoom) {
    bIsVisible = !bIsVisible;
    if (bIsVisible) {
        searchText = "";
        allAvailableButtons = allButtons;
        originalPan = currentPan;
        originalZoom = currentZoom;
        bIsHoveringSuggestion = false;
        hoveredIndex = -1;
        updateSuggestions();
    }
}

void PlaylistSearchBar::close() {
    bIsVisible = false;
}

void PlaylistSearchBar::updateSuggestions() {
    currentSuggestions.clear();
    suggestionRects.clear();
    if (searchText.empty()) return;
    
    string lowerSearch = ofToLower(searchText);
    for (const auto& btn : allAvailableButtons) {
        if (ofToLower(btn.name).find(lowerSearch) != string::npos) {
            currentSuggestions.push_back(btn);
            if (currentSuggestions.size() >= 10) break; // Limite à 10 propositions
        }
    }
    
    for (size_t i = 0; i < currentSuggestions.size(); ++i) {
        suggestionRects.push_back(ofRectangle(searchBoxRect.x, searchBoxRect.getBottom() + i * 30, searchBoxRect.width, 30));
    }
}

void PlaylistSearchBar::draw() {
    if (!bIsVisible) return;
    
    ofPushStyle();
    
    // Garder centré même si on redimensionne la fenêtre
    searchBoxRect.x = ofGetWidth()/2 - 200;
    for (size_t i = 0; i < suggestionRects.size(); ++i) {
        suggestionRects[i].x = searchBoxRect.x;
    }

    // Fond sombre pour faire ressortir la recherche
    ofSetColor(0, 0, 0, 150);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    // Barre de recherche
    ofSetColor(40, 40, 40, 240);
    ofFill();
    ofDrawRectangle(searchBoxRect);
    ofSetColor(200, 200, 200);
    ofNoFill();
    ofDrawRectangle(searchBoxRect);
    
    ofSetColor(255);
    ofDrawBitmapString("Recherche (Nom du bouton) : " + searchText + "_", searchBoxRect.x + 10, searchBoxRect.y + 25);
    
    // Liste des propositions
    for (size_t i = 0; i < currentSuggestions.size(); ++i) {
        if ((int)i == hoveredIndex) {
            ofSetColor(80, 120, 200, 240); // Surligné bleu
        } else {
            ofSetColor(30, 30, 30, 240);
        }
        ofFill();
        ofDrawRectangle(suggestionRects[i]);
        
        ofSetColor(200);
        ofNoFill();
        ofDrawRectangle(suggestionRects[i]);
        
        ofSetColor(255);
        ofDrawBitmapString(currentSuggestions[i].name, suggestionRects[i].x + 15, suggestionRects[i].y + 20);
    }
    
    ofPopStyle();
}

void PlaylistSearchBar::applyHoverView(ofVec2f& pan, float& zoom) {
    if (hoveredIndex != -1 && hoveredIndex < currentSuggestions.size()) {
        ofRectangle* rect = currentSuggestions[hoveredIndex].rect;
        if (rect) {
            zoom = 1.5f; // Zoom sur l'élément
            ofVec2f rectCenter(rect->x + rect->width/2.0f, rect->y + rect->height/2.0f);
            pan.x = ofGetWidth()/2.0f - rectCenter.x * zoom;
            pan.y = ofGetHeight()/2.0f - rectCenter.y * zoom;
            bIsHoveringSuggestion = true;
        }
    } else {
        if (bIsHoveringSuggestion) {
            pan = originalPan;
            zoom = originalZoom;
            bIsHoveringSuggestion = false;
        }
    }
}

bool PlaylistSearchBar::mouseMoved(int x, int y, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    int newHoveredIndex = -1;
    for (size_t i = 0; i < suggestionRects.size(); ++i) {
        if (suggestionRects[i].inside(x, y)) {
            newHoveredIndex = i;
            break;
        }
    }
    
    if (newHoveredIndex != hoveredIndex) {
        hoveredIndex = newHoveredIndex;
        applyHoverView(pan, zoom);
    }
    return true;
}

bool PlaylistSearchBar::mousePressed(int x, int y, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    if (hoveredIndex != -1) { close(); return true; } // Clic sur suggestion : On valide
    if (searchBoxRect.inside(x, y)) return true;      // Clic dans la barre de recherche
    
    // Clic en dehors, on annule tout
    if (bIsHoveringSuggestion) { pan = originalPan; zoom = originalZoom; }
    close();
    return true;
}

bool PlaylistSearchBar::keyPressed(int key, ofVec2f& pan, float& zoom) {
    if (!bIsVisible) return false;
    
    auto resetHover = [&]() {
        if (bIsHoveringSuggestion) { pan = originalPan; zoom = originalZoom; bIsHoveringSuggestion = false; hoveredIndex = -1; }
    };

    if (key == OF_KEY_UP) {
        if (!currentSuggestions.empty()) {
            if (hoveredIndex > 0) {
                hoveredIndex--;
            } else {
                hoveredIndex = currentSuggestions.size() - 1;
            }
            applyHoverView(pan, zoom);
        }
    } else if (key == OF_KEY_DOWN) {
        if (!currentSuggestions.empty()) {
            if (hoveredIndex < (int)currentSuggestions.size() - 1) {
                hoveredIndex++;
            } else {
                hoveredIndex = 0;
            }
            applyHoverView(pan, zoom);
        }
    } else if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
        if (!searchText.empty()) { searchText.pop_back(); updateSuggestions(); resetHover(); }
    } else if (key == OF_KEY_RETURN) {
        close();
    } else if (key == OF_KEY_ESC) {
        pan = originalPan; zoom = originalZoom; close();
    } else if (key >= 32 && key <= 126) {
        searchText += (char)key; updateSuggestions(); resetHover();
    }
    
    return true;
}