#ifndef RENDER_H
#define RENDER_H

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