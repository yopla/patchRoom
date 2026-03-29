#pragma once
#include "ofMain.h"
#include "PhysicSamBase.h"
#include "ColliderLayer.h"
#include "SamUtils.h"

class SoftbodySam : public PhysicSamBase {
public:
    void setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) override;
    void updateInteraction() override;
    void draw() override;
    void wrap(float shiftX) override;

    // Paramètres de la matière molle
    float tessellationFineness = 10.0f; // Maillage fin pour mieux épouser le sol
    float rigidity = 0.03f;             // Maintien de forme (très bas = liquide/slime, 0.5 = jelly)
    float edgeStiffness = 0.6f;         // Tenseurs internes
    float damping = 0.98f;              // Friction interne (0.98 = coule bien)
    
    shared_ptr<ColliderLayer> collider;

private:
    ofVec2f lastRigidPos;
    float lastRigidAngle = 0.0f;

    // Système de particules pour la déformation
    vector<ofVec3f> origVerts;    // Sommets d'origine (locaux)
    vector<ofVec3f> currentVerts; // Sommets actuels (monde)
    vector<ofVec3f> lastVerts;    // Sommets précédents (pour l'intégration Verlet)
    
    vector<SamSpring> springs;    // Réseau de ressorts structurels
    vector<int> nodeVertexIndices;// Lien entre les points de collision et le maillage mou
};