#include "PlaylistGeminiUI.h"
#include "ofApp.h"

void PlaylistGeminiUI::setup() {
    modelOptions = {
        "gemini-3.1-flash-image-preview",
        "gemini-3-pro-image-preview",
        "nano-banana-pro-preview"
    };
    imageSizeOptions = {"DEFAULT", "2K", "4K"};
    videoResOptions = {"DEFAULT", "4k"};
    
    float startX = 1500;
    float startY = 700;
    
    apiKeyBox.set(startX, startY, 300, 30);
    listModelsBtn.set(startX + 310, startY, 90, 30);
    themeBox.set(startX, startY + 40, 400, 80);
    genRoomBtn.set(startX, startY + 130, 400, 30);
    promptVid1Box.set(startX, startY + 170, 400, 80);
    genVidLastBtn.set(startX, startY + 260, 400, 30);
    promptVid2Box.set(startX, startY + 300, 400, 80);
    genVid2LastBtn.set(startX, startY + 390, 400, 30);
    
    modelAccordionBtn.set(startX, startY + 430, 400, 30);
    for(int i=0; i<modelOptions.size(); i++) {
        modelOptionRects.push_back(ofRectangle(startX, startY + 460 + i * 30, 400, 30));
    }
    imageSizeBtn.set(startX, startY + 470, 195, 30);
    videoResBtn.set(startX + 205, startY + 470, 195, 30);
    genTextToRoomBtn.set(startX, startY + 510, 400, 30);
}

void PlaylistGeminiUI::update(ofApp* mainAppPtr) {
    if(modelOptionRects.size() == modelOptions.size()) {
        for(size_t i=0; i<modelOptions.size(); i++) {
            modelOptionRects[i].x = modelAccordionBtn.x;
            modelOptionRects[i].y = modelAccordionBtn.y + modelAccordionBtn.height + i * modelAccordionBtn.height;
            modelOptionRects[i].width = modelAccordionBtn.width;
            modelOptionRects[i].height = modelAccordionBtn.height;
        }
    }
    
    if (mainAppPtr) {
        mainAppPtr->geminiGen.setModelName(modelOptions[currentModelIndex]);
        string iSize = imageSizeOptions[currentImageSizeIndex];
        mainAppPtr->geminiGen.setImageSize(iSize == "DEFAULT" ? "" : iSize);
        string vRes = videoResOptions[currentVideoResIndex];
        mainAppPtr->geminiGen.setVideoResolution(vRes == "DEFAULT" ? "" : vRes);
    }
}

void PlaylistGeminiUI::draw() {
    ofPushStyle();

    // API Key
    ofFill();
    ofSetColor(bApiKeyFocused ? 60 : 30);
    ofDrawRectangle(apiKeyBox);
    ofNoFill();
    ofSetColor(bApiKeyFocused ? ofColor(50, 200, 50) : ofColor(150));
    ofDrawRectangle(apiKeyBox);
    ofSetColor(255);
    
    string displayApi = apiKeyText;
    if(displayApi.length() > 40) displayApi = "..." + displayApi.substr(displayApi.length() - 37);
    if(displayApi.empty()) displayApi = "Coller Clef API ici...";
    ofPushMatrix(); ofTranslate(apiKeyBox.x, apiKeyBox.y); ofScale(apiKeyBox.height/30.0f, apiKeyBox.height/30.0f);
    ofDrawBitmapString("API: " + displayApi, 5, 20);
    ofPopMatrix();

    ofFill(); ofSetColor(100, 150, 200); ofDrawRectangle(listModelsBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(listModelsBtn);
    ofPushMatrix(); ofTranslate(listModelsBtn.x, listModelsBtn.y); ofScale(listModelsBtn.height/30.0f, listModelsBtn.height/30.0f);
    ofDrawBitmapString("SCAN MODELS", 5, 20);
    ofPopMatrix();

    // Rendu Scrollable Text Box pour Theme et Prompts
    auto drawScrollBox = [&](const ofRectangle& box, string title, string text, bool isFocused, int& scrollOffset) {
        ofFill();
        ofSetColor(isFocused ? 60 : 30);
        ofDrawRectangle(box);
        ofNoFill();
        ofSetColor(isFocused ? ofColor(50, 200, 50) : ofColor(150));
        ofDrawRectangle(box);
        ofSetColor(255);
        
        vector<string> lines;
        string firstLinePrefix = title + ": ";
        int maxChars = std::max(10, (int)(box.width - 25) / 8); 
        
        if(text.empty()) {
            lines.push_back(firstLinePrefix + "...");
        } else {
            string cleanText = text;
            ofStringReplace(cleanText, "\n", " \n "); 
            vector<string> words = ofSplitString(cleanText, " ", true, true);
            
            string currentLine = firstLinePrefix;
            for(auto& w : words) {
                if (w == "\n") {
                    lines.push_back(currentLine);
                    currentLine = "";
                    continue;
                }
                if (currentLine.length() + w.length() + 1 > maxChars) {
                    if(currentLine.empty()) {
                        lines.push_back(w.substr(0, maxChars));
                        currentLine = w.substr(maxChars) + " ";
                    } else {
                        lines.push_back(currentLine);
                        currentLine = w + " ";
                    }
                } else {
                    currentLine += w + " ";
                }
            }
            if(!currentLine.empty()) lines.push_back(currentLine);
        }

        int lineHeight = 16;
        int maxDisplayLines = std::max(1, (int)((box.height - 10) / lineHeight));
        
        if (scrollOffset > (int)lines.size() - maxDisplayLines) scrollOffset = std::max(0, (int)lines.size() - maxDisplayLines);
        if (scrollOffset < 0) scrollOffset = 0;
        
        int endIdx = std::min((int)lines.size(), scrollOffset + maxDisplayLines);
        for(int i = scrollOffset; i < endIdx; i++) {
            ofDrawBitmapString(lines[i], box.x + 5, box.y + 18 + (i - scrollOffset) * lineHeight);
        }
        
        if ((int)lines.size() > maxDisplayLines) {
            float scrollBarWidth = 6;
            float viewRatio = (float)maxDisplayLines / lines.size();
            float scrollBarHeight = std::max(10.0f, box.height * viewRatio);
            float maxScrollOffset = lines.size() - maxDisplayLines;
            float scrollPos = (box.height - scrollBarHeight) * ((float)scrollOffset / maxScrollOffset);
            
            ofSetColor(150, 150, 150, 180);
            ofFill();
            ofDrawRectangle(box.getRight() - scrollBarWidth - 4, box.y + scrollPos, scrollBarWidth, scrollBarHeight);
        }
    };

    drawScrollBox(themeBox, "Theme", themeText, bThemeFocused, themeScrollOffset);
    drawScrollBox(promptVid1Box, "Prompt V1", promptVid1Text, bPromptVid1Focused, promptVid1ScrollOffset);
    drawScrollBox(promptVid2Box, "Prompt V2", promptVid2Text, bPromptVid2Focused, promptVid2ScrollOffset);

    // Generate Buttons
    auto drawGenBtn = [&](const ofRectangle& rect, string text, ofColor col) {
        ofFill(); ofSetColor(col); ofDrawRectangle(rect);
        ofNoFill(); ofSetColor(255); ofDrawRectangle(rect);
        ofPushMatrix(); ofTranslate(rect.x, rect.y); float s = rect.height/30.0f; ofScale(s, s);
        ofDrawBitmapStringHighlight(text, (rect.width/s/2.0f) - (text.length()*4.0f), 20, col, ofColor(255));
        ofPopMatrix();
    };
    
    drawGenBtn(genRoomBtn, "GENERATE 360 FROM ROOM", ofColor(100, 150, 200));
    drawGenBtn(genVidLastBtn, "GEN VID FROM LAST FRAME", ofColor(100, 180, 150));
    drawGenBtn(genVid2LastBtn, "GEN VID FROM 2 LAST FRAMES", ofColor(120, 180, 120));
    drawGenBtn(genTextToRoomBtn, "GENERATE 360 FROM TEXT", ofColor(150, 100, 200));
    
    ofFill(); ofSetColor(100); ofDrawRectangle(imageSizeBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(imageSizeBtn);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(imageSizeBtn.x, imageSizeBtn.y); ofScale(imageSizeBtn.height/30.0f, imageSizeBtn.height/30.0f);
    ofDrawBitmapString("IMG SIZE: " + imageSizeOptions[currentImageSizeIndex], 5, 20);
    ofPopMatrix();

    ofFill(); ofSetColor(100); ofDrawRectangle(videoResBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(videoResBtn);
    ofSetColor(255); 
    ofPushMatrix(); ofTranslate(videoResBtn.x, videoResBtn.y); ofScale(videoResBtn.height/30.0f, videoResBtn.height/30.0f);
    ofDrawBitmapString("VID RES: " + videoResOptions[currentVideoResIndex], 5, 20);
    ofPopMatrix();

    // Accordion en dernier pour recouvrir les autres boutons s'il est ouvert
    if (bModelAccordionOpen) ofSetColor(150, 150, 200); else ofSetColor(100);
    ofFill(); ofDrawRectangle(modelAccordionBtn);
    ofSetColor(255);
    ofPushMatrix(); ofTranslate(modelAccordionBtn.x, modelAccordionBtn.y); ofScale(modelAccordionBtn.height/30.0f, modelAccordionBtn.height/30.0f);
    ofDrawBitmapString("MODEL: " + modelOptions[currentModelIndex] + (bModelAccordionOpen ? " [-]" : " [+]"), 5, 20);
    ofPopMatrix();

    if (bModelAccordionOpen) {
        for(size_t i=0; i<modelOptions.size(); i++) {
            if (currentModelIndex == i) ofSetColor(200, 200, 50); else ofSetColor(80);
            ofFill(); ofDrawRectangle(modelOptionRects[i]);
            ofNoFill(); ofSetColor(200); ofDrawRectangle(modelOptionRects[i]);
            ofSetColor(255);
            ofPushMatrix(); ofTranslate(modelOptionRects[i].x, modelOptionRects[i].y); ofScale(modelOptionRects[i].height/30.0f, modelOptionRects[i].height/30.0f);
            ofDrawBitmapString(modelOptions[i], 20, 20);
            ofPopMatrix();
        }
    }

    ofPopStyle();
}

bool PlaylistGeminiUI::mousePressed(ofVec2f worldM, ofApp* mainAppPtr) {
    if(apiKeyBox.inside(worldM)) {
        bApiKeyFocused = true; bThemeFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false; return true;
    }
    if(themeBox.inside(worldM)) {
        bThemeFocused = true; bApiKeyFocused = false; bPromptVid1Focused = false; bPromptVid2Focused = false; return true;
    }
    if(promptVid1Box.inside(worldM)) {
        bPromptVid1Focused = true; bApiKeyFocused = false; bThemeFocused = false; bPromptVid2Focused = false; return true;
    }
    if(promptVid2Box.inside(worldM)) {
        bPromptVid2Focused = true; bApiKeyFocused = false; bThemeFocused = false; bPromptVid1Focused = false; return true;
    }
    
    if(listModelsBtn.inside(worldM)) {
        unfocusAll();
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            mainAppPtr->geminiGen.listModels();
            ofLogNotice("PlaylistGeminiUI") << "Scan des modeles lance.";
        }
        return true;
    }
    
    if(genRoomBtn.inside(worldM)) {
        unfocusAll();
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            string prompt = "Transform this room into vector illustration of " + themeText + " (in style of day of the tentacle) , keeping the structure but changing materials and lighting";
            mainAppPtr->geminiGen.generateImage360FromImage(prompt, "export_360_room.png");
            ofLogNotice("PlaylistVisualizerApp") << "Generation 360 lancee via bouton.";
        }
        return true;
    }
    if(genVidLastBtn.inside(worldM)) {
        unfocusAll();
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            mainAppPtr->geminiGen.generateVideoFromImage(promptVid1Text, "gen360_last.jpg");
        }
        return true;
    }
    if(genVid2LastBtn.inside(worldM)) {
        unfocusAll();
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            mainAppPtr->geminiGen.generateVideoFromDeuxImages(promptVid2Text, "gen360.jpg", "gen360_last.jpg");
        }
        return true;
    }
    
    if (bModelAccordionOpen) {
        for(int i=0; i<modelOptionRects.size(); i++) {
            if (modelOptionRects[i].inside(worldM)) {
                currentModelIndex = i;
                bModelAccordionOpen = false;
                return true;
            }
        }
    }
    if (modelAccordionBtn.inside(worldM)) {
        bModelAccordionOpen = !bModelAccordionOpen;
        unfocusAll();
        return true;
    }
    if (imageSizeBtn.inside(worldM)) {
        currentImageSizeIndex = (currentImageSizeIndex + 1) % imageSizeOptions.size();
        unfocusAll();
        return true;
    }
    if (videoResBtn.inside(worldM)) {
        currentVideoResIndex = (currentVideoResIndex + 1) % videoResOptions.size();
        unfocusAll();
        return true;
    }
    if (genTextToRoomBtn.inside(worldM)) {
        unfocusAll();
        if(mainAppPtr) {
            if(!apiKeyText.empty()) mainAppPtr->geminiGen.setApiKey(apiKeyText);
            string prompt = themeText;
            mainAppPtr->geminiGen.generateImage360(prompt);
            ofLogNotice("PlaylistVisualizerApp") << "Generation 360 via texte lancee.";
        }
        return true;
    }
    
    return false;
}

bool PlaylistGeminiUI::mouseScrolled(ofVec2f worldM, float scrollY) {
    if (themeBox.inside(worldM)) {
        themeScrollOffset -= (scrollY > 0) ? 1 : -1;
        return true;
    }
    if (promptVid1Box.inside(worldM)) {
        promptVid1ScrollOffset -= (scrollY > 0) ? 1 : -1;
        return true;
    }
    if (promptVid2Box.inside(worldM)) {
        promptVid2ScrollOffset -= (scrollY > 0) ? 1 : -1;
        return true;
    }
    return false;
}

bool PlaylistGeminiUI::keyPressed(int key) {
    if(bApiKeyFocused || bThemeFocused || bPromptVid1Focused || bPromptVid2Focused) {
        string* targetText = bApiKeyFocused ? &apiKeyText : 
                             bThemeFocused ? &themeText : 
                             bPromptVid1Focused ? &promptVid1Text : &promptVid2Text;

        auto scrollToBottom = [&]() {
            if (bThemeFocused) themeScrollOffset = 9999;
            if (bPromptVid1Focused) promptVid1ScrollOffset = 9999;
            if (bPromptVid2Focused) promptVid2ScrollOffset = 9999;
        };

        if (ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) {
            if (key == 'v' || key == 'V' || key == 22) { 
                *targetText += ofGetWindowPtr()->getClipboardString();
                scrollToBottom();
                return true;
            }
        }

        if(key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
            if(!targetText->empty()) {
                targetText->pop_back();
                scrollToBottom();
            }
        } else if (key == OF_KEY_RETURN) {
            if (bApiKeyFocused) {
                bApiKeyFocused = false;
            } else {
                *targetText += "\n";
                scrollToBottom();
            }
        } else if (key >= 32 && key <= 126) {
            *targetText += (char)key;
            scrollToBottom();
        }
        return true;
    }
    return false;
}

void PlaylistGeminiUI::unfocusAll() {
    bApiKeyFocused = false;
    bThemeFocused = false;
    bPromptVid1Focused = false;
    bPromptVid2Focused = false;
}

void PlaylistGeminiUI::saveSettings(ofJson& pt) {
    pt["gemini"]["theme"] = themeText;
    pt["gemini"]["promptVid1"] = promptVid1Text;
    pt["gemini"]["promptVid2"] = promptVid2Text;
    
    auto saveR = [&](const string& key, const ofRectangle& r) {
        pt[key]["x"] = r.x; pt[key]["y"] = r.y; pt[key]["w"] = r.width; pt[key]["h"] = r.height;
    };
    
    saveR("gemini_apiKeyBox", apiKeyBox);
    saveR("gemini_listModelsBtn", listModelsBtn);
    saveR("gemini_themeBox", themeBox);
    saveR("gemini_promptVid1Box", promptVid1Box);
    saveR("gemini_promptVid2Box", promptVid2Box);
    saveR("gemini_genRoomBtn", genRoomBtn);
    saveR("gemini_genVidLastBtn", genVidLastBtn);
    saveR("gemini_genVid2LastBtn", genVid2LastBtn);
    saveR("gemini_modelAccordionBtn", modelAccordionBtn);
    for(int i=0; i<modelOptionRects.size(); i++) saveR("gemini_modelOpt_" + ofToString(i), modelOptionRects[i]);
    saveR("gemini_imageSizeBtn", imageSizeBtn);
    saveR("gemini_videoResBtn", videoResBtn);
    saveR("gemini_genTextToRoomBtn", genTextToRoomBtn);

    pt["gemini"]["currentModelIndex"] = currentModelIndex;
    pt["gemini"]["currentImageSizeIndex"] = currentImageSizeIndex;
    pt["gemini"]["currentVideoResIndex"] = currentVideoResIndex;
}

void PlaylistGeminiUI::loadSettings(const ofJson& pt) {
    if(pt.contains("gemini")) {
        themeText = pt["gemini"].value("theme", themeText);
        promptVid1Text = pt["gemini"].value("promptVid1", promptVid1Text);
        promptVid2Text = pt["gemini"].value("promptVid2", promptVid2Text);
        
        currentModelIndex = ofClamp(pt["gemini"].value("currentModelIndex", currentModelIndex), 
                                    0, std::max(0, (int)modelOptions.size() - 1));
        currentImageSizeIndex = ofClamp(pt["gemini"].value("currentImageSizeIndex", currentImageSizeIndex), 
                                        0, std::max(0, (int)imageSizeOptions.size() - 1));
        currentVideoResIndex = ofClamp(pt["gemini"].value("currentVideoResIndex", currentVideoResIndex), 
                                       0, std::max(0, (int)videoResOptions.size() - 1));
    }
    
    auto loadR = [&](const string& key, ofRectangle& r) {
        if(pt.contains(key)) {
            r.x = pt[key].value("x", r.x); r.y = pt[key].value("y", r.y);
            if(pt[key].contains("w")) r.width = pt[key].value("w", r.width);
            if(pt[key].contains("h")) r.height = pt[key].value("h", r.height);
        }
    };
    
    loadR("gemini_apiKeyBox", apiKeyBox);
    loadR("gemini_listModelsBtn", listModelsBtn);
    loadR("gemini_themeBox", themeBox);
    loadR("gemini_promptVid1Box", promptVid1Box);
    loadR("gemini_promptVid2Box", promptVid2Box);
    loadR("gemini_genRoomBtn", genRoomBtn);
    loadR("gemini_genVidLastBtn", genVidLastBtn);
    loadR("gemini_genVid2LastBtn", genVid2LastBtn);
    loadR("gemini_modelAccordionBtn", modelAccordionBtn);
    for(int i=0; i<modelOptionRects.size(); i++) loadR("gemini_modelOpt_" + ofToString(i), modelOptionRects[i]);
    loadR("gemini_imageSizeBtn", imageSizeBtn);
    loadR("gemini_videoResBtn", videoResBtn);
    loadR("gemini_genTextToRoomBtn", genTextToRoomBtn);
}

vector<ofRectangle*> PlaylistGeminiUI::getInteractableRects() {
    vector<ofRectangle*> rects;
    rects.push_back(&apiKeyBox);
    rects.push_back(&listModelsBtn);
    rects.push_back(&themeBox);
    rects.push_back(&promptVid1Box);
    rects.push_back(&promptVid2Box);
    rects.push_back(&genRoomBtn);
    rects.push_back(&genVidLastBtn);
    rects.push_back(&genVid2LastBtn);
    rects.push_back(&modelAccordionBtn);
    if (bModelAccordionOpen) {
        for(auto& r : modelOptionRects) rects.push_back(&r);
    }
    rects.push_back(&imageSizeBtn);
    rects.push_back(&videoResBtn);
    rects.push_back(&genTextToRoomBtn);
    return rects;
}

ofRectangle* PlaylistGeminiUI::findButtonAt(ofVec2f pos) {
    if(apiKeyBox.inside(pos)) return &apiKeyBox;
    if(listModelsBtn.inside(pos)) return &listModelsBtn;
    if(themeBox.inside(pos)) return &themeBox;
    if(promptVid1Box.inside(pos)) return &promptVid1Box;
    if(promptVid2Box.inside(pos)) return &promptVid2Box;
    if(genRoomBtn.inside(pos)) return &genRoomBtn;
    if(genVidLastBtn.inside(pos)) return &genVidLastBtn;
    if(genVid2LastBtn.inside(pos)) return &genVid2LastBtn;
    if(bModelAccordionOpen) {
        for(auto& r : modelOptionRects) if(r.inside(pos)) return &r;
    }
    if(modelAccordionBtn.inside(pos)) return &modelAccordionBtn;
    if(imageSizeBtn.inside(pos)) return &imageSizeBtn;
    if(videoResBtn.inside(pos)) return &videoResBtn;
    if(genTextToRoomBtn.inside(pos)) return &genTextToRoomBtn;
    return nullptr;
}

string PlaylistGeminiUI::getTooltip(ofVec2f worldM, PlaylistTooltipManager& tooltipManager) {
    if(apiKeyBox.inside(worldM)) return tooltipManager.getTooltipText("API_KEY_BOX");
    if(listModelsBtn.inside(worldM)) return tooltipManager.getTooltipText("SCAN_MODELS_BTN");
    if(themeBox.inside(worldM)) return tooltipManager.getTooltipText("THEME_BOX");
    if(promptVid1Box.inside(worldM)) return tooltipManager.getTooltipText("PROMPT_VID1_BOX");
    if(promptVid2Box.inside(worldM)) return tooltipManager.getTooltipText("PROMPT_VID2_BOX");
    if(genRoomBtn.inside(worldM)) return tooltipManager.getTooltipText("GEN_ROOM_360");
    if(genVidLastBtn.inside(worldM)) return tooltipManager.getTooltipText("GEN_VID_LAST");
    if(genVid2LastBtn.inside(worldM)) return tooltipManager.getTooltipText("GEN_VID_2_LAST");
    if(modelAccordionBtn.inside(worldM)) return tooltipManager.getTooltipText("MODEL_ACCORDION");
    if(imageSizeBtn.inside(worldM)) return tooltipManager.getTooltipText("IMG_SIZE_BTN");
    if(videoResBtn.inside(worldM)) return tooltipManager.getTooltipText("VID_RES_BTN");
    if(genTextToRoomBtn.inside(worldM)) return tooltipManager.getTooltipText("GEN_TEXT_ROOM");
    return "";
}