#include "../include/map.h"
#include <SDL2/SDL_render.h>

int textureMap[9][9] =
{
    {0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,1,1,1,0},
    {0,1,1,1,0,1,1,1,0},
    {0,1,1,1,0,1,1,1,0},
    {0,1,1,1,0,1,1,1,0},
    {0,1,1,1,0,1,1,1,0},
    {0,1,1,1,0,1,1,1,0},
    {0,1,1,1,0,1,1,1,0},
    {0,0,0,0,0,0,0,0,0}
};

int flagMap[9][9] =
{
    {0,0,0,0,0,0,0,0,0},
    {0,5,5,5,0,0,0,0,0},
    {0,5,5,5,0,0,0,0,0},
    {0,2,2,2,0,0,0,0,0},
    {0,2,2,2,0,0,0,0,0},
    {0,2,2,2,0,0,0,0,0},
    {0,5,5,5,0,0,0,0,0},
    {0,5,5,5,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0}
};

float* tileOffset;

// Currently loaded map size
Vector2i currentMapSize = {9, 9};

// Currently loaded map textures, populated on load
CFW_Texture** mapTextures;

CFW_Texture* floorTexture;
CFW_Texture* ceilingTexture;

void TC_ReverseMap() {
    for (int x = 0; x < currentMapSize.x/2; x++) {
        for (int y = 0; y < currentMapSize.y; y++) {
            int swapInt = textureMap[x][y];
            textureMap[x][y] = textureMap[currentMapSize.x-x-1][y];
            textureMap[currentMapSize.x-x-1][y] = swapInt;

            swapInt = flagMap[x][y];
            flagMap[x][y] = flagMap[currentMapSize.x-x-1][y];
            flagMap[currentMapSize.x-x-1][y] = swapInt;
        }
    }
}

void TC_InitializeMap() {
    TC_ReverseMap();
    mapTextures = malloc(sizeof(void*)*2);
    mapTextures[0] = CFW_CreateTexture("assets/textures/wall1.png");
    mapTextures[1] = CFW_CreateTexture("assets/textures/water.png");

    for (int x = 0; x < 2; x++) {
        CFW_ReqTexture(mapTextures[x]);
        SDL_SetTextureScaleMode(mapTextures[x]->texture, SDL_ScaleModeNearest);
    }

    floorTexture = CFW_CreateTexture("assets/textures/floor.png");
    CFW_ReqTexture(floorTexture);
    ceilingTexture = CFW_CreateTexture("assets/textures/ceiling.png");
    CFW_ReqTexture(ceilingTexture);
}

int TC_GetMapTextureID(int x, int y) {
    if (x >= 0 && y >= 0 && x < currentMapSize.x && y < currentMapSize.y)
        return textureMap[x][y];
    else
        return 0;
}
CFW_Texture* TC_GetMapTexture(int id) {
    return mapTextures[id];
}
CFW_Texture* TC_GetFloorTexture() {
    return floorTexture;
}
CFW_Texture* TC_GetCeilingTexture() {
    return ceilingTexture;
}

int TC_GetMapFlags(int x, int y) {
    if (x >= 0 && y >= 0 && x < currentMapSize.x && y < currentMapSize.y)
        return flagMap[x][y];
    else
        return 0;
}

int TC_GetMapCollision(int x, int y) {
    if (x >= 0 && y >= 0 && x < currentMapSize.x && y < currentMapSize.y && (flagMap[x][y] & TILEFLAG_COLLIDE) > 0)
        return 1;
    else
        return 0;
}
Vector2i* TC_GetMapSizePointer() {
    return &currentMapSize;
}

void TC_FreeMap() {
    for (int x = 0; x < 2; x++) {
        CFW_DestroyTexture(mapTextures[x], true);
    }
    CFW_DestroyTexture(floorTexture, true);
    CFW_DestroyTexture(ceilingTexture, true);
    free(mapTextures);
}
