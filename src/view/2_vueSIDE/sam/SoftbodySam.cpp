#include "SoftbodySam.h"

void SoftbodySam::setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) {
    // Configuration de la physique rigide de base
    PhysicSamBase::setup(contour, bbox, sourceImg, scale, offsetY);

    // 1. Amélioration de la finesse de la tesselation pour un meilleur effet Soft Body
    // On rééchantillonne la forme d'origine pour ajouter plus de sommets sur les bords
    ofPolyline fineShape = shape.getResampledBySpacing(tessellationFineness / scale);
    
    ofTessellator tess;
    ofMesh tempMesh;
    tess.tessellateToMesh(fineShape, ofPolyWindingMode::OF_POLY_WINDING_NONZERO, tempMesh);
    
    // On remplace le mesh de base par le nouveau
    mesh = tempMesh;
    mesh.setUsage(GL_DYNAMIC_DRAW); // Important: permet d'animer les vertices à chaque frame

    // 2. Mapping UV de la texture sur le nouveau maillage
    mesh.clearTexCoords();
    for(auto& v : mesh.getVertices()) {
        float tx = ((v.x * scale) + bbox.width / 2.0f) / bbox.width;
        float ty = ((v.y * scale) + bbox.height / 2.0f) / bbox.height;
        mesh.addTexCoord(ofVec2f(tx, ty));
    }

    // 3. Initialisation du système de particules pour la gelée
    auto& mVerts = mesh.getVertices();
    origVerts.assign(mVerts.begin(), mVerts.end());
    currentVerts.clear();
    lastVerts.clear();
    
    for(auto& v : origVerts) {
        ofVec3f worldPos = ofVec3f(pos.x, pos.y, 0) + v.getRotated(ofRadToDeg(angle), ofVec3f(0,0,1));
        currentVerts.push_back(worldPos);
        lastVerts.push_back(worldPos);
    }

    // 4. Création des ressorts structurels (à partir des triangles du maillage)
    auto& indices = mesh.getIndices();
    springs.clear();
    
    auto addSpring = [&](int a, int b) {
        if (a == b) return;
        for(auto& s : springs) {
            if ((s.a == a && s.b == b) || (s.a == b && s.b == a)) return;
        }
        SamSpring s;
        s.a = a; s.b = b;
        s.restLength = origVerts[a].distance(origVerts[b]);
        springs.push_back(s);
    };

    for (size_t i = 0; i < indices.size(); i += 3) {
        addSpring(indices[i], indices[i+1]);
        addSpring(indices[i+1], indices[i+2]);
        addSpring(indices[i+2], indices[i]);
    }

    // 5. Connecter le solveur de collision (localNodes) aux sommets déformables
    nodeVertexIndices.clear();
    for (auto& n : localNodes) {
        float minDist = 999999.0f;
        int bestIdx = 0;
        for (size_t i = 0; i < origVerts.size(); ++i) {
            float d = origVerts[i].distance(ofVec3f(n.x, n.y, 0));
            if (d < minDist) { minDist = d; bestIdx = i; }
        }
        nodeVertexIndices.push_back(bestIdx);
    }
    
    lastRigidPos = pos;
    lastRigidAngle = angle;
}

void SoftbodySam::updateInteraction() {
    // 1. On absorbe les déplacements causés par les autres corps rigides
    ofVec2f deltaPos = this->pos - this->lastRigidPos;
    float deltaAngle = this->angle - this->lastRigidAngle;

    for(size_t i = 0; i < currentVerts.size(); i++) {
        currentVerts[i] += ofVec3f(deltaPos.x, deltaPos.y, 0);
        lastVerts[i]    += ofVec3f(deltaPos.x, deltaPos.y, 0); // Décale l'historique pour ne pas créer d'explosion de vélocité
        
        // Ajout de l'impulsion physique envoyée par les colliders externes
        currentVerts[i] += ofVec3f(this->vel.x, this->vel.y, 0);
        
        if (abs(deltaAngle) > 0.0001f || abs(this->angularVel) > 0.0001f) {
            ofVec2f local(currentVerts[i].x - this->pos.x, currentVerts[i].y - this->pos.y);
            local.rotateRad(deltaAngle + this->angularVel);
            currentVerts[i].x = this->pos.x + local.x;
            currentVerts[i].y = this->pos.y + local.y;
        }
    }
    
    // On purge le proxy rigide (c'est le liquide qui prend le contrôle du mouvement)
    this->vel.set(0,0);
    this->angularVel = 0;
    forceAccum.set(0,0);
    torqueAccum = 0;

    // 2. Physique Liquide par Particule (Verlet)
    for(size_t i = 0; i < currentVerts.size(); i++) {
        ofVec3f velocity = currentVerts[i] - lastVerts[i];
        lastVerts[i] = currentVerts[i];

        velocity.y += 0.4f; // Vraie Gravité indépendante par particule
        
        if (bIsDragged) {
            ofVec2f dragWorld = this->pos + dragPointLocal.getRotatedRad(this->angle);
            ofVec2f diff = dragTarget - dragWorld;
            velocity += ofVec3f(diff.x, diff.y, 0) * 0.02f; // Étire tout le liquide
        }
        
        currentVerts[i] += velocity * damping;
    }

    // 3. SHAPE MATCHING (Algorithme "As Rigid As Possible" pour un vrai volume)
    ofVec3f currentCOM(0,0,0);
    for(auto& v : currentVerts) currentCOM += v;
    currentCOM /= currentVerts.size();

    // Calcul de la rotation optimale pour réaligner le contour sans tordre l'image
    float sumCross = 0, sumDot = 0;
    for(size_t i = 0; i < currentVerts.size(); i++) {
        ofVec2f p(currentVerts[i].x - currentCOM.x, currentVerts[i].y - currentCOM.y);
        ofVec2f q(origVerts[i].x, origVerts[i].y);
        sumCross += (q.x * p.y - q.y * p.x);
        sumDot   += (q.x * p.x + q.y * p.y);
    }
    float optimalAngle = atan2(sumCross, sumDot);

    // On attire très doucement les points vers la forme optimale (c'est ce qui fait l'effet liquide !!)
    for(size_t i = 0; i < currentVerts.size(); i++) {
        ofVec2f q(origVerts[i].x, origVerts[i].y);
        q.rotateRad(optimalAngle);
        ofVec3f goal = currentCOM + ofVec3f(q.x, q.y, 0);
        currentVerts[i] += (goal - currentVerts[i]) * rigidity;
    }

    // 4. Maintien de l'intégrité du maillage (pour éviter les déchirures de texture)
    for (int iter = 0; iter < 2; iter++) {
        for(auto& s : springs) {
            ofVec3f& p1 = currentVerts[s.a];
            ofVec3f& p2 = currentVerts[s.b];
            ofVec3f delta = p2 - p1;
            float dist = delta.length();
            if (dist > 0.001f) {
                float diff = (dist - s.restLength) / dist;
                ofVec3f offset = delta * 0.5f * diff * edgeStiffness;
                p1 += offset;
                p2 -= offset;
            }
        }
    }

    // 5. Collisions EXACTES des sommets contre le décor (L'objet coule autour des obstacles)
    if (collider) {
        for(size_t i = 0; i < currentVerts.size(); i++) {
            int safe = 0;
            while(collider->isWall(currentVerts[i].x, currentVerts[i].y) && safe < 5) {
                ofVec2f normal;
                float eps = 4.0f;
                float nx = (collider->isWall(currentVerts[i].x - eps, currentVerts[i].y) ? 1 : 0) - (collider->isWall(currentVerts[i].x + eps, currentVerts[i].y) ? 1 : 0);
                float ny = (collider->isWall(currentVerts[i].x, currentVerts[i].y - eps) ? 1 : 0) - (collider->isWall(currentVerts[i].x, currentVerts[i].y + eps) ? 1 : 0);
                normal.set(nx, ny);
                
                if (normal.lengthSquared() > 0.1f) {
                    normal.normalize();
                } else {
                    normal = ofVec2f(currentCOM.x - currentVerts[i].x, currentCOM.y - currentVerts[i].y).getNormalized();
                    if (normal.lengthSquared() == 0) normal.set(0, -1);
                }
                
                currentVerts[i] += ofVec3f(normal.x, normal.y, 0) * 2.0f;
                
                // Friction forte (effet slime contre les murs)
                ofVec3f vel = currentVerts[i] - lastVerts[i];
                lastVerts[i] = currentVerts[i] - vel * 0.3f; 
                safe++;
            }
        }
    }

    // 6. Mise à jour de la texture et du Proxy rigide
    currentCOM.set(0,0,0);
    for(auto& v : currentVerts) currentCOM += v;
    currentCOM /= currentVerts.size();

    this->pos.set(currentCOM.x, currentCOM.y);
    this->angle = optimalAngle;
    this->lastRigidPos = this->pos;
    this->lastRigidAngle = this->angle;

    // Met à jour la carte graphique et le colliseur
    for(size_t i = 0; i < currentVerts.size(); i++) {
        mesh.setVertex(i, currentVerts[i]);
    }

    for (size_t i = 0; i < localNodes.size(); ++i) {
        int vIdx = nodeVertexIndices[i];
        ofVec3f worldPos = currentVerts[vIdx];
        ofVec2f localPos = ofVec2f(worldPos.x - pos.x, worldPos.y - pos.y).getRotatedRad(-angle);
        localNodes[i] = localNodes[i].getInterpolated(localPos, 0.4f); 
    }
}

void SoftbodySam::draw() {
    // Contrairement à la base, on ne fait PAS de translate(pos) et rotate(angle)
    // Car nos `currentVerts` sont déjà calculés dans l'espace global (World Space) !
    ofPushStyle();
    ofSetColor(255);
    
    if (texture.isAllocated()) {
        texture.bind();
        mesh.draw();
        texture.unbind();
    } else {
        mesh.draw();
    }

    // Interaction utilisateur
    if (bIsDragged) {
        ofSetColor(255, 0, 0);
        ofDrawCircle(dragTarget, 5); // Dessine la cible de la souris
    }
    ofPopStyle();
}

void SoftbodySam::wrap(float shiftX) {
    PhysicSamBase::wrap(shiftX);
    lastRigidPos.x += shiftX;
    for (size_t i = 0; i < currentVerts.size(); i++) {
        currentVerts[i].x += shiftX;
        lastVerts[i].x += shiftX;
        mesh.setVertex(i, currentVerts[i]);
    }
}