#include "PhysicSamLayer.h"
#include "RigidbodySam.h"
#include "GearsSam.h"
#include "SoftbodySam.h"
#include "AliveSam.h"

// --- PhysicSamLayer ---

void PhysicSamLayer::setup(float w, float h, float s, shared_ptr<ColliderLayer> col) {
    simWidth = w;
    simHeight = h;
    scale = s;
    collider = col;
    bActive = false;
}

void PhysicSamLayer::update(float mx, float my, float time) {
    if (!bActive) return;
    
    // 1. Intégration des forces et vitesses
    for (auto& body : bodies) {
        body->updateInteraction();
    }
    
    // 2. Solveur Box2D-like (Impulsions Séquentielles sur plusieurs itérations)
    int iterations = 8;
    for (int iter = 0; iter < iterations; iter++) {
        
        // --- Corps vs Environnement ---
        if (collider) {
            for (auto& A : bodies) {
                float snA = sin(A->angle);
                float csA = cos(A->angle);
                
                for (auto& nA : A->localNodes) {
                    float rx = nA.x * csA - nA.y * snA;
                    float ry = nA.x * snA + nA.y * csA;
                    ofVec2f wp(A->pos.x + rx, A->pos.y + ry);
                    
                    bool hitWall = false;
                    ofVec2f normal;
                    float pen = 0;

                    if (wp.y + A->nodeRadius > simHeight) {
                        hitWall = true;
                        normal.set(0, -1);
                        pen = (wp.y + A->nodeRadius) - simHeight;
                    } else if (collider->isWall(wp.x, wp.y)) {
                        hitWall = true;
                        float eps = 4.0f;
                        // Correction de la normale : elle doit pointer vers le vide (ext. du mur)
                        float nx = (collider->isWall(wp.x - eps, wp.y) ? 1 : 0) - (collider->isWall(wp.x + eps, wp.y) ? 1 : 0);
                        float ny = (collider->isWall(wp.x, wp.y - eps) ? 1 : 0) - (collider->isWall(wp.x, wp.y + eps) ? 1 : 0);
                        normal.set(nx, ny);
                        if (normal.lengthSquared() > 0.1f) normal.normalize();
                        else normal.set(0, -1); 
                        
                        // Calcul précis de la pénétration (Raycast local)
                        int searchDist = 0;
                        while(collider->isWall(wp.x + normal.x * searchDist, wp.y + normal.y * searchDist) && searchDist < 20) {
                            searchDist++;
                        }
                        pen = (float)searchDist; 
                    }

                    if (hitWall) {
                        // Si la masse n'est pas infinie (Rigidbody), on corrige la position
                        if (A->invMass > 0.0f) {
                            A->pos += normal * (pen * 0.25f); 
                        }

                        // Résolution de vélocité
                        ofVec2f r_perp(-ry, rx);
                        ofVec2f vPoint = A->vel + r_perp * A->angularVel;
                        
                        float vn = vPoint.dot(normal);
                        if (vn < 0) {
                            // Rebondit seulement si l'impact est assez rapide, sinon il se pose
                            float restitution = (iter == 0 && vn < -1.0f) ? 0.2f : 0.0f; 
                            float denom = A->invMass + pow(r_perp.dot(normal), 2) * A->invInertia;
                            float j = -(1.0f + restitution) * vn / denom;
                            ofVec2f impulse = normal * j;
                            
                            // Frottements (Coulomb Friction)
                            float friction = 0.2f;
                            ofVec2f tangent(-normal.y, normal.x);
                            float vt = vPoint.dot(tangent);
                            float denomT = A->invMass + pow(r_perp.dot(tangent), 2) * A->invInertia;
                            float jt = -vt / denomT;
                            jt = ofClamp(jt, -j * friction, j * friction);
                            impulse += tangent * jt;

                            A->vel += impulse * A->invMass;
                            A->angularVel += r_perp.dot(impulse) * A->invInertia;
                        }
                    }
                }
            }
        }

        // --- Corps vs Corps ---
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                auto& A = bodies[i];
                auto& B = bodies[j];
                
                // Broadphase ultra-rapide basé sur le Rayon Global des objets
                float minDist = A->maxRadius + B->maxRadius;
                if (A->pos.squareDistance(B->pos) > minDist * minDist) continue;
                
                float snA = sin(A->angle), csA = cos(A->angle);
                float snB = sin(B->angle), csB = cos(B->angle);

                for (auto& nA : A->localNodes) {
                    float rAx = nA.x * csA - nA.y * snA;
                    float rAy = nA.x * snA + nA.y * csA;
                    ofVec2f wpA(A->pos.x + rAx, A->pos.y + rAy);

                    for (auto& nB : B->localNodes) {
                        float rBx = nB.x * csB - nB.y * snB;
                        float rBy = nB.x * snB + nB.y * csB;
                        ofVec2f wpB(B->pos.x + rBx, B->pos.y + rBy);

                        ofVec2f diff = wpA - wpB;
                        float distSq = diff.lengthSquared();
                        float targetDist = A->nodeRadius + B->nodeRadius;

                        if (distSq < targetDist * targetDist && distSq > 0.0001f) {
                            float dist = sqrt(distSq);
                            ofVec2f normal = diff / dist; // B vers A
                            float pen = targetDist - dist;

                            // Correction de Position
                            float sumInvMass = A->invMass + B->invMass;
                            if (sumInvMass > 0.0f) {
                                float mRatioA = A->invMass / sumInvMass;
                                float mRatioB = B->invMass / sumInvMass;
                                A->pos += normal * (pen * 0.25f * mRatioA);
                                B->pos -= normal * (pen * 0.25f * mRatioB);
                            }

                            // Résolution de Vélocité
                            ofVec2f rA_perp(-rAy, rAx);
                            ofVec2f rB_perp(-rBy, rBx);
                            ofVec2f vA = A->vel + rA_perp * A->angularVel;
                            ofVec2f vB = B->vel + rB_perp * B->angularVel;
                            ofVec2f vRel = vA - vB;
                            
                            float vn = vRel.dot(normal);
                            if (vn < 0) {
                                float restitution = (iter == 0 && vn < -1.0f) ? 0.2f : 0.0f;
                                float friction = 0.3f;
                                
                                float denomN = A->invMass + B->invMass + 
                                               pow(rA_perp.dot(normal), 2) * A->invInertia + 
                                               pow(rB_perp.dot(normal), 2) * B->invInertia;
                                
                                float j = -(1.0f + restitution) * vn / denomN;
                                ofVec2f impulse = normal * j;

                                // Frottements
                                ofVec2f tangent(-normal.y, normal.x);
                                float vt = vRel.dot(tangent);
                                float denomT = A->invMass + B->invMass + 
                                               pow(rA_perp.dot(tangent), 2) * A->invInertia + 
                                               pow(rB_perp.dot(tangent), 2) * B->invInertia;
                                float jt = -vt / denomT;
                                jt = ofClamp(jt, -j * friction, j * friction);
                                impulse += tangent * jt;

                                A->vel += impulse * A->invMass;
                                A->angularVel += rA_perp.dot(impulse) * A->invInertia;
                                B->vel -= impulse * B->invMass;
                                B->angularVel -= rB_perp.dot(impulse) * B->invInertia;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // --- Wrapping Physique ---
    for (auto& body : bodies) {
        if (body->pos.x > simWidth) {
            body->wrap(-simWidth);
        } else if (body->pos.x < 0) {
            body->wrap(simWidth);
        }
    }
}

void PhysicSamLayer::draw() {
    if (!bActive) return;
    ofPushMatrix();
    ofScale(scale, scale);
    
    float wrapThreshold = 500.0f; // Largeur du seuil pour dessiner les fantômes
    
    // 1. Dessiner les trous dans le décor en premier
    for (auto& body : bodies) {
        vector<float> offsets = {0.0f};
        if (body->origPos.x < wrapThreshold) offsets.push_back(simWidth);
        if (body->origPos.x > simWidth - wrapThreshold) offsets.push_back(-simWidth);
        
        for (float ox : offsets) {
            ofPushMatrix();
            ofTranslate(ox, 0);
            body->drawHole(holeShadowAngle, holeShadowDistance);
            ofPopMatrix();
        }
    }
    
    // 2. Dessiner les objets physiques par-dessus
    for (auto& body : bodies) {
        vector<float> offsets = {0.0f};
        if (body->pos.x < wrapThreshold) offsets.push_back(simWidth);
        if (body->pos.x > simWidth - wrapThreshold) offsets.push_back(-simWidth);
        
        for (float ox : offsets) {
            ofPushMatrix();
            ofTranslate(ox, 0);
            body->draw();
            ofPopMatrix();
        }
    }
    ofPopMatrix();
}

void PhysicSamLayer::addBody(const ofImage& mask, const ofImage& sourceImg, float offsetY) {
    createBody(mask, sourceImg, offsetY, 0);
}

void PhysicSamLayer::addGear(const ofImage& mask, const ofImage& sourceImg, float offsetY) {
    createBody(mask, sourceImg, offsetY, 1);
}

void PhysicSamLayer::addSoftBody(const ofImage& mask, const ofImage& sourceImg, float offsetY) {
    createBody(mask, sourceImg, offsetY, 2);
}

void PhysicSamLayer::addAliveBody(const ofImage& mask, const ofImage& sourceImg, float offsetY) {
    createBody(mask, sourceImg, offsetY, 3);
}

void PhysicSamLayer::createBody(const ofImage& mask, const ofImage& sourceImg, float offsetY, int bodyType) {
    if (!mask.isAllocated() || !sourceImg.isAllocated()) return;

    ofxCvGrayscaleImage cvGray;
    cvGray.allocate(mask.getWidth(), mask.getHeight());
    cvGray.setFromPixels(mask.getPixels());

    ofxCvContourFinder contourFinder;
    contourFinder.findContours(cvGray, 100, (mask.getWidth() * mask.getHeight()) / 2, 1, false);

    if (contourFinder.nBlobs > 0) {
        // For simplicity, we take the largest blob
        auto& blob = contourFinder.blobs[0];
        
        shared_ptr<PhysicSamBase> newBody;
        if (bodyType == 1) newBody = make_shared<GearsSam>();
        else if (bodyType == 2) {
            auto sb = make_shared<SoftbodySam>();
            sb->collider = collider; // Injecte l'environnement pour la physique liquide
            newBody = sb;
        }
        else if (bodyType == 3) {
            auto alive = make_shared<AliveSam>();
            alive->collider = collider;
            newBody = alive;
        }
        else newBody = make_shared<RigidbodySam>();

        ofPolyline poly;
        poly.addVertices(blob.pts);
        poly.close();
        poly.simplify(1.0f); 
        poly = poly.getResampledBySpacing(8.0f); // Répartit uniformément les sommets pour de meilleures collisions

        newBody->setup(poly, blob.boundingRect, sourceImg, scale, offsetY);
        bodies.push_back(newBody);
    }
}

void PhysicSamLayer::clear() {
    bodies.clear();
}

void PhysicSamLayer::mousePressed(float x, float y, int button) {
    if (!bActive) return;
    ofVec2f m(x, y);
    for (int i = bodies.size() - 1; i >= 0; --i) {
        auto& body = bodies[i];
        ofPolyline worldShape = body->shape;
        worldShape.rotateDeg(ofRadToDeg(body->angle), ofVec3f(0,0,1));
        worldShape.translate(body->pos);
        if (worldShape.inside(m.x, m.y)) {
            draggedBody = body;
            draggedBody->bIsDragged = true;
            draggedBody->dragTarget = m;
            ofVec2f localM = m - body->pos;
            draggedBody->dragPointLocal = localM.getRotated(-ofRadToDeg(body->angle));
            break;
        }
    }
}

void PhysicSamLayer::mouseDragged(float x, float y, int button) {
    if (!bActive || !draggedBody) return;
    ofVec2f m(x, y);
    draggedBody->dragTarget = m;
}

void PhysicSamLayer::mouseReleased(float x, float y, int button) {
    if (!bActive || !draggedBody) return;
    draggedBody->bIsDragged = false;
    draggedBody = nullptr;
}