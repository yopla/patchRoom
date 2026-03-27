#pragma once
#include "ofMain.h"
#include <map>
#include <string>

class PlaylistTooltipManager {
public:
    PlaylistTooltipManager();
    void setup();
    void drawTooltip(const std::string& text, float x, float y);
    
    std::string getTooltipText(const std::string& key);

private:
    std::map<std::string, std::string> texts;
};