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

#define invertFloat(floatToInvert) floatToInvert = 1.f - floatToInvert
#define clampFloat(floatToClamp, min, max) if (floatToClamp < min)\
    floatToClamp = min;\
else if (floatToClamp > max)\
    floatToClamp = max

#include "../CosmaFW/include/cfw.h"
#include "map.h"

typedef struct RayCamera RayCamera;

struct RayCamera {
    Vector2 cameraPosition;
    Vector2 cameraDirection;
    Vector2 cameraPlane;
};

void TC_SetupRenderer(Vector2i* mapSizePointer, SDL_Surface* targetSurface);
void TC_RenderGeo();
RayCamera* TC_GetCamera();
void TC_CloseRenderer();

#endif
