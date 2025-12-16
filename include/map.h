#ifndef MAP_H
#define MAP_H

#include "../CosmaFW/include/cfw.h"

void TC_InitializeMap();
void TC_ReverseMap();
int TC_GetMapTile(int x, int y);
SDL_Surface* TC_GetMapTexture(int id);
Vector2i* TC_GetMapSizePointer();
void TC_FreeMap();

#endif