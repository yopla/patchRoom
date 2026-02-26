#include "WormSystem.h"

//--------------------------------------------------------------
void WormSystem::setup(int count) {
    agents.resize(count);
    for (auto& agent : agents) {
        // Entre 4 et 7 segments pour se courber
        agent.numSegments = (int)ofRandom(4, 8);
        agent.speed = ofRandom(0.01f, 0.03f);
        agent.segmentSpacing = ofRandom(15.0f, 30.0f);
        agent.isNew = true;
        agent.color = ofColor::fromHsb(ofRandom(255), 200, 255);
    }
}

//--------------------------------------------------------------
void WormSystem::spawnAgent(WormAgent& agent, RoomWalls& walls) {
    ofVec3f center(0, 600, 0);
    ofVec3f randomDir = ofVec3f(ofRandom(-1, 1), ofRandom(-0.5, 0.5), ofRandom(-1, 1)).getNormalized();

    float minDistance = 100000.0f;
    ofVec3f bestHit = center;
    bool found = false;

    auto check = [&](ofMesh& mesh) {
        if (mesh.getNumVertices() < 3) return;
        ofVec3f v0 = mesh.getVertex(0);
        ofVec3f v1 = mesh.getVertex(1);
        ofVec3f v2 = mesh.getVertex(2);
        ofVec3f normal = ((v1 - v0).cross(v2 - v0)).getNormalized();

        float denom = normal.dot(randomDir);
        if (abs(denom) > 0.0001f) {
            float t = (v0 - center).dot(normal) / denom;
            if (t > 0 && t < minDistance) {
                ofVec3f p = center + randomDir * t;
                // Simple bounding box check to stay within the room
                if (abs(p.x) < 1300 && abs(p.z) < 1400 && p.y > -10 && p.y < 1700) {
                    minDistance = t;
                    bestHit = p;
                    found = true;
                }
            }
        }
    };

    check(walls.meshFront);
    check(walls.meshBack);
    check(walls.meshCour);
    check(walls.meshJar);
    check(walls.meshSol);
    check(walls.meshTopCour);
    check(walls.meshTopJar);

    if (!found) {
        bestHit = ofVec3f(ofRandom(-1000, 1000), 0, ofRandom(-1000, 1000));
    }

    agent.targetPos = bestHit;
    
    // If it's a new agent, it starts at the target
    if (agent.isNew) {
        agent.headPos = bestHit;
        agent.segments.assign(agent.numSegments, bestHit);
        agent.isNew = false;
    }
}

//--------------------------------------------------------------
void WormSystem::update(RoomWalls& walls) {
    for (auto& agent : agents) {
        if (agent.isNew || agent.headPos.distance(agent.targetPos) < 20.0f) {
            spawnAgent(agent, walls);
        }

        // Déplacement de la tête
        agent.headPos.interpolate(agent.targetPos, agent.speed);

        // Mise à jour des segments (Suivi de type "Drag")
        if(agent.segments.size() != agent.numSegments) {
            agent.segments.assign(agent.numSegments, agent.headPos);
        }

        ofVec3f target = agent.headPos;
        for(int i=0; i<agent.segments.size(); i++) {
            ofVec3f dir = agent.segments[i] - target;
            float dist = dir.length();
            
            // Si le segment est trop loin, on le tire vers la cible (tête ou segment précédent)
            if(dist > agent.segmentSpacing) {
                ofVec3f pull = dir.getNormalized() * agent.segmentSpacing;
                agent.segments[i] = target + pull;
            }
            
            // Le prochain segment suit celui-ci
            target = agent.segments[i];
        }
    }
}

//--------------------------------------------------------------
void WormSystem::draw(RoomWalls& walls) {
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(3.0f);
    
    // Offset pour dessiner par dessus les murs
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-4.0, -4.0);

    for (auto& agent : agents) {
        ofSetColor(agent.color);
        ofPolyline line;
        line.addVertex(agent.headPos);
        for(const auto& seg : agent.segments) {
            line.addVertex(seg);
        }
        
        // Lissage spline
        line.getSmoothed(4).draw();
    }

    glDisable(GL_POLYGON_OFFSET_LINE);
    ofPopStyle();
}