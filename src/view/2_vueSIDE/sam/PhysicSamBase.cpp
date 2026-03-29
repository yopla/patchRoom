#include "PhysicSamBase.h"

void PhysicSamBase::setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) {
    // Convertir la position du centre en coordonnées de simulation
    pos.x = bbox.getCenter().x / scale;
    pos.y = (bbox.getCenter().y + offsetY) / scale;

    shape = contour;
    ofVec2f imgCenter = bbox.getCenter();
    shape.translate(-imgCenter); // Center the shape around (0,0)

    // Appliquer le scale pour la simulation physique
    for (auto& v : shape.getVertices()) v /= scale;

    // --- Préparation du modèle physique granulaire (Façon moCf.as) ---
    nodeRadius = 2.0f; // Taille de nos perles de collision
    ofPolyline physShape = shape;
    physShape.simplify(1.0f);
    physShape = physShape.getResampledBySpacing(nodeRadius * 1.5f); // Noeuds serrés

    localNodes.clear();
    for (auto& p : physShape.getVertices()) {
        localNodes.push_back(ofVec2f(p.x, p.y));
    }

    // Recalcul exact du Centre de Masse pour une vraie inertie
    ofVec2f com(0,0);
    for(auto& n : localNodes) com += n;
    if (!localNodes.empty()) com /= localNodes.size();
    
    for(auto& n : localNodes) n -= com;
    for(auto& v : shape.getVertices()) {
        v.x -= com.x;
        v.y -= com.y;
    } // Aligner le visuel aussi
    pos += com;
    origPos = pos;

    // --- Triangulation pour le rendu ---
    ofTessellator tess;
    ofMesh tempMesh;
    tess.tessellateToMesh(shape, ofPolyWindingMode::OF_POLY_WINDING_NONZERO, tempMesh);
    mesh = tempMesh; // Charge dans le VboMesh pour la performance
    holeMesh = tempMesh; // Copie statique pour le trou (ne sera pas affecté par Softbody)

    // --- Application de la Texture ---
    texture.cropFrom(sourceImg, bbox.x, bbox.y, bbox.width, bbox.height);
    
    // L'App utilise ofDisableArbTex(), les coordonnées UV doivent donc être normalisées entre 0 et 1
    mesh.clearTexCoords();
    for(auto& v : mesh.getVertices()) {
        float tx = ((v.x * scale) + bbox.width / 2.0f) / bbox.width;
        float ty = ((v.y * scale) + bbox.height / 2.0f) / bbox.height;
        mesh.addTexCoord(ofVec2f(tx, ty));
    }

    vel.set(0,0);
    angle = 0;
    angularVel = 0;
    forceAccum.set(0,0);
    torqueAccum = 0;

    // Calcul des vraies propriétés physiques
    maxRadius = 0;
    inertia = 0;
    for(auto& n : localNodes) {
        float distSq = n.lengthSquared();
        if (distSq > maxRadius * maxRadius) maxRadius = sqrt(distSq);
        inertia += 1.5f * distSq;
    }
    maxRadius += nodeRadius;

    mass = localNodes.size() * 1.5f;
    if (mass < 10.0f) mass = 10.0f;
    invMass = 1.0f / mass;
    
    if (inertia < 10.0f) inertia = 10.0f;
    invInertia = 1.0f / inertia;
}

void PhysicSamBase::draw() {
    ofPushMatrix();
    ofTranslate(pos);
    ofRotateRad(angle);
    
    ofPushStyle();
    ofSetColor(255);
    
    if (texture.isAllocated()) {
        texture.bind();
        mesh.draw();
        texture.unbind();
    }

    // Point d'ancrage visuel pour les Gears
    if (invMass == 0.0f) {
        ofSetColor(200, 50, 50, 200);
        ofFill();
        ofDrawCircle(0, 0, 8);
        ofSetColor(255);
        ofNoFill();
        ofDrawCircle(0, 0, 8);
    }

    // Rendu spécifique si sélectionné
    if (bIsDragged) {
        ofSetColor(255, 255, 0, 150);
        ofNoFill();
        ofSetLineWidth(2);
        shape.draw();
        
        ofSetColor(255, 0, 0);
        ofDrawCircle(dragPointLocal, 5);
    }
    ofPopStyle();
    ofPopMatrix();

    // Dessin de l'élastique de contrainte
    if (bIsDragged) {
        ofPushStyle();
        ofVec2f dragWorld = pos + dragPointLocal.getRotated(ofRadToDeg(angle));
        
        if (dragWorld.squareDistance(dragTarget) > 150.0f * 150.0f) {
            ofSetColor(255, 100, 100, 200); // Rouge quand la tension est maximale
            ofSetLineWidth(2.0f);
        } else {
            ofSetColor(255, 255, 0, 200); // Jaune quand l'objet suit bien
            ofSetLineWidth(3.0f);
        }
        
        ofDrawLine(dragWorld, dragTarget);
        ofPopStyle();
    }
}

void PhysicSamBase::drawHole(float shadowAngle, float shadowDist) {
    ofPushStyle();
    ofEnableAlphaBlending();
    
    ofPushMatrix();
    ofTranslate(origPos);
    
    // Le vecteur de décalage de l'ombre
    ofVec2f offset(cos(ofDegToRad(shadowAngle)) * shadowDist, sin(ofDegToRad(shadowAngle)) * shadowDist);
    
    // Initialisation des couleurs du maillage si ce n'est pas déjà fait
    if (holeMesh.getNumColors() != holeMesh.getNumVertices()) {
        holeMesh.clearColors();
        for (size_t i = 0; i < holeMesh.getNumVertices(); i++) {
            holeMesh.addColor(ofColor(0, 0, 0, 0));
        }
    }
    
    // Trou de base (plus clair, 60% de noir)
    float lightAlpha = 255.0f * 0.60f; 
    // Ombre interne supplémentaire (75% de noir)
    float darkAlpha = 255.0f * 0.75f; 
    
    // Calcul de l'ombre interne par vertex (découpe nette basée sur la forme)
    for (size_t i = 0; i < holeMesh.getNumVertices(); i++) {
        const ofVec3f& v = holeMesh.getVertices()[i];
        
        // Un point est dans la lumière s'il se trouve dans la projection de l'ouverture (forme décalée)
        bool inLight = shape.inside(v.x - offset.x, v.y - offset.y);
        
        float alpha = inLight ? lightAlpha : darkAlpha;
        holeMesh.setColor(i, ofColor(0, 0, 0, alpha));
    }
    
    holeMesh.draw();
    ofPopMatrix();

    ofPopStyle();
}

void PhysicSamBase::applyForce(const ofVec2f& force, const ofVec2f& point) {
    forceAccum += force;
    ofVec2f r = point - pos;
    torqueAccum += r.x * force.y - r.y * force.x;
}

void PhysicSamBase::wrap(float shiftX) {
    pos.x += shiftX;
    origPos.x += shiftX;
    dragTarget.x += shiftX;
}