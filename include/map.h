#ifndef MAP_H
#define MAP_H

#include "../CosmaFW/include/cfw.h"

enum TILEPAINT {
    TILEPAINT_NONE,
    TILEPAINT_WALL,
    TILEPAINT_FULLFLOOR,
    TILEPAINT_HALFFLOOR,
    TILEPAINT_FULLCEILING,
    TILEPAINT_HALFCEILING
};

void TC_InitializeMap();
int TC_GetMapTextureID(int x, int y);
CFW_Texture* TC_GetMapTexture(int id);
CFW_Texture* TC_GetFloorTexture();
CFW_Texture* TC_GetCeilingTexture();
bool TC_IsTilePainted(float x, float y, enum TILEPAINT surface);
enum TILEPAINT TC_GetMapPaint(int x, int y);
int TC_GetMapAmbient(int x, int y);
int TC_GetMapCollision(int x, int y);
Vector2i* TC_GetMapSizePointer();
void TC_FreeMap();

#endif
