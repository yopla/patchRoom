#include "PosterSystem.h"

void PosterSystem::setup(float roomWidth, float roomDepth, float heightFrontBack) {
    if(!posterImg.load("IMG_REF/garde.png")) {
        posterImg.allocate(posterW, posterH, OF_IMAGE_COLOR);
        posterImg.setColor(ofColor::cyan);
    }
    
    totalPerimeter = (roomWidth * 2) + (roomDepth * 2);
    posterGlobalX = roomWidth / 2.0f - posterW / 2.0f;
    posterY = heightFrontBack / 2.0f - posterH / 2.0f;
    maxWallHeight = 784.0f; // Hauteur du mur le plus bas (JAR)
}

void PosterSystem::update() {
    if (ofGetKeyPressed('u')) posterY += posterSpeed;
    if (ofGetKeyPressed('j')) posterY -= posterSpeed;
    if (ofGetKeyPressed('h')) posterGlobalX -= posterSpeed;
    if (ofGetKeyPressed('k')) posterGlobalX += posterSpeed;

    // Wrapping horizontal
    if (posterGlobalX < 0) posterGlobalX += totalPerimeter;
    if (posterGlobalX >= totalPerimeter) posterGlobalX -= totalPerimeter;

    // Clamp Vertical
    posterY = ofClamp(posterY, 0, maxWallHeight - posterH);
}

void PosterSystem::draw(float roomWidth, float roomDepth) {
    if(!posterImg.isAllocated()) return;

    ofSetColor(255);
    struct Segment { float start; float end; float texOffsetX; };
    vector<Segment> segmentsToDraw;

    // Gestion du wrapping (si le poster chevauche la fin et le début du périmètre)
    if (posterGlobalX + posterW <= totalPerimeter) {
        segmentsToDraw.push_back({ posterGlobalX, posterGlobalX + posterW, 0.0f });
    } else {
        float part1W = totalPerimeter - posterGlobalX;
        segmentsToDraw.push_back({ posterGlobalX, totalPerimeter, 0.0f });
        segmentsToDraw.push_back({ 0.0f, posterW - part1W, part1W });
    }

    float w = roomWidth;
    float d = roomDepth;
    float wallStarts[4] = { 0, w, w+d, 2*w+d };
    float wallEnds[4]   = { w, w+d, 2*w+d, 2*w+2*d };

    for (auto& seg : segmentsToDraw) {
        for (int i = 0; i < 4; i++) {
            float overlapStart = std::max(seg.start, wallStarts[i]);
            float overlapEnd   = std::min(seg.end, wallEnds[i]);

            if (overlapStart < overlapEnd) {
                float drawW = overlapEnd - overlapStart;
                float xOnWall = overlapStart - wallStarts[i];
                float texX = (overlapStart - seg.start) + seg.texOffsetX;

                drawPosterSegmentOnWall(i, xOnWall, texX, drawW, roomWidth, roomDepth);
            }
        }
    }
}

void PosterSystem::drawPosterSegmentOnWall(int wallIndex, float xOnWall, float texX, float drawW, float roomWidth, float roomDepth) {
    ofPushMatrix();
    float w2 = roomWidth / 2.0f;
    float d2 = roomDepth / 2.0f;
    float epsilon = 2.0f; // Anti-clipping (Z-fighting)

    switch(wallIndex) {
        case 0: // FRONT
            ofTranslate(-w2, 0, -d2 + epsilon);
            break;
        case 1: // COUR
            ofTranslate(w2 - epsilon, 0, -d2);
            ofRotateYDeg(-90);
            break;
        case 2: // BACK
            ofTranslate(w2, 0, d2 - epsilon);
            ofRotateYDeg(180);
            break;
        case 3: // JAR
            ofTranslate(-w2 + epsilon, 0, d2);
            ofRotateYDeg(90);
            break;
    }

    posterImg.drawSubsection(xOnWall, posterY, drawW, posterH, texX, 0, drawW, posterH);
    ofPopMatrix();
}