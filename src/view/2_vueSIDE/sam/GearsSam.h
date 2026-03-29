#pragma once
#include "ofMain.h"
#include "PhysicSamBase.h"

class GearsSam : public PhysicSamBase {
public:
    void setup(const ofPolyline& contour, const ofRectangle& bbox, const ofImage& sourceImg, float scale, float offsetY) override;
    void updateInteraction() override;
    void wrap(float shiftX) override;
    ofVec2f anchorPos;
};