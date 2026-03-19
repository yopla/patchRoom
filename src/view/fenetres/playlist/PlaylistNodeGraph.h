#pragma once

#include "ofMain.h"
#include "Scene360VideoPlayer.h"
#include <map>
#include <set>
#include <vector>

struct VisualNode {
    string name;
    ofVec2f pos;
};

class PlaylistNodeGraph {
public:
    void update(Scene360VideoPlayer* player, float cx, float cy, float radius);
    void draw(Scene360VideoPlayer* player, const ofRectangle& dropZone);
    bool mousePressed(ofVec2f worldM, Scene360VideoPlayer* player);
    string getTooltip(ofVec2f worldM);

    std::map<string, VisualNode> nodes;
    vector<string> deadEnds;

private:
    vector<string> lastPaths;
};