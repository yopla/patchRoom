#pragma once

#include "ofMain.h"

// Forward-declare all the classes it needs to interact with
// to avoid including all headers here.
class RoomApp;
class ofEasyCam;
class ProjectionSystem;
class FluidRing;
class RoomWalls;
class AtmosphereSystem;

class RoomInputHandler {
public:
    void setup(RoomApp* owner);
    void update();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void dragEvent(ofDragInfo dragInfo);

    ofVec3f cursor3DPos;
    ofVec3f lastCreatedHalo3DPos;

private:
    void updateKeyStates();
    void handleCameraAndProjection();
    void updateFluidRingInteraction();
    void updateLightFlyInteraction();

    RoomApp* app = nullptr; // Pointer to the main app to access its members

    // Key states
    bool bLeftShiftPressed = false;
    bool bRightShiftPressed = false;
    bool bSpacePressed = false;
    bool bTabPressed = false;
};