#include "BreakableCreature.h"

BreakableCreature::BreakableCreature(float x, float y, float w, float h, ofImage* img) {
    pos.set(x, y);
    width = w;
    height = h;
    texture = img;
    bBroken = false;
}

void BreakableCreature::update(float mx, float my) {
    if(bBroken) {
        for(auto& s : shards) {
            s.update();
        }
        // Remove shards that fall too far
        for(int i=shards.size()-1; i>=0; i--) {
            if(shards[i].pos.y > pos.y + 2000) { 
                shards.erase(shards.begin()+i);
            }
        }
    }
}

void BreakableCreature::draw() {
    ofSetColor(255);
    if(!bBroken) {
        if(texture && texture->isAllocated()) texture->draw(pos.x, pos.y, width, height);
        else ofDrawRectangle(pos.x, pos.y, width, height);
    } else {
        for(auto& s : shards) {
            s.draw(texture);
        }
    }
}

bool BreakableCreature::isInside(float mx, float my) {
    if(bBroken) return false;
    return (mx > pos.x && mx < pos.x + width && my > pos.y && my < pos.y + height);
}

void BreakableCreature::onPress(float mx, float my) {
    if(isInside(mx, my)) {
        breakApart(mx, my);
    }
}

void BreakableCreature::onRelease(float mx, float my) {}

void BreakableCreature::breakApart(float impactX, float impactY) {
    bBroken = true;
    
    // Create a grid of irregular pieces
    int cols = 5;
    int rows = 5;
    
    float cellW = width / cols;
    float cellH = height / rows;
    
    // Generate grid points with randomization to create heterogeneous shapes
    vector<vector<ofVec2f>> points;
    points.resize(cols + 1);
    for(int i=0; i<=cols; i++) {
        points[i].resize(rows + 1);
        for(int j=0; j<=rows; j++) {
            float x = i * cellW;
            float y = j * cellH;
            
            // Randomize internal points
            if(i > 0 && i < cols && j > 0 && j < rows) {
                x += ofRandom(-cellW*0.3, cellW*0.3);
                y += ofRandom(-cellH*0.3, cellH*0.3);
            }
            points[i][j] = ofVec2f(x, y);
        }
    }
    
    // Create shards from grid quads (split into 2 triangles each)
    for(int i=0; i<cols; i++) {
        for(int j=0; j<rows; j++) {
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            
            ofVec2f p1 = points[i][j];
            ofVec2f p2 = points[i+1][j];
            ofVec2f p3 = points[i+1][j+1];
            ofVec2f p4 = points[i][j+1];
            
            auto addTri = [&](ofVec2f a, ofVec2f b, ofVec2f c) {
                mesh.addVertex(ofVec3f(a.x, a.y, 0));
                mesh.addTexCoord(texture->getTexture().getCoordFromPercent(a.x/width, a.y/height));
                
                mesh.addVertex(ofVec3f(b.x, b.y, 0));
                mesh.addTexCoord(texture->getTexture().getCoordFromPercent(b.x/width, b.y/height));
                
                mesh.addVertex(ofVec3f(c.x, c.y, 0));
                mesh.addTexCoord(texture->getTexture().getCoordFromPercent(c.x/width, c.y/height));
            };
            
            addTri(p1, p2, p4);
            addTri(p2, p3, p4);
            
            // Calculate center of shard relative to creature
            ofVec2f center = (p1 + p2 + p3 + p4) / 4.0;
            ofVec2f globalCenter = pos + center;
            
            // Velocity based on explosion from impact
            ofVec2f dir = globalCenter - ofVec2f(impactX, impactY);
            float dist = dir.length();
            dir.normalize();
            
            float force = ofMap(dist, 0, width, 12, 4);
            ofVec2f vel = dir * force;
            vel.y -= ofRandom(8, 15); // Jump up
            vel.x += ofRandom(-3, 3);
            
            Shard s;
            s.setup(pos, vel, mesh); 
            shards.push_back(s);
        }
    }
}