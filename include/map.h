#ifndef MAP_H
#define MAP_H

#include "../CosmaFW/include/cfw.h"

enum TILEFLAG {
    TILEFLAG_COLLIDE = 1,
    TILEFLAG_PAINTVERTICAL = 2,
    TILEFLAG_PAINTWALL = 4,
    TILEFLAG_PAINTCEILING = 4,
    TILEFLAG_MIRRORU = 8,
    TILEFLAG_MIRRORV = 16,
    TILEFLAG_SWAPUV = 32,
    TILEFLAG_PADNORTH = 64,
    TILEFLAG_PADSOUTH = 128,
    TILEFLAG_PADEAST = 256,
    TILEFLAG_PADWEST = 512
};

#define TC_CHECKIFPAINTWALL(tileFlags) ((tileFlags&TILEFLAG_PAINTVERTICAL) == 0 && (tileFlags&TILEFLAG_PAINTWALL) != 0)
#define TC_CHECKIFPAINTFLOOR(tileFlags) ((tileFlags&TILEFLAG_PAINTVERTICAL) != 0 && (tileFlags&TILEFLAG_PAINTCEILING) == 0)
#define TC_CHECKIFPAINTCEILING(tileFlags) ((tileFlags&TILEFLAG_PAINTVERTICAL) != 0 && (tileFlags&TILEFLAG_PAINTCEILING) != 0)

void TC_InitializeMap();
int TC_GetMapTextureID(int x, int y);
CFW_Texture* TC_GetMapTexture(int id);
CFW_Texture* TC_GetFloorTexture();
CFW_Texture* TC_GetCeilingTexture();
int TC_GetMapFlags(int x, int y);
int TC_GetMapCollision(int x, int y);
Vector2i* TC_GetMapSizePointer();
void TC_FreeMap();

#endif
