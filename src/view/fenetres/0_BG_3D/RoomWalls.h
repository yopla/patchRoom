#pragma once
#include "ofMain.h"

class RoomWalls {
public:
    void setup();
    void draw(bool showRoof, float alpha, int mode = 0,
              ofFbo* fFront = nullptr, ofFbo* fBack = nullptr, ofFbo* fCour = nullptr, ofFbo* fJar = nullptr,
              ofFbo* fSol = nullptr, ofFbo* fTopCour = nullptr, ofFbo* fTopJar = nullptr);
    ofColor getPixelFromRay(const ofVec3f& origin, const ofVec3f& dir);
    ofColor getPixelFromRayDynamic(const ofVec3f& origin, const ofVec3f& dir, int mode,
                                   ofPixels* pFront, ofPixels* pBack, ofPixels* pCour, ofPixels* pJar,
                                   ofPixels* pSol, ofPixels* pTopCour, ofPixels* pTopJar);

    // Dimensions
    const float roomWidth = 2400.0f;
    const float roomDepth = 2624.0f;
    const float roomSolDepth = 2368.0f;
    const float heightFrontBack = 1472.0f;
    const float heightCour = 1072.0f;
    const float heightJar = 784.0f;
    const float heightTopCour = 1008.0f;
    const float heightTopJar = 1600.0f;

    // Points pour caméras
    vector<ofVec3f> pFront, pBack, pCour, pJar;
    ofVec3f pSolTL, pSolBL, pSolBR;
    ofVec3f pTopCourTL, pTopCourBL, pTopCourBR;
    ofVec3f pTopJarTL, pTopJarBL, pTopJarBR;

    // Meshes (Tous les murs sont maintenant des meshes)
    ofMesh meshFront, meshBack, meshJar, meshCour, meshSol;
    ofMesh meshTopCour, meshTopJar;

private:
    ofImage imgFront, imgBack, imgCour, imgJar, imgSol, imgTopCour, imgTopJar;
    float peakX, peakY;
    bool rayTriangleIntersect(const ofVec3f &orig, const ofVec3f &dir, const ofVec3f &v0, const ofVec3f &v1, const ofVec3f &v2, float &t, float &u, float &v);
};