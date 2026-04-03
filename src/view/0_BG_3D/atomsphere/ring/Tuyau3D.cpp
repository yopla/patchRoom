#include "Tuyau3D.h"

void Tuyau3D::setup() {
    // --- GENERATION DES TEXTURES STATIQUES (Exécuté une seule fois !) ---
    generateTexture();
    
    // Dimensions proportionnelles à la Room (Room + 200 de marge)
    objBox.set(2600, 1672, 2824);
    objBox.setResolution(2);
    objBox.mapTexCoords(0, 0, 1, 1);
    generateObjTextures();

    // --- SHADER POUR UNE DECOUPE PIXEL PERFECT ---
    string vert, frag;
    if(ofIsGLProgrammableRenderer()){
        vert = R"(
            #version 150
            uniform mat4 modelViewProjectionMatrix;
            in vec4 position;
            in vec4 color;
            in vec2 texcoord;
            out vec4 vColor;
            out vec2 vTexCoord;
            void main() { vColor = color; vTexCoord = texcoord; gl_Position = modelViewProjectionMatrix * position; }
        )";
        
        //   ou  vec3 bandColor = vec3(1.0, 1.0, 1.0); // Bande blanche éclatante (soudure)

        frag = R"(
            #version 150
            uniform sampler2D tex0;
            uniform int uDrawCouture;
            in vec4 vColor;
            in vec2 vTexCoord;
            out vec4 fragColor;
            void main() {
                // On décale la coupe à 0.45 pour rajouter de la matière (chevauchement physique)
                if (vColor.a < 0.45) discard; 
                vec4 texColor = texture(tex0, vTexCoord);
                if (uDrawCouture == 1) {
                    float distFromCut = abs(vColor.a - 0.5) * 1000.0;
                    float bandWidth = 120.0;
                    float bandAlpha = 1.0 - smoothstep(0.0, bandWidth, distFromCut);
                    vec3 baseColor = texColor.rgb * vColor.rgb;
                    vec3 bandColor = texColor.rgb * 1.2;
                    fragColor = vec4(mix(baseColor, bandColor, bandAlpha), 1.0);
                } else {
                    fragColor = vec4(texColor.rgb * vColor.rgb, 1.0);
                }
            }
        )";
    } else {
        vert = R"(
            #version 120
            varying vec4 vColor; varying vec2 vTexCoord;
            void main() { vColor = gl_Color; vTexCoord = gl_MultiTexCoord0.xy; gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; }
        )";
        frag = R"(
            #version 120
            uniform sampler2D tex0;
            uniform int uDrawCouture;
            varying vec4 vColor; varying vec2 vTexCoord;
            void main() { 
                if (vColor.a < 0.45) discard; 
                vec4 texColor = texture2D(tex0, vTexCoord); 
                if (uDrawCouture == 1) {
                    float distFromCut = abs(vColor.a - 0.5) * 1000.0;
                    float bandWidth = 120.0;
                    float bandAlpha = 1.0 - smoothstep(0.0, bandWidth, distFromCut);
                    vec3 baseColor = texColor.rgb * vColor.rgb;
                    vec3 bandColor = texColor.rgb * 1.2;
                    gl_FragColor = vec4(mix(baseColor, bandColor, bandAlpha), 1.0); 
                } else {
                    gl_FragColor = vec4(texColor.rgb * vColor.rgb, 1.0); 
                }
            }
        )";
    }
    bShaderLoaded = clipShader.setupShaderFromSource(GL_VERTEX_SHADER, vert) &&
                    clipShader.setupShaderFromSource(GL_FRAGMENT_SHADER, frag) &&
                    clipShader.linkProgram();

    buildMesh();
    reset();
}

void Tuyau3D::go() { bIsPlaying = true; }
void Tuyau3D::pause() { bIsPlaying = false; }

void Tuyau3D::setDrawArcs(bool draw) {
    if (bDrawArcs == draw) return;
    bDrawArcs = draw;
    buildMesh();
    if (!bDrawArcs && currentPath > 0) {
        reset(); // On réinitialise la position UNIQUEMENT si l'on était sur un arc qui vient de disparaître
    } else {
        generateTuyauObjs(); // On met juste à jour les objets attachés pour enlever ceux des arcs
    }
}

void Tuyau3D::reset() { 
    currentJunction = 3;
    targetJunction = 0;
    currentPath = 0;
    currentT = 0.0f; // On se positionne au début du segment pour éviter un saut au prochain 'nextCarrefour'
    phase = 0.0f;
    bIsPlaying = false;
    bPauseAtJunction = true;
    selectedPathOption = 0;
    rotX = 0.0f; rotY = 0.0f; rotZ = 0.0f;
    
    wpMainTargetJunc = (int)ofRandom(0, 4);
    wpMainT = ofRandom(0.3f, 0.7f); // Toujours au milieu du segment, jamais au bout
    
    if (bDrawArcs) {
        wpArcPath = (ofRandom(1.0f) > 0.5f) ? 1 : 2;
        wpArcT = ofRandom(0.3f, 0.7f);
    } else {
        wpArcPath = -1;
        wpArcT = -1.0f;
    }
    bAtWaypoint = false;
    
    state = 2; // MOVING (paused)
    TransformState ts = evaluateTransform(currentPath, targetJunction, currentT);
    currentP = ts.P;
    currentMatrix = ts.M;
    phase = ts.phase;
    
    generateTuyauObjs();
}

void Tuyau3D::nextCarrefour() {
    if (bAtWaypoint) {
        bAtWaypoint = false;
        state = 2; // MOVING
        bIsPlaying = true;
        selectedPathOption = 0;
        return;
    }

    if (currentJunction != -1) {
        int nextPath = 0;
        int nextJunc = 0;
        if (currentJunction == 0) {
            if (bDrawArcs && selectedPathOption == 1) { nextPath = 1; nextJunc = 1; }
            else { nextPath = 0; nextJunc = 1; }
        } else if (currentJunction == 1) {
            nextPath = 0; nextJunc = 2;
        } else if (currentJunction == 2) {
            if (bDrawArcs && selectedPathOption == 1) { nextPath = 2; nextJunc = 3; }
            else { nextPath = 0; nextJunc = 3; }
        } else if (currentJunction == 3) {
            nextPath = 0; nextJunc = 0;
        }
        
        int oldPath = currentPath;
        int startJunc = currentJunction;
        
        currentPath = nextPath;
        targetJunction = nextJunc;
        currentJunction = -1;
        currentT = 0.0f;
        selectedPathOption = 0;
        
        if (oldPath != currentPath) {
            state = 1; // TRANS_START
            transProgress = 0.0f;
            startTransState = evaluateTransform(oldPath, startJunc, 1.0f);
            endTransState = evaluateTransform(currentPath, targetJunction, 0.0f);
        } else {
            state = 2; // MOVING
        }
        bIsPlaying = true;
    } else {
        if (state == 0) state = 2;
        bIsPlaying = true;
    }
}

vector<string> Tuyau3D::getAvailablePaths() {
    vector<string> paths;
    if (bAtWaypoint) {
        paths.push_back("Continuer (Etape)");
        return paths;
    }
    if (currentJunction == 0) {
        paths.push_back("Tuyau Principal");
        if (bDrawArcs) paths.push_back("Arc 1 (Contournement)");
    } else if (currentJunction == 1) {
        paths.push_back("Tuyau Principal");
    } else if (currentJunction == 2) {
        paths.push_back("Tuyau Principal");
        if (bDrawArcs) paths.push_back("Arc 2 (Contournement)");
    } else if (currentJunction == 3) {
        paths.push_back("Tuyau Principal");
    } else {
        paths.push_back("En transit...");
    }
    return paths;
}

void Tuyau3D::selectPath(int index) {
    selectedPathOption = index;
}

float Tuyau3D::getPhase(int path, int targetJunc, float t) {
    int startJunc = (targetJunc + 3) % 4;
    
    float startPhase = 0, endPhase = 0;
    if (startJunc == 0) startPhase = 0.15f;
    if (startJunc == 1) startPhase = 0.35f;
    if (startJunc == 2) startPhase = 0.65f;
    if (startJunc == 3) startPhase = 0.85f;
    
    if (targetJunc == 0) endPhase = 1.15f;
    if (targetJunc == 1) endPhase = 0.35f;
    if (targetJunc == 2) endPhase = 0.65f;
    if (targetJunc == 3) endPhase = 0.85f;
    
    float p = 0.0f;
    if (path == 0) {
        p = ofLerp(startPhase, endPhase, t);
    } else {
        float arc_start_t = (targetJunc == 1) ? 0.15f : 0.65f;
        float arc_end_t   = (targetJunc == 1) ? 0.35f : 0.85f;
        p = ofLerp(arc_start_t, arc_end_t, t);
    }
    return p;
}

void Tuyau3D::generateObjTextures() {
    ofPixels bwPix, colPix;
    int texW = 1024, texH = 1024;
    bwPix.allocate(texW, texH, OF_PIXELS_RGBA);
    colPix.allocate(texW, texH, OF_PIXELS_RGBA);
    
    int cols = 24;
    int rows = 28;
    float cellW = (float)texW / cols;
    float cellH = (float)texH / rows;
    
    ofColor cellColors[24][28];
    for(int c=0; c<24; c++) {
        for(int r=0; r<28; r++) {
            cellColors[c][r] = ofColor::fromHsb(ofRandom(255), 200, 255);
        }
    }
    
    for(int y=0; y<texH; y++) {
        for(int x=0; x<texW; x++) {
            int c = std::min((int)(x / cellW), 23);
            int r = std::min((int)(y / cellH), 27);
            bool isWhite = ((c + r) % 2 == 0);
            
            bwPix.setColor(x, y, isWhite ? ofColor(255, 255) : ofColor(0, 255));
            colPix.setColor(x, y, isWhite ? cellColors[c][r] : ofColor(0, 255));
        }
    }
    
    bool bWasArb = ofGetUsingArbTex();
    if(bWasArb) ofDisableArbTex();
    
    texObjBW.allocate(bwPix);
    texObjBW.setTextureWrap(GL_REPEAT, GL_REPEAT);
    texObjBW.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    texObjColor.allocate(colPix);
    texObjColor.setTextureWrap(GL_REPEAT, GL_REPEAT);
    texObjColor.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    if(bWasArb) ofEnableArbTex();
}

void Tuyau3D::generateTuyauObjs() {
    tuyauObjs.clear();
    
    auto getTransformAt = [&](int path, float t) {
        ofVec3f P, P_next;
        if (path > 0) {
            int arcIdx = (path == 1) ? 0 : 1;
            P = getArcPoint(arcIdx, t);
            if (t < 0.999f) P_next = getArcPoint(arcIdx, t + 0.001f);
            else P_next = P + (P - getArcPoint(arcIdx, t - 0.001f));
        } else {
            P = getPathPoint(t * TWO_PI);
            P_next = getPathPoint((t + 0.001f) * TWO_PI);
        }
        ofVec3f T = (P_next - P).getNormalized();
        ofVec3f upRef(0, 1, 0);
        ofVec3f right = upRef.getCrossed(T).getNormalized();
        if (right.lengthSquared() < 0.001f) right = ofVec3f(0,0,1).getCrossed(T).getNormalized();
        ofVec3f up = T.getCrossed(right).getNormalized();
        
        TuyauObj obj;
        obj.pos = P;
        obj.rot.set(T.x, T.y, T.z, 0, up.x, up.y, up.z, 0, -right.x, -right.y, -right.z, 0, 0, 0, 0, 1);
        return obj;
    };
    
    // 1. Carrefours (Couleur) - Aligné sur le tuyau
    if (bDrawArcs) {
        float juncT[4] = {0.15f, 0.35f, 0.65f, 0.85f};
        for(int i=0; i<4; i++) {
            TuyauObj obj = getTransformAt(0, juncT[i]);
            obj.bIsJunction = true;
            tuyauObjs.push_back(obj);
        }
    }
    
    // Points d'étape (Waypoints) ajoutés visuellement au réseau
    if (wpMainTargetJunc != -1) {
        float phaseMain = getPhase(0, wpMainTargetJunc, wpMainT);
        TuyauObj obj = getTransformAt(0, phaseMain);
        obj.bIsJunction = true; // Dessiné comme un vrai carrefour (en couleurs)
        tuyauObjs.push_back(obj);
    }
    if (bDrawArcs && wpArcPath != -1) {
        TuyauObj obj = getTransformAt(wpArcPath, wpArcT);
        obj.bIsJunction = true; 
        tuyauObjs.push_back(obj);
    }

    // 2. Boites aléatoires (Noir et Blanc) - Rotation aléatoire
    int boxesPlaced = 0;
    int totalAttempts = 0;
    
    // On s'assure que les boites aléatoires ne sont pas trop proches des autres boites 
    // (carrefours inclus) en comparant directement leur distance physique dans l'espace 3D.
    while (boxesPlaced < numExtraTuyauObjs && totalAttempts < 5000) {
        int path = bDrawArcs ? (int)ofRandom(0, 3) : 0; // 0=Main, 1=Arc1, 2=Arc2
        float t = ofRandom(0.0f, 1.0f);

        TuyauObj obj = getTransformAt(path, t);
        
        bool tooClose = false;
        for (auto& existingObj : tuyauObjs) {
            if (obj.pos.squareDistance(existingObj.pos) < 6000.0f * 6000.0f) { // Marge de 6000 unités 3D
                tooClose = true;
                break;
            }
        }
        
        totalAttempts++;
        
        if (!tooClose) {
            obj.bIsJunction = false;
            
            ofMatrix4x4 randRot;
            randRot.makeRotationMatrix(ofRandom(360), ofVec3f(1,0,0), ofRandom(360), ofVec3f(0,1,0), ofRandom(360), ofVec3f(0,0,1));
            obj.rot = randRot * obj.rot;
            
            tuyauObjs.push_back(obj);
            boxesPlaced++;
        }
    }
}

TransformState Tuyau3D::evaluateTransform(int path, int targetJunc, float t_path) {
    TransformState ts;
    float phase_val = getPhase(path, targetJunc, t_path);
    ts.phase = phase_val;
    
    ofVec3f P, P_next;
    if (path > 0) {
        int arcIdx = (path == 1) ? 0 : 1;
        P = getArcPoint(arcIdx, t_path);
        if (t_path < 0.999f) {
            P_next = getArcPoint(arcIdx, t_path + 0.001f);
        } else {
            P_next = P + (P - getArcPoint(arcIdx, t_path - 0.001f));
        }
    } else {
        float t = phase_val * TWO_PI;
        P = getPathPoint(t);
        P_next = getPathPoint(t + 0.001f);
    }
    
    ts.P = P;
    ofVec3f T = (P_next - P).getNormalized();
    ofVec3f upRef(0, 1, 0);
    ofVec3f right = upRef.getCrossed(T).getNormalized();
    if (right.lengthSquared() < 0.001f) right = ofVec3f(0,0,1).getCrossed(T).getNormalized();
    ofVec3f up = T.getCrossed(right).getNormalized();
    
    // On inverse "right" (-right) pour forcer le déterminant à +1 (Matrice de rotation valide)
    // Cela évite la corruption (NaN) lors de la conversion en ofQuaternion pour le slerp !
    ts.M.set(
        T.x, T.y, T.z, 0,
        up.x, up.y, up.z, 0,
        -right.x, -right.y, -right.z, 0,
        0, 0, 0, 1
    );
    
    return ts;
}

void Tuyau3D::update(float time) {
    if (bIsPlaying) {
        if (state == 1) { // TRANS_START
            transProgress += ofGetLastFrameTime() * 2.0f * speed; // Base : 0.5 sec -> 30 frames
            bool finished = false;
            if (transProgress >= 1.0f) {
                transProgress = 1.0f;
                finished = true;
            }
            
            currentP = startTransState.P.getInterpolated(endTransState.P, transProgress);
            ofQuaternion qStart, qEnd, qCurrent;
            qStart.set(startTransState.M);
            qEnd.set(endTransState.M);
            qCurrent.slerp(transProgress, qStart, qEnd);
            qCurrent.get(currentMatrix);
            
            float endPhase = endTransState.phase;
            while(endPhase - startTransState.phase > 0.5f) endPhase -= 1.0f;
            while(endPhase - startTransState.phase < -0.5f) endPhase += 1.0f;
            phase = ofLerp(startTransState.phase, endPhase, transProgress);
            
            if (finished) {
                state = 2; // MOVING
            }
        }
        else if (state == 3) { // TRANS_END
            transProgress += ofGetLastFrameTime() * 2.0f * speed;
            bool finished = false;
            if (transProgress >= 1.0f) {
                transProgress = 1.0f;
                finished = true;
            }
            
            currentP = startTransState.P.getInterpolated(endTransState.P, transProgress);
            ofQuaternion qStart, qEnd, qCurrent;
            qStart.set(startTransState.M);
            qEnd.set(endTransState.M);
            qCurrent.slerp(transProgress, qStart, qEnd);
            qCurrent.get(currentMatrix);
            
            float endPhase = endTransState.phase;
            while(endPhase - startTransState.phase > 0.5f) endPhase -= 1.0f;
            while(endPhase - startTransState.phase < -0.5f) endPhase += 1.0f;
            phase = ofLerp(startTransState.phase, endPhase, transProgress);
            
            if (finished) {
                currentPath = 0;
                currentT = 1.0f;
                currentJunction = targetJunction;
                if (bPauseAtJunction) {
                    bIsPlaying = false;
                    state = 0; // IDLE
                } else {
                    nextCarrefour();
                }
            }
        }
        else if (state == 2) { // MOVING
            float speedFactor = speed * 0.05f;
            float segmentLength = 1.0f;
            
            if (currentPath == 0) {
                if (targetJunction == 0) segmentLength = 0.30f;
                else if (targetJunction == 1) segmentLength = 0.20f;
                else if (targetJunction == 2) segmentLength = 0.30f;
                else if (targetJunction == 3) segmentLength = 0.20f;
            } else {
                segmentLength = 0.40f;
            }
            
            float oldT = currentT;
            currentT += (speedFactor / segmentLength) * ofGetLastFrameTime();
            
            bool hitWp = false;
            if (currentPath == 0 && targetJunction == wpMainTargetJunc && oldT < wpMainT && currentT >= wpMainT) {
                currentT = wpMainT;
                hitWp = true;
            } else if (currentPath == wpArcPath && oldT < wpArcT && currentT >= wpArcT) {
                currentT = wpArcT;
                hitWp = true;
            }

            bool finished = false;
            if (!hitWp && currentT >= 1.0f) {
                currentT = 1.0f;
                finished = true;
            }
            
            TransformState ts = evaluateTransform(currentPath, targetJunction, currentT);
            currentP = ts.P;
            currentMatrix = ts.M;
            phase = ts.phase;
            
            if (hitWp) {
                bAtWaypoint = true;
                if (bPauseAtJunction) {
                    bIsPlaying = false;
                    state = 0; // IDLE
                } else {
                    bAtWaypoint = false;
                }
            } else if (finished) {
                if (currentPath > 0) {
                    state = 3; // TRANS_END
                    transProgress = 0.0f;
                    startTransState = ts; // On réutilise l'état parfaitement calculé !
                    endTransState = evaluateTransform(0, targetJunction, 1.0f); 
                } else {
                    currentJunction = targetJunction;
                    if (bPauseAtJunction) {
                        bIsPlaying = false;
                        state = 0; // IDLE
                    } else {
                        nextCarrefour();
                    }
                }
            }
        }
    } else {
        if (state == 2 || state == 0) {
            TransformState ts = evaluateTransform(currentPath, targetJunction, currentT);
            currentP = ts.P;
            currentMatrix = ts.M;
            phase = ts.phase;
        }
    }
}

ofVec3f Tuyau3D::getPathPoint(float t) {
    // Le chemin : Un grand cercle ondulant dans les 3 dimensions
    // Le départ (t=0) passe exactement par le centre de la salle (0, 600, 0) pour qu'on soit "dedans"
    float R = 18000.0f; // Agrandissement du parcours principal
    float x = R * sin(t);
    float z = R * (1.0f - cos(t)); 
    
    // Ondulation en Y plus marquée pour le relief global
    float y = 600.0f + 4000.0f * (1.0f - cos(3.0f * t));
    
    return ofVec3f(x, y, z);
}

void Tuyau3D::generateTexture() {
    bool wasArb = ofGetUsingArbTex();
    ofDisableArbTex(); // Nécessaire pour les UVs normalisés (0.0 à 1.0)
    
    int w = 4096;
    int h = 1024;
    textureFbo.allocate(w, h, GL_RGB);
    // Important pour que le décalage de texture se répète infiniment
    textureFbo.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    
    textureFbo.begin();
    ofClear(0, 255);
    
    // 1. Dessin du fond en dégradé de Hue
    for(int x = 0; x < w; x++) {
        float pct = (float)x / w;
        float hue = fmod(pct * 255.0f * hueCycles, 255.0f);
        ofSetColor(ofColor::fromHsb(hue, 255, 255));
        ofDrawLine(x, 0, x, h);
    }
    
    // 2. Dessin des anneaux et du texte (%)
    for(int i = 0; i < numRings; i++) {
        float pct = (float)i / numRings;
        float x = pct * w;
        
        ofSetColor(255);
        ofDrawRectangle(x - 4, 0, 8, h); // Ligne blanche de l'anneau
        
        string text = ofToString((int)(pct * 100)) + "%";
        // On répète le texte autour de la section du tuyau
        for(int y = 100; y < h; y += 200) {
            ofPushMatrix();
            ofTranslate(x + 20, y);
            ofScale(6.0f, 6.0f); // On grossit le texte pour qu'il soit bien lisible de loin
            ofDrawBitmapStringHighlight(text, 0, 0, ofColor(0, 150), ofColor(255));
            ofPopMatrix();
        }
    }
    
    textureFbo.end();
    
    if(wasArb) ofEnableArbTex();
}

ofVec3f Tuyau3D::getArcPoint(int arcIdx, float s) {
    float t1 = (arcIdx == 0) ? TWO_PI * 0.15f : TWO_PI * 0.65f;
    float t2 = (arcIdx == 0) ? TWO_PI * 0.35f : TWO_PI * 0.85f;
    
    ofVec3f P0 = getPathPoint(t1);
    ofVec3f P3 = getPathPoint(t2);
    
    ofVec3f T0 = (getPathPoint(t1 + 0.05f) - P0).getNormalized();
    ofVec3f T3 = (getPathPoint(t2 + 0.05f) - P3).getNormalized();
    
    float L = 15000.0f;
    ofVec3f center(0, 600, 18000.0f); // Centre approximatif de la boucle principale
    ofVec3f out0 = (P0 - center).getNormalized();
    ofVec3f out3 = (P3 - center).getNormalized();
    
    ofVec3f P1, P2;
    if (arcIdx == 0) {
        P1 = P0 + T0 * L + out0 * 12000.0f + ofVec3f(0, 8000, 0);
        P2 = P3 - T3 * L + out3 * 12000.0f + ofVec3f(0, 8000, 0);
    } else {
        P1 = P0 + T0 * L - out0 * 12000.0f - ofVec3f(0, 6000, 0);
        P2 = P3 - T3 * L - out3 * 12000.0f - ofVec3f(0, 6000, 0);
    }
    
    float u = 1.0f - s;
    return (u*u*u)*P0 + 3*(u*u)*s*P1 + 3*u*(s*s)*P2 + (s*s*s)*P3;
}

void Tuyau3D::buildMesh() {
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int tubeRes = 100;  // Resolution rehaussée
    ofVec3f lightDir = ofVec3f(0.5f, 0.8f, 0.3f).getNormalized(); // Lumière globale directionnelle simulée

    // --- Pré-calcul des squelettes pour la soustraction du volume interne ---
    vector<ofVec3f> skelMain, skelArc0, skelArc1;
    int skelRes = 1000;
    for(int i=0; i<=skelRes; i++) {
        float t = (float)i / skelRes;
        skelMain.push_back(getPathPoint(t * TWO_PI));
        if (bDrawArcs) {
            skelArc0.push_back(getArcPoint(0, t));
            skelArc1.push_back(getArcPoint(1, t));
        }
    }

    // Trouve le point EXACT sur la ligne du squelette (pas seulement le sommet le plus proche)
    auto getClosestSkelPoint = [](const ofVec3f& p, const vector<ofVec3f>& skel) {
        float minDistSq = 1e12f; // Initialisation très large
        ofVec3f closestP = p;
        for (size_t i = 0; i < skel.size() - 1; i++) {
            ofVec3f v = skel[i+1] - skel[i];
            ofVec3f w = p - skel[i];
            float c1 = w.dot(v);
            if (c1 <= 0) {
                float dSq = p.squareDistance(skel[i]);
                if (dSq < minDistSq) { minDistSq = dSq; closestP = skel[i]; }
                continue;
            }
            float c2 = v.dot(v);
            if (c2 <= c1) {
                float dSq = p.squareDistance(skel[i+1]);
                if (dSq < minDistSq) { minDistSq = dSq; closestP = skel[i+1]; }
                continue;
            }
            float b = c1 / c2;
            ofVec3f proj = skel[i] + v * b;
            float dSq = p.squareDistance(proj);
            if (dSq < minDistSq) { minDistSq = dSq; closestP = proj; }
        }
        return closestP;
    };

    auto addTube = [&](std::function<ofVec3f(float)> pathFunc, int pRes, float r, float texUStart, float texUEnd, int tubeType) {
        int vOffset = mesh.getNumVertices();
        vector<ofVec3f> pts, norms, binorms;
        vector<bool> vertInside; // Culling CPU des zones 100% cachées
        
        for(int i=0; i<=pRes; i++) {
            float s = (float)i / pRes;
            pts.push_back(pathFunc(s));
        }
        
        ofVec3f up(0, 1, 0);
        for(int i=0; i<=pRes; i++) {
            ofVec3f t_dir;
            if (i < pRes) t_dir = (pts[i+1] - pts[i]).getNormalized();
            else t_dir = (pts[i] - pts[i-1]).getNormalized();
            
            ofVec3f n_dir = t_dir.getCrossed(up);
            if(n_dir.lengthSquared() < 0.001f) n_dir = t_dir.getCrossed(ofVec3f(1,0,0));
            n_dir.normalize();
            
            ofVec3f b_dir = t_dir.getCrossed(n_dir).getNormalized();
            
            norms.push_back(n_dir);
            binorms.push_back(b_dir);
        }
        
        for(int i=0; i<=pRes; i++) {
            float pctPath = texUStart + (texUEnd - texUStart) * ((float)i / pRes);
            float s = (float)i / pRes;
            for(int j=0; j<=tubeRes; j++) {
                float pctTube = (float)j / tubeRes;
                float angle = pctTube * TWO_PI;
                
                ofVec3f surfaceNormal = (norms[i] * cos(angle) + binorms[i] * sin(angle)).getNormalized();
                ofVec3f vpos = pts[i] + surfaceNormal * r;
                
                ofVec3f closestP;
                float d = 1000.0f; // Distance > 0 = A l'exterieur
                
                // Si on se trouve dans une zone de croisement, on évalue la collision
                if (tubeType == 0 && bDrawArcs) {
                    if (s > 0.12f && s < 0.38f) {
                        closestP = getClosestSkelPoint(vpos, skelArc0);
                        d = vpos.distance(closestP) - r;
                    } else if (s > 0.62f && s < 0.88f) {
                        closestP = getClosestSkelPoint(vpos, skelArc1);
                        d = vpos.distance(closestP) - r;
                    }
                } else if (tubeType == 1 || tubeType == 2) {
                    if (s < 0.20f || s > 0.80f) {
                        closestP = getClosestSkelPoint(vpos, skelMain);
                        d = vpos.distance(closestP) - r;
                    }
                }
                
                // On encode la distance "d" dans la couche Alpha.
                // L'utilisation d'une grande plage (1000.0) et de ofFloatColor (32-bit)
                // assure une interpolation sub-pixel parfaite au moment du rendu par le GPU !
                float alpha = ofClamp(0.5f + d / 1000.0f, 0.0f, 1.0f);
                vertInside.push_back(alpha < 0.01f);
                
                float diffuse = max(0.2f, abs(surfaceNormal.dot(lightDir)));
                ofFloatColor shadedCol(diffuse, diffuse, diffuse, alpha);
                
                mesh.addVertex(vpos);
                mesh.addTexCoord(ofVec2f(pctPath, pctTube));
                mesh.addColor(shadedCol);
            }
        }
        
        for(int i=0; i<pRes; i++) {
            for(int j=0; j<tubeRes; j++) {
                int i1 = i * (tubeRes + 1) + j;
                int i2 = i1 + 1;
                int i3 = (i + 1) * (tubeRes + 1) + j;
                int i4 = i3 + 1;

                // Culling d'optimisation CPU : Si la face est à 100% à l'intérieur de l'autre tube, on l'efface.
                // Les faces à cheval sur la frontière sont envoyées au GPU qui coupera le pixel au scalpel.
                if (vertInside[i1] && vertInside[i2] && vertInside[i3] && vertInside[i4]) continue;

                int g1 = vOffset + i1;
                int g2 = vOffset + i2;
                int g3 = vOffset + i3;
                int g4 = vOffset + i4;

                mesh.addIndex(g1); mesh.addIndex(g2); mesh.addIndex(g3);
                mesh.addIndex(g2); mesh.addIndex(g4); mesh.addIndex(g3);
            }
        }
    };

    // Tuyau principal
    addTube([&](float s) {
        return getPathPoint(s * TWO_PI);
    }, 1000, tubeRadius, 0.0f, 1.0f, 0);
    
    if (bDrawArcs) {
        // Arc 1
        addTube([&](float s) {
            return getArcPoint(0, s);
        }, 300, tubeRadius, 0.15f, 0.35f, 1);
        
        // Arc 2
        addTube([&](float s) {
            return getArcPoint(1, s);
        }, 300, tubeRadius, 0.65f, 0.85f, 2);
    }
}

void Tuyau3D::draw() {
    ofPushStyle();
    ofEnableDepthTest();
    glDisable(GL_CULL_FACE); // Force l'affichage de la texture intérieure
    
    ofPushMatrix();
    ofTranslate(0, 600, 0); // Point de pivot au centre de la salle
    
    // Rotation fixée par les sliders
    ofRotateXDeg(rotX);
    ofRotateYDeg(rotY);
    ofRotateZDeg(rotZ);
    
    ofMultMatrix(currentMatrix.getInverse());
    ofTranslate(-currentP);
    
    textureFbo.getTexture().bind();
    if (bShaderLoaded) {
        clipShader.begin();
        clipShader.setUniformTexture("tex0", textureFbo.getTexture(), 0);
        clipShader.setUniform1i("uDrawCouture", bDrawCouture ? 1 : 0);
    }
    
    mesh.draw();
    
    if (bShaderLoaded) clipShader.end();
    textureFbo.getTexture().unbind();
    
    if (bDrawTuyauObj) {
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        glDepthMask(GL_FALSE); // Pas d'occlusion entre les boites transparentes
        
        for(auto& obj : tuyauObjs) {
            ofPushMatrix();
            ofTranslate(obj.pos);
            ofMultMatrix(obj.rot);
            
            if (obj.bIsJunction) texObjColor.bind();
            else texObjBW.bind();
            
            glEnable(GL_CULL_FACE);
            
            // Interieur (60% opacite)
            glCullFace(GL_FRONT);
            ofSetColor(255, 255, 255, 255 * 0.6f);
            objBox.draw();
            
            // Exterieur (20% opacite)
            glCullFace(GL_BACK);
            ofSetColor(255, 255, 255, 255 * 0.2f);
            objBox.draw();
            
            glDisable(GL_CULL_FACE);
            
            if (obj.bIsJunction) texObjColor.unbind();
            else texObjBW.unbind();
            
            ofPopMatrix();
        }
        
        glDepthMask(GL_TRUE);
        ofPopStyle();
    }
    ofPopMatrix();
    
    ofDisableDepthTest();
    ofPopStyle();
}