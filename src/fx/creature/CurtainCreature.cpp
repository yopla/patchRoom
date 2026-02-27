#include "CurtainCreature.h"

//--------------------------------------------------------------
void CurtainConstraint::resolve() {
    glm::vec2 delta = p1->pos - p2->pos;
    float dist = glm::length(delta);
    
    if (dist > 0.0001f) {
        float diff = (restLength - dist) / dist;
        glm::vec2 offset = delta * diff * 0.5f;

        if (!p1->bPinned) p1->pos += offset;
        if (!p2->bPinned) p2->pos -= offset;
        
        // Stabilité accrue si un point est fixé
        if (p1->bPinned && !p2->bPinned) p2->pos -= offset;
        if (!p1->bPinned && p2->bPinned) p1->pos += offset;
    }
}

//--------------------------------------------------------------
void CurtainCreature::setup(float x, float y, float w, float h, string imgPath) {
    startX = x;
    startY = y;
    width = w;
    height = h;
    
    texture.load(imgPath);
    
    points.clear();
    constraints.clear();
    
    float spacingX = w / (float)cols;
    float spacingY = h / (float)rows;
    
    // 1. Création de la grille de points
    for (int j = 0; j <= rows; j++) {
        for (int i = 0; i <= cols; i++) {
            float px = startX + i * spacingX;
            float py = startY + j * spacingY;
            
            // Mapping UV
            float uPct = (float)i / cols;
            float vPct = (float)j / rows;
            
            glm::vec2 tCoord;
            if(texture.isAllocated()) {
                tCoord = texture.getTexture().getCoordFromPercent(uPct, vPct);
            } else {
                tCoord = glm::vec2(uPct, vPct);
            }
            
            CurtainPoint p(px, py, tCoord.x, tCoord.y);
            
            // On fixe la ligne du haut (Tringle à rideau)
            if (j == 0) p.pin(px, py);
            
            points.push_back(p);
        }
    }
    
    // 2. Création des contraintes (Ressorts)
    for (int j = 0; j <= rows; j++) {
        for (int i = 0; i <= cols; i++) {
            int idx = i + j * (cols + 1);
            
            // Lien Horizontal (Gauche)
            if (i > 0) {
                int leftIdx = (i - 1) + j * (cols + 1);
                constraints.push_back(CurtainConstraint(&points[idx], &points[leftIdx]));
            }
            
            // Lien Vertical (Haut)
            if (j > 0) {
                int topIdx = i + (j - 1) * (cols + 1);
                constraints.push_back(CurtainConstraint(&points[idx], &points[topIdx]));
            }
        }
    }
}

//--------------------------------------------------------------
void CurtainCreature::update(float mx, float my) {
    glm::vec2 mouse(mx, my);
    glm::vec2 mouseDelta = mouse - lastMouse;
    lastMouse = mouse;

    // 1. Interaction "Tug" (Tirer le tissu)
    if (isDragging) {
        for (auto& p : points) {
            if (p.bPinned) continue;
            float dist = glm::distance(p.pos, mouse);
            if (dist < mouseInfluenceSize) {
                // On applique le mouvement de la souris au point (comme dans le code AS3)
                // _px = _x - (mouse_delta) * factor
                p.oldPos = p.pos - mouseDelta * 1.8f;
            }
        }
    }

    // 2. Physique Verlet
    for (auto& p : points) {
        if (p.bPinned) continue;
        glm::vec2 vel = (p.pos - p.oldPos) * friction;
        p.oldPos = p.pos;
        p.pos += vel;
        p.pos.y += gravity;
    }

    // 3. Résolution des contraintes (5 itérations pour la rigidité)
    for (int k = 0; k < 5; k++) {
        for (auto& c : constraints) c.resolve();
    }
}

//--------------------------------------------------------------
void CurtainCreature::draw() {
    // if (!texture.isAllocated()) return;
    
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            int idx1 = i + j * (cols + 1);          
            int idx2 = (i + 1) + j * (cols + 1);    
            int idx3 = i + (j + 1) * (cols + 1);    
            int idx4 = (i + 1) + (j + 1) * (cols + 1); 
            
            mesh.addVertex(glm::vec3(points[idx1].pos, 0)); mesh.addTexCoord(points[idx1].texCoord);
            mesh.addVertex(glm::vec3(points[idx2].pos, 0)); mesh.addTexCoord(points[idx2].texCoord);
            mesh.addVertex(glm::vec3(points[idx3].pos, 0)); mesh.addTexCoord(points[idx3].texCoord);
            
            mesh.addVertex(glm::vec3(points[idx2].pos, 0)); mesh.addTexCoord(points[idx2].texCoord);
            mesh.addVertex(glm::vec3(points[idx4].pos, 0)); mesh.addTexCoord(points[idx4].texCoord);
            mesh.addVertex(glm::vec3(points[idx3].pos, 0)); mesh.addTexCoord(points[idx3].texCoord);
        }
    }
    
    ofSetColor(255);
    if(texture.isAllocated()) {
        texture.bind();
        mesh.draw();
        texture.unbind();
    } else {
        ofSetColor(200, 50, 50);
        mesh.draw();
        ofSetColor(0);
        mesh.drawWireframe();
    }
}

void CurtainCreature::mousePressed(float mx, float my) { isDragging = true; lastMouse = glm::vec2(mx, my); }
void CurtainCreature::mouseReleased(float mx, float my) { isDragging = false; }
