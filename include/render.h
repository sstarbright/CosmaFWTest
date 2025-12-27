#ifndef RENDER_H
#define RENDER_H

#define applyAmbient(leftCorner, rightCorner)\
if (ambientData & leftCorner) {\
    wallClamp = flerp(AO_SHARPNESS, AO_BRIGHTNESS, wallX);\
    if (wallClamp > 1.0)\
        wallClamp = 1.0;\
    aoStrength *= wallClamp;\
}\
if (ambientData & rightCorner) {\
    wallClamp = flerp(AO_BRIGHTNESS, AO_SHARPNESS, wallX);\
    if (wallClamp > 1.0)\
        wallClamp = 1.0;\
    aoStrength *= wallClamp;\
}

#include "../CosmaFW/include/cfw.h"
#include "map.h"

typedef struct RayCamera RayCamera;

struct RayCamera {
    Vector2 cameraPosition;
    Vector2 cameraDirection;
    Vector2 cameraPlane;
    float cameraAngle;
    float horizontalOffset;
    float verticalOffset;
};

void TC_SetupRenderer(Vector2i* mapSizePointer, CFW_Window* targetWindow, SDL_Texture* renderTexture);
void TC_RenderFloorCeiling();
void TC_RenderWalls();
RayCamera* TC_GetCamera();
void TC_CloseRenderer();

#endif
