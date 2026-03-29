#pragma once
#include "ofMain.h"
#include "PhysicSamBase.h"

class RigidbodySam : public PhysicSamBase {
public:
    void updateInteraction() override;
};