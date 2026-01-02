#ifndef MAP_H
#define MAP_H

#include "../CosmaFW/include/cfw.h"

enum TILEFLAG {
    TILEFLAG_PAINTVERTICAL = 1,
    TILEFLAG_PAINTWALL = 2,
    TILEFLAG_PAINTCEILING = 2,
    TILEFLAG_MIRRORU = 4,
    TILEFLAG_MIRRORV = 8,
    TILEFLAG_PADNORTH = 16,
    TILEFLAG_PADSOUTH = 32,
    TILEFLAG_PADEAST = 64,
    TILEFLAG_PADWEST = 128
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
