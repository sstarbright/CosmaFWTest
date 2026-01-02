#ifndef RENDER_H
#define RENDER_H

#define IS_BETWEEN(value, low, high) (subCoord >= minPad && subCoord <= maxPad)

#define SETUP_PADDING() if (tileFlags & TILEFLAG_PADNORTH) {\
    if (tileFlags & TILEFLAG_PADSOUTH) {\
        northWallOffset = .25f;\
        southWallOffset = -.25f;\
    } else {\
        northWallOffset = .0f;\
        southWallOffset = -.5f;\
    }\
} else {\
    if (tileFlags & TILEFLAG_PADSOUTH) {\
        northWallOffset = .5f;\
        southWallOffset = .0f;\
    } else {\
        northWallOffset = .5f;\
        southWallOffset = -.5f;\
    }\
}\
if (tileFlags & TILEFLAG_PADWEST) {\
    if (tileFlags & TILEFLAG_PADEAST) {\
        westWallOffset = .25f;\
        eastWallOffset = -.25f;\
    } else {\
        westWallOffset = .0f;\
        eastWallOffset = -.5f;\
    }\
} else {\
    if (tileFlags & TILEFLAG_PADEAST) {\
        westWallOffset = .5f;\
        eastWallOffset = .0f;\
    } else {\
        westWallOffset = .5f;\
        eastWallOffset = -.5f;\
    }\
}

#define SETUP_X_WALL() wallLine.start.y = mapCoord.y + westWallOffset;\
wallLine.end.y = mapCoord.y + eastWallOffset;\
wallLine.start.x = mapCoord.x + (rayDir.x > 0 ? southWallOffset : northWallOffset);\
wallLine.end.x = wallLine.start.x

#define SETUP_Y_WALL() wallLine.start.y = mapCoord.y + (rayDir.y > 0 ? eastWallOffset : westWallOffset);\
wallLine.end.y = wallLine.start.y;\
wallLine.start.x = mapCoord.x + northWallOffset;\
wallLine.end.x = mapCoord.x + southWallOffset

#define SETUP_RAY_LINE() wallPosition -= floor(wallPosition);\
rayLine.start = (Vector2){.x = hitPosition.x, .y = hitPosition.y};\
rayLine.end = (Vector2){.x = hitPosition.x+rayNorm.x*5, .y = hitPosition.y+rayNorm.y*5}

#include "../CosmaFW/include/cfw.h"
#include "map.h"
#include "collide.h"

typedef struct RayCamera RayCamera;

struct RayCamera {
    Vector2 cameraPosition;
    Vector2 cameraDirection;
    Vector2 cameraPlane;
    float cameraAngle;
    float horizontalOffset;
    float verticalOffset;
};

enum TC_RENDERFLAGS {
    TC_RENDER_CHECKCOLLISION = 1
};

void TC_SetupRenderer(Vector2i* mapSizePointer, CFW_Window* targetWindow, SDL_Texture* renderTexture);
void TC_RenderFloorCeiling();
void TC_RenderWalls();
void TC_RenderViewport();
RayCamera* TC_GetCamera();
void TC_CloseRenderer();

#endif
