#ifndef RENDER_H
#define RENDER_H

#define applyAmbient(leftCorner, rightCorner)\
if (ambientData & leftCorner) {\
    float wallLClamp = flerp(AO_SHARPNESS, AO_BRIGHTNESS, wallX);\
    if (wallLClamp > 1.0)\
        wallLClamp = 1.0;\
    aoStrength *= wallLClamp;\
}\
if (ambientData & rightCorner) {\
    float wallRClamp = flerp(AO_BRIGHTNESS, AO_SHARPNESS, wallX);\
    if (wallRClamp > 1.0)\
        wallRClamp = 1.0;\
    aoStrength *= wallRClamp;\
}

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
