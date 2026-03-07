#include "FireCLayer.h"

//--------------------------------------------------------------
void FireCLayer::setup(float w, float h) {
    width = w;
    height = h;
    bx = w / 2;
    by = h / 2;
    frameCount = 0;
    noiseScrollY = 0;
    bPress = false;

    // 1. Allocation FBOs
    bufferFbo.allocate(width, height, GL_RGBA);
    bufferFbo.begin(); ofClear(0,0,0,0); bufferFbo.end();

    // Noise FBO (plus petit pour perf, le shader interpolera)
    noiseFbo.allocate(512, 512, GL_RGB);
    
    // Génération du bruit Perlin dans noiseFbo
    ofPixels noisePix;
    noisePix.allocate(512, 512, OF_PIXELS_RGB);
    float noiseScale = 0.02;
    for(int y=0; y<512; y++) {
        for(int x=0; x<512; x++) {
            // On met du bruit dans R et G pour le displacement X et Y
            float n1 = ofNoise(x * noiseScale, y * noiseScale);
            float n2 = ofNoise(x * noiseScale + 1000, y * noiseScale + 1000);
            noisePix.setColor(x, y, ofColor(n1 * 255, n2 * 255, 0));
        }
    }
    noiseFbo.getTexture().loadData(noisePix);

    // 2. Création du Sprite "Fire" (Gradient Radial)
    // AS3: [0xffffff,0xffff00,0xff8000,0x802000,0x402000]
    int size = 128;
    particleImg.allocate(size, size, OF_IMAGE_COLOR_ALPHA);
    ofPixels & pix = particleImg.getPixels();
    ofVec2f center(size/2, size/2);
    float maxDist = size/2;

    for(int y=0; y<size; y++) {
        for(int x=0; x<size; x++) {
            float d = center.distance(ofVec2f(x, y));
            float pct = ofClamp(d / maxDist, 0.0f, 1.0f);
            
            ofColor c;
            if(pct < 0.12) c.set(255, 255, 255);      // Blanc
            else if(pct < 0.27) c.set(255, 255, 0);   // Jaune
            else if(pct < 0.35) c.set(255, 128, 0);   // Orange
            else if(pct < 0.45) c.set(128, 32, 0);    // Rouge sombre
            else c.set(64, 32, 0);                    // Marron
            
            // Alpha dégressif sur les bords
            if(pct > 0.4) c.a = ofMap(pct, 0.4, 1.0, 255, 0, true);
            else c.a = 255;
            
            pix.setColor(x, y, c);
        }
    }
    particleImg.update();

    // 3. Shader Displacement
    // Simule DisplacementMapFilter de Flash
    string fragShader = R"(
        #version 120
        uniform sampler2D tex0; // Buffer particules
        uniform sampler2D tex1; // Noise texture
        uniform vec2 scroll;    // Décalage du bruit
        uniform float strength; // Force du displacement

        void main() {
            vec2 uv = gl_TexCoord[0].st;
            
            // Coordonnées pour le bruit (tiled)
            vec2 noiseUV = uv * vec2(width/512.0, height/512.0) + scroll;
            vec4 noise = texture2D(tex1, mod(noiseUV, 1.0));
            
            // Displacement map: 0.5 est neutre. 
            // AS3 map: 0->-scale, 1->+scale. Donc (val - 0.5) * 2 * scale
            vec2 disp = (noise.rg - 0.5) * 2.0 * strength;
            
            // On inverse Y pour le displacement pour simuler la montée de chaleur
            vec2 targetUV = uv + disp * vec2(0.01, 0.03); 
            
            gl_FragColor = texture2D(tex0, targetUV);
        }
    )";
    
    // On injecte la largeur/hauteur dans le shader pour le tiling correct
    string header = "#version 120\n#define width " + ofToString(width) + ".0\n#define height " + ofToString(height) + ".0\n";
    fragShader.replace(fragShader.find("#version 120"), 12, header);

    displaceShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
    displaceShader.linkProgram();
}

//--------------------------------------------------------------
void FireCLayer::update(float mx, float my) {
    frameCount++;
    bx = mx;
    by = my;
    
    // --- 1. LOGIQUE D'EMISSION (AS3) ---
    if (bPress) {
        // Tir continu souris
        fire(bx, by, bx, by, 1, 0.25, 0);
        frameCount = 0;
    } else {
        // Auto-fire périodique
        if (frameCount % 40 == 0) {
            float cx = width / 2;
            float cy = height / 2;
            float len = 20 + ofRandom(4);
            
            fire(bx, by, cx, cy, len * 0.5, ofRandom(1.0) + 1.0, 0);
            fire(bx, by, cx, cy, len, 0.1, 6);
            fire(bx, by, cx, cy, len, 0.1, 12);
            fire(bx, by, cx, cy, len, 0.1, 18);
            fire(bx, by, cx, cy, len, 0.05, 28);
            fire(bx, by, cx, cy, len * 0.25, 0.1, 38);
        }
    }
    bPress = false; // Reset press state (sera set par mousePressed externe)

    // --- 2. DESSIN DANS LE BUFFER (FEEDBACK LOOP) ---
    bufferFbo.begin();
        // Fade out (ColorTransform 0.9, 0.75, 0.75)
        // On dessine un rect semi-transparent pour assombrir les traces précédentes
        ofEnableAlphaBlending();
        ofSetColor(0, 0, 0, 20); // Alpha faible pour garder la traînée
        ofDrawRectangle(0, 0, width, height);
        
        // Dessin des particules en mode ADD
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofSetColor(255);
        
        for (int i = particles.size() - 1; i >= 0; i--) {
            FireCParticle& p = particles[i];
            if (p.delay > 0) {
                p.delay--;
                continue;
            }
            
            float drawSize = p.sx * 128.0f; // Base size 128
            particleImg.draw(p.x - drawSize/2, p.y - drawSize/2, drawSize, drawSize);
            
            // Physique
            p.x += p.vx;
            p.y += p.vy;
            p.sx += p.growth;
            p.life--;
            
            if (p.life < 0) particles.erase(particles.begin() + i);
        }
        ofDisableBlendMode();
    bufferFbo.end();
    
    // Scroll du bruit (vers le haut)
    noiseScrollY -= 0.005;
}

//--------------------------------------------------------------
void FireCLayer::draw() {
    // Rendu final avec Shader de displacement
    ofSetColor(255);
    displaceShader.begin();
    displaceShader.setUniformTexture("tex1", noiseFbo.getTexture(), 1);
    displaceShader.setUniform2f("scroll", 0, noiseScrollY);
    displaceShader.setUniform1f("strength", 1.0); // Force du displacement
    
    bufferFbo.draw(0, 0);
    
    displaceShader.end();
}

//--------------------------------------------------------------
void FireCLayer::fire(float x, float y, float tx, float ty, float life, float scale, int delay) {
    float dx = tx - width/2; // Cible relative au centre (comme AS3)
    float dy = ty - height/2;
    float len = sqrt(dx*dx + dy*dy);
    if(len != 0) { dx = 30 * dx/len; dy = 30 * dy/len; }
    
    FireCParticle p;
    p.x = x + ofRandom(5);
    p.y = y + sin(frameCount * PI/4.0) * 5;
    p.sx = scale;
    p.growth = 0.04 + ofRandom(0.04);
    p.vx = dx; p.vy = dy;
    p.life = life;
    p.delay = delay;
    particles.push_back(p);
}

void FireCLayer::mousePressed(float x, float y) {
    bPress = true;
}