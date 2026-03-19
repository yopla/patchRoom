#include "PlaylistNodeGraph.h"
#include <algorithm>

void PlaylistNodeGraph::update(Scene360VideoPlayer* player, float cx, float cy, float radius) {
    if(!player) return;

    auto& videos = player->getVideos();
    
    // Optimisation : On ne reconstruit la structure que si la playlist a changé
    bool playlistChanged = false;
    if (videos.size() != lastPaths.size()) {
        playlistChanged = true;
    } else {
        for(size_t j = 0; j < videos.size(); j++) {
            if (videos[j].path != lastPaths[j]) {
                playlistChanged = true;
                break;
            }
        }
    }

    if (playlistChanged) {
        lastPaths.clear();
        for (auto& v : videos) lastPaths.push_back(v.path);

        std::set<string> uniqueFrames;
        std::set<string> startFrames;
        for(auto& v : videos) {
            uniqueFrames.insert(v.startFrame);
            uniqueFrames.insert(v.endFrame);
            startFrames.insert(v.startFrame);
        }

        // Détection des culs-de-sac
        deadEnds.clear();
        for(auto& f : uniqueFrames) {
            if(startFrames.find(f) == startFrames.end()) {
                deadEnds.push_back(f);
            }
        }

        nodes.clear();
        for(auto& f : uniqueFrames) {
            nodes[f] = VisualNode{f, ofVec2f()};
        }
    }

    // On met toujours à jour les positions car le panneau ou le zoom peuvent bouger
    int i = 0;
    int n = nodes.size();
    if(n > 0) {
        for(auto& kv : nodes) {
            float angle = TWO_PI * float(i) / float(n) - HALF_PI;
            kv.second.pos.set(cx + cos(angle) * radius, cy + sin(angle) * radius);
            i++;
        }
    }
}

void PlaylistNodeGraph::draw(Scene360VideoPlayer* player, const ofRectangle& diagramDropZone) {
    float cx = diagramDropZone.getCenter().x;
    float cy = diagramDropZone.getCenter().y;

    ofPushStyle();
    ofSetColor(255, 255, 255, 15);
    ofFill();
    ofDrawRectangle(diagramDropZone);
    ofSetColor(255, 255, 255, 200);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(diagramDropZone);
    ofPopStyle();

    if(!player || player->getVideos().empty()) {
        ofSetColor(255);
        string msg = "En attente de chargement de la playlist (Glissez le dossier ici)...";
        ofDrawBitmapStringHighlight(msg, cx - msg.length() * 4, cy, ofColor(0, 0, 0, 150), ofColor(255));
        return;
    }

    auto& videos = player->getVideos();
    int curIdx = player->getCurrentVideoIndex();
    int upcomingIdx = player->getUpcomingVideoIndex();
    float currentPos = player->getVideoPosition();

    ofPushStyle();
    ofSetLineWidth(2);
    for(int j = 0; j < videos.size(); ++j) {
        auto& v = videos[j];
        if(nodes.count(v.startFrame) && nodes.count(v.endFrame)) {
            ofVec2f p1 = nodes[v.startFrame].pos;
            ofVec2f p2 = nodes[v.endFrame].pos;
            bool isActive = (j == curIdx);
            bool isNext = (j == upcomingIdx);
            
            bool isSamePath = false;
            if (curIdx >= 0 && upcomingIdx >= 0 && curIdx < videos.size() && upcomingIdx < videos.size()) {
                if (videos[curIdx].startFrame == videos[upcomingIdx].startFrame && 
                    videos[curIdx].endFrame == videos[upcomingIdx].endFrame) {
                    isSamePath = true;
                }
            }

            bool isPlanned = false;
            int pathIndex = -1;
            int pathLength = 1;
            if (player && !player->getPlannedPath().empty()) {
                auto& p = player->getPlannedPath();
                auto it = std::find(p.begin(), p.end(), j);
                if (it != p.end()) {
                    isPlanned = true;
                    pathIndex = std::distance(p.begin(), it);
                    pathLength = p.size();
                }
            }

            if((isActive || isNext) && isSamePath && !isPlanned) {
                ofSetColor(255, 255, 0, 255);
                ofSetLineWidth(4);
            } else if(isActive) {
                ofSetColor(255, 50, 50, 255);
                ofSetLineWidth(4);
            } else if (isPlanned) {
                float t = pathLength > 1 ? (float)pathIndex / (pathLength - 1) : 0.0f;
                ofSetColor(50, 255 * (1.0f - t), 50 + 205 * t, 255);
                ofSetLineWidth(3.5);
            } else if (isNext) {
                ofSetColor(50, 255, 50, 200);
                ofSetLineWidth(3);
            } else {
                ofSetColor(100, 200, 255, 120);
                ofSetLineWidth(1.5);
            }

            float arrowPosPct = isActive ? currentPos : 0.7f;

            if(v.startFrame == v.endFrame) {
                ofVec2f dir = (p1 - ofVec2f(cx, cy)).getNormalized();
                ofVec2f loopCenter = p1 + dir * 60;
                ofNoFill();
                ofDrawCircle(loopCenter, 30);
                ofFill();
                
                float angle = arrowPosPct * TWO_PI;
                ofVec2f perp(-dir.y, dir.x);
                ofVec2f arrowPos = loopCenter - dir * 30 * cos(angle) + perp * 30 * sin(angle);
                ofVec2f tangent = dir * 30 * sin(angle) + perp * 30 * cos(angle);
                tangent.normalize();
                
                ofVec2f arrPerp(-tangent.y, tangent.x);
                ofDrawTriangle(arrowPos + tangent * 15, arrowPos + arrPerp * 10 - tangent * 6, arrowPos - arrPerp * 10 - tangent * 6);
                
                if (isPlanned) {
                    ofDrawBitmapStringHighlight(ofToString(pathLength - pathIndex), loopCenter.x - 4, loopCenter.y + 4, ofColor(0, 150), ofColor(255));
                }
            } else {
                ofDrawLine(p1, p2);
                
                ofVec2f dir = p2 - p1;
                ofVec2f nDir = dir.getNormalized();
                ofVec2f mid = p1 + dir * arrowPosPct;
                ofVec2f perp(-nDir.y, nDir.x);
                
                ofDrawTriangle(mid + nDir * 15, mid + perp * 10 - nDir * 6, mid - perp * 10 - nDir * 6);
                
                if (isPlanned) {
                    ofVec2f textPos = p1 + dir * 0.5f;
                    ofDrawBitmapStringHighlight(ofToString(pathLength - pathIndex), textPos.x - 4, textPos.y + 4, ofColor(0, 150), ofColor(255));
                }
            }
        }
    }

    for(auto& kv : nodes) {
        bool isDeadEnd = (std::find(deadEnds.begin(), deadEnds.end(), kv.first) != deadEnds.end());
        
        if(isDeadEnd) ofSetColor(255, 50, 50); 
        else ofSetColor(220); 
        
        ofFill();
        ofDrawCircle(kv.second.pos, 15);

        ofSetColor(255);
        if(isDeadEnd) {
            ofDrawBitmapStringHighlight(kv.first + " (CUL-DE-SAC)", kv.second.pos.x + 20, kv.second.pos.y + 6, ofColor(200, 0, 0), ofColor(255));
        } else {
            ofDrawBitmapStringHighlight(kv.first, kv.second.pos.x + 20, kv.second.pos.y + 6);
        }
    }

    ofPopStyle();
}

bool PlaylistNodeGraph::mousePressed(ofVec2f worldM, Scene360VideoPlayer* player) {
    if (!player) return false;
    for (auto& kv : nodes) {
        if (kv.second.pos.distance(worldM) < 30.0f) {
            player->planPathToNode(kv.first);
            return true;
        }
    }
    return false;
}

string PlaylistNodeGraph::getTooltip(ofVec2f worldM) {
    for (auto& kv : nodes) {
        if (kv.second.pos.distance(worldM) < 30.0f) {
            return "Clic : Calculer un chemin vers le noeud [" + kv.first + "]";
        }
    }
    return "";
}