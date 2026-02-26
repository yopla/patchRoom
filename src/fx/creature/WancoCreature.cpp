#include "WancoCreature.h"

//--------------------------------------------------------------
WancoCreature::WancoCreature(float startX, float startY, ofImage& sharedImg) {
    texture = &sharedImg;
    
    // Position initiale
    centerX = startX;
    centerY = startY;
    
    // Paramètres 2.5D (Similaire au script AS3)
    focus = 400.0f;
    
    // Init Wanco (Le chien)
    pos = glm::vec3(0, 0, 0); // X, Y (hauteur), Z (profondeur)
    rotation = 0;
    jumpTime = 0;

    // Init Hae (Le papillon)
    targetBase = glm::vec3(0, 100, 0); // Y inversé par rapport à AS3 car OF Y+ est en bas
    targetPos = glm::vec3(0, 0, 0);
    targetAngle = 0;
}

//--------------------------------------------------------------
WancoCreature::~WancoCreature() {
}

//--------------------------------------------------------------
void WancoCreature::update(float mx, float my) {
    // 1. LOGIQUE DU PAPILLON (CIBLE)
    // Conversion souris locale par rapport au centre de la créature
    float localMx = mx - centerX;
    float localMy = my - centerY;

    // Déplacement fluide de la base vers la souris
    // Note: Dans le script AS3, MouseY contrôle la profondeur (Z)
    targetBase.x += (localMx - targetBase.x) * 0.1f;
    targetBase.z += (localMy - targetBase.z) * 0.1f; // On map Y souris sur Z profondeur
    
    // Mouvement circulaire du papillon autour de sa base
    targetAngle -= 0.15f;
    float radius = 40.0f;
    targetPos.x = targetBase.x + cos(targetAngle) * radius;
    targetPos.z = targetBase.z + sin(targetAngle) * radius;
    // targetPos.y reste fixe ou flotte légèrement pour le papillon
    
    // 2. LOGIQUE DU CHIEN (WANCO)
    // Il suit le papillon avec un délai (Lerp)
    pos.x += (targetPos.x - pos.x) * 0.05f;
    pos.z += (targetPos.z - pos.z) * 0.05f;
    
    // Calcul de l'angle pour regarder le papillon
    // atan2(deltaX, deltaZ)
    float dx = targetPos.x - pos.x;
    float dz = targetPos.z - pos.z;
    float targetRot = atan2(dx, dz) * RAD_TO_DEG; 
    
    // Lissage de la rotation
    float angleDiff = targetRot - rotation;
    // Normalisation de l'angle pour éviter le "tour complet" inutile
    while (angleDiff < -180) angleDiff += 360;
    while (angleDiff > 180) angleDiff -= 360;
    rotation += angleDiff * 0.1f;

    // 3. ANIMATION DU SAUT (Remplacement de BetweenAS3)
    // On crée un saut périodique simple
    jumpTime += 0.1f;
    // pos.y représente la hauteur (le saut). 
    // abs(sin) fait un rebond. On inverse car Y est bas en OF.
    float jumpHeight = abs(sin(jumpTime)) * 60.0f; 
    pos.y = -jumpHeight; 
}

//--------------------------------------------------------------
void WancoCreature::draw() {
    ofEnableAlphaBlending();

    // --- MODE 2D (Correction demandée) ---
    float ratio = 1.0f; // Taille fixe, pas de zoom selon Y
    
    // Coordonnées écran directes pour bien suivre la souris
    float screenX = centerX + pos.x;
    float screenY = centerY + pos.z; // On utilise Z comme Y écran
    float jumpScreenY = screenY + pos.y; // On ajoute la hauteur du saut

    // 1. DESSIN DE L'OMBRE (Au sol)
    ofSetColor(0, 0, 0, 50);
    float shadowScale = ratio * (1.0f - (abs(pos.y)/150.0f)); // L'ombre rétrécit quand il saute
    ofDrawEllipse(screenX, screenY, 60 * shadowScale, 30 * shadowScale);

    // 2. DESSIN DU "CHIEN"
    ofSetColor(255);
    ofPushMatrix();
        ofTranslate(screenX, jumpScreenY);
        ofScale(ratio, ratio);
        
        // Rotation (On inverse car l'axe Z pointe vers l'écran ou le fond selon convention)
        ofRotateDeg(-rotation); 
        
        // On dessine l'image partagée (le blob)
        // On simule l'étirement du saut (squash & stretch)
        float stretch = 1.0f - (pos.y * 0.005f); // s'étire quand il saute haut (pos.y est négatif)
        
        if(texture->isAllocated()){
            float w = 200; 
            float h = 200;
            // Centrage de l'image
            texture->draw(-w/2, -h/2 * stretch, w, h * stretch);
        } else {
            // Fallback si pas d'image
            ofDrawRectangle(-50, -50, 100, 100);
        }
        
    ofPopMatrix();

    // 3. DESSIN DU PAPILLON (DEBUG VISUEL)
    float flyScreenX = centerX + targetPos.x;
    float flyScreenY = centerY + targetPos.z - 100; // -100 hauteur vol

    ofSetColor(255, 255, 0); // Jaune
    ofDrawCircle(flyScreenX, flyScreenY, 15);
    ofSetColor(255); // Jaune
}