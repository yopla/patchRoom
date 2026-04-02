#include "AliveSam.h"
//#include "ofxOpenCv.h"

void AliveSam::setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) {
    // 1. Base setup for physics properties and texture
    PhysicSamBase::setup(contour, bbox, sourceImg, scale, offsetY);

    // 2. Softbody setup (from SoftbodySam)
    ofPolyline fineShape = shape.getResampledBySpacing(tessellationFineness / scale);
    ofTessellator tess;
    ofMesh tempMesh;
    tess.tessellateToMesh(fineShape, ofPolyWindingMode::OF_POLY_WINDING_NONZERO, tempMesh);
    mesh = tempMesh;
    mesh.setUsage(GL_DYNAMIC_DRAW);

    mesh.clearTexCoords();
    for(auto& v : mesh.getVertices()) {
        float tx = ((v.x * scale) + bbox.width / 2.0f) / bbox.width;
        float ty = ((v.y * scale) + bbox.height / 2.0f) / bbox.height;
        mesh.addTexCoord(ofVec2f(tx, ty));
    }

    auto& mVerts = mesh.getVertices();
    origVerts.assign(mVerts.begin(), mVerts.end());
    currentVerts.clear();
    lastVerts.clear();
    
    for(auto& v : origVerts) {
        glm::vec3 worldPos = glm::vec3(pos.x, pos.y, 0) + glm::vec3(v.x, v.y, v.z);
        currentVerts.push_back(worldPos);
        lastVerts.push_back(worldPos);
    }

    auto& indices = mesh.getIndices();
    springs.clear();
    auto addSpring = [&](int a, int b) {
        if (a == b) return;
        for(auto& s : springs) { if ((s.a == a && s.b == b) || (s.a == b && s.b == a)) return; }
        SamSpring s; s.a = a; s.b = b; s.restLength = glm::distance(origVerts[a], origVerts[b]);
        springs.push_back(s);
    };
    for (size_t i = 0; i < indices.size(); i += 3) {
        addSpring(indices[i], indices[i+1]);
        addSpring(indices[i+1], indices[i+2]);
        addSpring(indices[i+2], indices[i]);
    }

    nodeVertexIndices.clear();
    for (auto& n : localNodes) {
        float minDist = 999999.0f; int bestIdx = 0;
        for (size_t i = 0; i < origVerts.size(); ++i) {
            float d = glm::distance(glm::vec2(origVerts[i].x, origVerts[i].y), glm::vec2(n.x, n.y));
            if (d < minDist) { minDist = d; bestIdx = i; }
        }
        nodeVertexIndices.push_back(bestIdx);
    }
    
    lastRigidPos = pos;
    lastRigidAngle = angle;

    // 3. "Alive" bone setup
    // bool isWide = bbox.getWidth() > bbox.getHeight();
    numBones = (int)ofRandom(4, 8); // Entre 4 et 7 os
    int numMotors = numBones - 1;

    motorAngles.resize(numMotors);
    angleLimits.resize(numMotors);
    motorSpeeds.resize(numMotors);

    for(int i = 0; i < numMotors; i++) {
        motorAngles[i] = 0.0f;
        angleLimits[i] = PI * ofRandom(0.2f, 0.9f);
        motorSpeeds[i] = (i % 2 == 0 ? 1.0f : -1.0f) * ofRandom(0.04f, 0.06f);
    }

    worldNodes.resize(numBones);
    worldAngles.resize(numBones);
}

void AliveSam::updateInteraction() {
    // --- Part 1: Update the invisible "Alive" skeleton ---
    for(int i = 0; i < motorSpeeds.size(); i++) {
        motorAngles[i] += motorSpeeds[i];
        if (motorAngles[i] > angleLimits[i]) { motorAngles[i] = angleLimits[i]; motorSpeeds[i] *= -1.0f; }
        else if (motorAngles[i] < -angleLimits[i]) { motorAngles[i] = -angleLimits[i]; motorSpeeds[i] *= -1.0f; }
    }
    
    vector<float> segmentAngles(numBones);
    segmentAngles[0] = 0.0f;
    for (int i = 0; i < motorSpeeds.size(); ++i) {
        segmentAngles[i+1] = segmentAngles[i] + motorAngles[i];
    }
    
    float boneLength = 21.0f;
    vector<ofVec2f> tempLocalNodes(numBones);
    tempLocalNodes[0].set(0, 0);
    for (int i = 0; i < motorSpeeds.size(); ++i) {
        tempLocalNodes[i+1] = tempLocalNodes[i] + ofVec2f(boneLength, 0).getRotatedRad(segmentAngles[i]);
    }
    
    ofVec2f com(0, 0);
    for(int i = 0; i < numBones; i++) com += tempLocalNodes[i];
    com /= numBones;
    for(int i = 0; i < numBones; i++) tempLocalNodes[i] -= com;
        
    for(int i = 0; i < numBones; i++) {
        worldAngles[i] = this->angle + segmentAngles[i];
        worldNodes[i] = this->pos + tempLocalNodes[i].getRotatedRad(this->angle);
    }

    // --- Part 2: Update the Soft Body ---
    ofVec2f deltaPos = this->pos - this->lastRigidPos;
    float deltaAngle = this->angle - this->lastRigidAngle;

    for(size_t i = 0; i < currentVerts.size(); i++) {
        currentVerts[i] += glm::vec3(deltaPos.x, deltaPos.y, 0);
        lastVerts[i] += glm::vec3(deltaPos.x, deltaPos.y, 0);
        currentVerts[i] += glm::vec3(this->vel.x, this->vel.y, 0);
        
        if (abs(deltaAngle) > 0.0001f || abs(this->angularVel) > 0.0001f) {
            ofVec2f local(currentVerts[i].x - this->pos.x, currentVerts[i].y - this->pos.y);
            local.rotateRad(deltaAngle + this->angularVel);
            currentVerts[i].x = this->pos.x + local.x;
            currentVerts[i].y = this->pos.y + local.y;
        }
    }
    
    this->vel.set(0,0); this->angularVel = 0; forceAccum.set(0,0); torqueAccum = 0;

    // 2. Verlet Physics
    for(size_t i = 0; i < currentVerts.size(); i++) {
        glm::vec3 velocity = currentVerts[i] - lastVerts[i];
        lastVerts[i] = currentVerts[i];
        velocity.y += 0.4f; // Gravity
        
        if (bIsDragged) {
            ofVec2f dragWorld = this->pos + dragPointLocal.getRotatedRad(this->angle);
            ofVec2f diff = dragTarget - dragWorld;
            velocity += glm::vec3(diff.x, diff.y, 0) * 0.02f; // Étire la créature vers la souris
        }
        
        currentVerts[i] += velocity * damping;
    }

    // 3. Attract vertices to the animated skeleton
    float attractionStrength = 0.05f; // Lowered strength
    for(size_t i = 0; i < currentVerts.size(); i++) {
        float minDst = 999999;
        int closestBone = 0;
        for(int j=0; j<numBones; ++j){
            float d = glm::distance(glm::vec2(currentVerts[i].x, currentVerts[i].y), glm::vec2(worldNodes[j].x, worldNodes[j].y));
            if(d < minDst){ minDst = d; closestBone = j; }
        }
        glm::vec3 goal = glm::vec3(worldNodes[closestBone].x, worldNodes[closestBone].y, 0);
        currentVerts[i] += (goal - currentVerts[i]) * attractionStrength;
    }

    // 4. SHAPE MATCHING (from SoftbodySam)
    glm::vec3 currentCOM(0,0,0);
    for(auto& v : currentVerts) currentCOM += v;
    currentCOM /= currentVerts.size();

    float sumCross = 0, sumDot = 0;
    for(size_t i = 0; i < currentVerts.size(); i++) {
        glm::vec2 p(currentVerts[i].x - currentCOM.x, currentVerts[i].y - currentCOM.y);
        glm::vec2 q(origVerts[i].x, origVerts[i].y);
        sumCross += (q.x * p.y - q.y * p.x);
        sumDot   += (q.x * p.x + q.y * p.y);
    }
    float optimalAngle = atan2(sumCross, sumDot);

    for(size_t i = 0; i < currentVerts.size(); i++) {
        glm::vec2 q(origVerts[i].x, origVerts[i].y);
        q = glm::rotate(q, optimalAngle);
        glm::vec3 goal = currentCOM + glm::vec3(q.x, q.y, 0);
        currentVerts[i] += (goal - currentVerts[i]) * rigidity;
    }

    // 5. Spring constraints
    for (int iter = 0; iter < 2; iter++) {
        for(auto& s : springs) {
            glm::vec3& p1 = currentVerts[s.a]; glm::vec3& p2 = currentVerts[s.b];
            glm::vec3 delta = p2 - p1;
            float dist = glm::length(delta);
            if (dist > 0.001f) {
                float diff = (dist - s.restLength) / dist;
                glm::vec3 offset = delta * 0.5f * diff * edgeStiffness;
                p1 += offset; p2 -= offset;
            }
        }
    }

    // 6. Collision
    if (collider) {
        for(size_t i = 0; i < currentVerts.size(); i++) {
            int safe = 0;
            while(collider->isWall(currentVerts[i].x, currentVerts[i].y) && safe < 5) {
                glm::vec2 normal;
                float eps = 4.0f;
                float nx = (collider->isWall(currentVerts[i].x - eps, currentVerts[i].y) ? 1 : 0) - (collider->isWall(currentVerts[i].x + eps, currentVerts[i].y) ? 1 : 0);
                float ny = (collider->isWall(currentVerts[i].x, currentVerts[i].y - eps) ? 1 : 0) - (collider->isWall(currentVerts[i].x, currentVerts[i].y + eps) ? 1 : 0);
                normal = glm::vec2(nx, ny);
                
                if (glm::length(normal) > 0.1f) {
                    normal = glm::normalize(normal);
                } else {
                    normal = glm::normalize(glm::vec2(currentCOM.x - currentVerts[i].x, currentCOM.y - currentVerts[i].y));
                    if (glm::length(normal) == 0) normal = glm::vec2(0, -1);
                }
                
                currentVerts[i] += glm::vec3(normal.x, normal.y, 0) * 2.0f;
                
                glm::vec3 vel = currentVerts[i] - lastVerts[i];
                lastVerts[i] = currentVerts[i] - vel * 0.3f;
                safe++;
            }
        }
    }

    // 7. Update proxy rigid body and mesh
    currentCOM = glm::vec3(0,0,0);
    for(auto& v : currentVerts) currentCOM += v;
    currentCOM /= currentVerts.size();

    this->pos.set(currentCOM.x, currentCOM.y);
    this->angle = optimalAngle;
    this->lastRigidPos = this->pos;
    this->lastRigidAngle = this->angle;

    for(size_t i = 0; i < currentVerts.size(); i++) {
        mesh.setVertex(i, ofVec3f(currentVerts[i].x, currentVerts[i].y, currentVerts[i].z));
    }
}

void AliveSam::draw() {
    ofPushStyle();
    ofSetColor(255);
    
    if (texture.isAllocated()) {
        texture.bind();
        mesh.draw();
        texture.unbind();
    } else {
        mesh.draw();
    }
    
    /* Debug draw for bones
    ofSetColor(255, 0, 0);
    for(const auto& bone : worldNodes) {
        ofDrawCircle(bone, 5);
    }
    */
    
    // Interaction utilisateur
    if (bIsDragged) {
        ofSetColor(255, 0, 0);
        ofDrawCircle(dragTarget, 5); // Dessine la cible de la souris
    }

    ofPopStyle();
}

void AliveSam::wrap(float shiftX) {
    PhysicSamBase::wrap(shiftX);
    lastRigidPos.x += shiftX;
    for (size_t i = 0; i < currentVerts.size(); i++) {
        currentVerts[i].x += shiftX;
        lastVerts[i].x += shiftX;
        mesh.setVertex(i, ofVec3f(currentVerts[i].x, currentVerts[i].y, currentVerts[i].z));
    }
    for (auto& n : worldNodes) n.x += shiftX;
}
