#ifndef MAP_H
#define MAP_H

#include "../CosmaFW/include/cfw.h"

void TC_InitializeMap();
void TC_ReverseMap();
void TC_GenerateAmbient();
int TC_GetMapTile(int x, int y);
SDL_Surface* TC_GetMapTexture(int id);
bool TC_CheckFaceAmbient(int x, int y, int corner);
Vector2i* TC_GetMapSizePointer();
void TC_FreeMap();

#endif