#include "JellyCreature.h"

// ---------------------------------------------------------
// OSCILLATOR
// ---------------------------------------------------------
const float Oscillator::K = 0.5;
const float Oscillator::R = 0.899;

void Oscillator::update() {
    float param = K * spring / z / mass;
    vx = (vx - param * x) * R;
    vy = (vy - param * y) * R;
    x += vx; y += vy;
}

void Oscillator::addForce(float fx, float fy) {
    float param = z / mass;
    vx += param * fx; vy += param * fy;
}

// ---------------------------------------------------------
// DOME FUNCTION
// ---------------------------------------------------------
DomeFunction::DomeFunction(float radius, float height, float depth) {
    gamma = depth; 
    beta = height + gamma;
    alpha = 0; 
}

float DomeFunction::domeRadius(float height) {
    return alpha * basicInverse((height + gamma) / beta);
}

CircFunction::CircFunction(float r, float h, float d) : DomeFunction(r, h, d) {
    gamma = d; beta = h + gamma;
    alpha = r / basicInverse(gamma / beta);
}

float CircFunction::basicInverse(float y) {
    float v = 1 - y * y;
    return (v < 0) ? 0 : sqrt(v);
}

// ---------------------------------------------------------
// DISK
// ---------------------------------------------------------
Disk::Disk(float w, float h, float trimX, float trimY, float trimR, int _n) {
    x = 0; y = 0; r = trimR; n = _n;
    dRad = TWO_PI / n;
    initializeUV(w, h, trimX, trimY, trimR);
    updateVerticesLocal();
}

void Disk::initializeUV(float w, float h, float trimX, float trimY, float trimR) {
    for (int i = 0; i < n; i++) {
        float c = cos(i * dRad);
        float s = sin(i * dRad);
        _uvData.push_back((trimX + trimR * c) / w);
        _uvData.push_back((trimY + trimR * s) / h);
    }
}

void Disk::updateVerticesLocal() {
    _vertices.clear();
    float rad = rotation;
    for (int i = 0; i < n; i++) {
        _vertices.push_back(x + r * cos(rad));
        _vertices.push_back(y + r * sin(rad));
        rad += dRad;
    }
}

void Disk::moveTo(float targetX, float targetY) {
    float dx = targetX - x;
    float dy = targetY - y;
    x += dx; y += dy;
    for (size_t i = 0; i < _vertices.size(); i += 2) {
        _vertices[i] += dx;
        _vertices[i+1] += dy;
    }
}

void Disk::mergeVertices(vector<float>& target) {
    target.insert(target.end(), _vertices.begin(), _vertices.end());
}

void Disk::mergeUVData(vector<float>& target) {
    target.insert(target.end(), _uvData.begin(), _uvData.end());
}

void Disk::updateGlobalVertices(vector<float>& target, int startIndex) {
    for (size_t i = 0; i < _vertices.size(); i++) {
        target[startIndex + i] = _vertices[i];
    }
}

// ---------------------------------------------------------
// DOME (OPTIMISÉ 4K)
// ---------------------------------------------------------
Dome::Dome(ofImage img, float _x, float _y, float _r, float _h, int polygon, int diskNumber) {
    this->bitmapData = img;
    this->x = _x;
    this->y = _y;
    this->r = _r;
    this->h = _h;
    this->n = polygon;

    func = new CircFunction(_r, _h, 0.1 * _h);

    vertices.push_back(0.0); vertices.push_back(0.0);
    uvtData.push_back(_x / (float)bitmapData.getWidth());
    uvtData.push_back(_y / (float)bitmapData.getHeight());

    float thickness = _h / diskNumber;

    for (int i = diskNumber - 1; i > -1; i--) {
        float heightVal = i * thickness;
        float radius = func->domeRadius(heightVal);

        Disk disk(img.getWidth(), img.getHeight(), _x, _y, radius, polygon);
        disks.push_back(disk);

        startIndices.push_back(vertices.size());
        disk.mergeVertices(vertices);
        disk.mergeUVData(uvtData);

        Oscillator osc;
        osc.mass = thickness * radius * radius;
        osc.z = heightVal;
        osc.spring = _h * _r * _r;
        oscillators.push_back(osc);
    }

    buildIndices();

    // --- SETUP MESH OPTIMISÉ ---
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    // Note: On n'appelle pas setUsage() car il n'est pas dispo sur ofMesh standard
    // mais ofMesh gère bien les mises à jour dynamiques par défaut.

    int numVertices = vertices.size() / 2; 
    
    for(int i = 0; i < numVertices; i++) {
        // Init vertices à 0 (mis à jour dans update)
        mesh.addVertex(ofVec3f(0, 0, 0));
        
        // UVs fixes
        float u = uvtData[i*2];
        float v = uvtData[i*2+1];
        mesh.addTexCoord(ofVec2f(u, v));
    }

    // Indices fixes (Topology)
    mesh.addIndices(indices);
}

Dome::~Dome() {
    if (func) delete func;
}

void Dome::buildIndices() {
    indices.clear();
    // Les boucles utilisent 'int' mais on push dans un vector<ofIndexType>, le cast est implicite
    for (int i = 1; i < n; i++) { indices.push_back(0); indices.push_back(i); indices.push_back(i + 1); }
    indices.push_back(0); indices.push_back(n); indices.push_back(1);

    int len = disks.size();
    for (int j = 1; j < len; j++) {
        int jn = j * n;
        int j1n = (j - 1) * n;
        for (int i = 1; i < n; i++) {
            int i1 = i + 1;
            indices.push_back(j1n + i); indices.push_back(jn + i); indices.push_back(jn + i1);
            indices.push_back(j1n + i); indices.push_back(jn + i1); indices.push_back(j1n + i1);
        }
        indices.push_back(jn); indices.push_back(jn + n); indices.push_back(jn + 1);
        indices.push_back(jn); indices.push_back(jn + 1); indices.push_back(j1n + 1);
    }
    std::reverse(indices.begin(), indices.end());
}

void Dome::update(float localMX, float localMY) {
    // 1. Physique (CPU)
    float distMouse = ofDist(localMX, localMY, x, y);
    isMouseOn = (distMouse < r);

    float fx = 0, fy = 0;
    if (isMouseOn) {
        float f0 = 0.001 * h * r * r;
        float dx = localMX - x;
        float dy = localMY - y;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist == 0) dist = 0.001;
        fx = - f0 * dx / dist;
        fy = - f0 * dy / dist;
    }

    int len = disks.size();
    for (int i = len - 2; i > -1; i--) {
        Oscillator& osc = oscillators[i];
        if (isMouseOn) osc.addForce(fx, fy);
        osc.update();
        disks[i].moveTo(disks[i+1].x + osc.x, disks[i+1].y + osc.y);
        disks[i].updateGlobalVertices(vertices, startIndices[i]);
    }

    Disk& disk0 = disks[0];
    Disk& disk1 = disks[1];
    vertices[0] = 2 * disk0.x - disk1.x;
    vertices[1] = 2 * disk0.y - disk1.y;
    
    // 2. Mise à jour GPU (Rapide)
    int numVertices = vertices.size() / 2;
    for(int i = 0; i < numVertices; i++) {
        float vx = vertices[i*2];
        float vy = vertices[i*2+1];
        // Injection directe des nouvelles coordonnées
        mesh.setVertex(i, ofVec3f(vx + x, vy + y, 0));
    }
}

void Dome::display() {
    // Rendu optimisé : un seul appel de draw()
    bitmapData.bind();
    mesh.draw();
    bitmapData.unbind();
}

// ---------------------------------------------------------
// JELLY CREATURE
// ---------------------------------------------------------
JellyCreature::JellyCreature(float _x, float _y, float _w, float _h, ofImage _sourceImg) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;

    img = _sourceImg; 
    img.crop(0, 0, _sourceImg.getWidth(), _sourceImg.getHeight());
    img.resize((int)w, (int)h);
}

JellyCreature::~JellyCreature() {
    if (currentDome != nullptr) delete currentDome;
}

bool JellyCreature::isInside(float mx, float my) {
    return (mx >= x && mx < x + w && my >= y && my < y + h);
}

void JellyCreature::onPress(float mx, float my) {
    if (isInside(mx, my)) {
        isDrawingCircle = true;
        localCircleX = mx - x;
        localCircleY = my - y;
        circleR = 0;
        
        if (currentDome != nullptr) {
            delete currentDome;
            currentDome = nullptr;
        }
    }
}

void JellyCreature::onRelease(float mx, float my) {
    if (isDrawingCircle) {
        isDrawingCircle = false;
        circleR = ofDist(mx, my, localCircleX + x, localCircleY + y);

        if (circleR > 5) {
            createDome(localCircleX, localCircleY, circleR);
        }
    }
}

void JellyCreature::createDome(float lx, float ly, float lr) {
    currentDome = new Dome(img, lx, ly, lr, lr, 12, 10);
}


void JellyCreature::update(float mx, float my) {
    if (currentDome != nullptr) {
       // float localMX = ofGetMouseX() - x;
       // float localMY = ofGetMouseY() - y;
        float localMX = mx - x;
        float localMY = my - y;
        currentDome->update(localMX, localMY);

        currentDome->update(localMX, localMY); // La physique et mesh.setVertex() vont ici
    }
}

void JellyCreature::draw(float mx, float my) { // <--- Ajout des arguments
    ofPushMatrix();
    ofTranslate(x, y);
    
    if (bDrawBaseImage) {
        img.draw(0, 0);
    }
    
     if (isDrawingCircle) {
        ofNoFill();
        ofSetColor(255, 255, 0);
        ofSetLineWidth(2);
        
        // CORRECTION ICI : On utilise mx/my passés en paramètres
        // au lieu de ofGetMouseX()/Y qui sont à l'échelle de l'écran
        float localMX = mx - x;
        float localMY = my - y;
        
        // Le rayon sera maintenant calculé dans le bon référentiel (le FBO géant)
        float r = ofDist(localCircleX, localCircleY, localMX, localMY);
        
        ofDrawCircle(localCircleX, localCircleY, r);
        
        ofFill();
        ofSetColor(255);
        ofSetLineWidth(1);
    }

    if (currentDome != nullptr) {
        currentDome->display(); 
    }
    ofPopMatrix();
}

/*
void JellyCreature::run() {
    ofPushMatrix();
    ofTranslate(x, y);

    ofSetColor(255);
    img.draw(0, 0);

    if (isDrawingCircle) {
        ofNoFill();
        ofSetColor(255, 255, 0);
        ofSetLineWidth(2);
        
        float localMX = ofGetMouseX() - x;
        float localMY = ofGetMouseY() - y;
        float r = ofDist(localCircleX, localCircleY, localMX, localMY);
        
        ofDrawCircle(localCircleX, localCircleY, r);
        
        ofFill();
        ofSetColor(255);
        ofSetLineWidth(1);
    }

    if (currentDome != nullptr) {
        float localMX = ofGetMouseX() - x;
        float localMY = ofGetMouseY() - y;
        currentDome->update(localMX, localMY);
        currentDome->display();
    }

    ofPopMatrix();
}
*/