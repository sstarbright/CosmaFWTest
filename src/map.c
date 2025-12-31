#include "../include/map.h"
#include <SDL2/SDL_render.h>

int textureMap[19][19] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,2,2,2,2,2,2,2,2,0,3,3,3,3,3,3,0,0},
    {0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0},
    {0,0,2,0,0,0,0,3,3,3,3,0,2,2,2,0,2,0,0},
    {0,0,2,0,0,0,0,0,0,0,3,0,0,0,2,0,2,0,0},
    {0,0,2,2,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,1,0,0},
    {0,0,1,0,0,0,1,0,0,0,3,0,0,0,2,2,1,0,0},
    {0,1,1,0,0,0,1,0,0,0,3,3,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,3,0,2,1,1,1,1,0,0},
    {0,0,2,0,0,2,2,0,0,0,0,0,2,0,2,0,1,0,0},
    {0,0,2,0,0,0,2,2,2,2,2,2,2,0,2,0,0,0,0},
    {0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,1,0,0},
    {0,0,2,0,0,0,0,0,3,3,3,0,2,2,2,0,1,0,0},
    {0,0,2,0,0,0,0,0,3,0,3,0,0,0,0,0,1,0,0},
    {0,0,2,1,1,1,1,1,1,0,3,3,3,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int collisionMap[19][19] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {1,0,1,0,1,1,0,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1,0,1},
    {1,1,1,0,1,0,1,0,1,0,1,1,1,0,0,0,1,0,1},
    {1,0,0,0,1,0,0,0,1,0,1,0,1,1,1,1,1,0,1},
    {1,0,1,0,1,1,1,0,0,0,0,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,0,0,1},
    {1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1}
};

int flagMap[19][19] =
{
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,0,2},
    {2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2},
    {2,0,2,0,2,2,0,2,2,2,2,0,2,2,2,0,2,0,2},
    {2,0,2,0,2,0,0,0,0,0,2,0,0,0,2,0,2,0,2},
    {2,0,2,2,2,0,2,2,2,2,2,2,2,0,2,0,0,0,2},
    {2,0,0,0,2,0,2,0,0,0,0,0,2,0,2,0,2,0,2},
    {2,0,2,0,2,0,2,2,2,0,2,0,2,0,2,2,2,0,2},
    {2,2,2,0,2,0,2,0,2,0,2,14,2,0,0,0,2,0,2},
    {2,0,0,0,2,0,0,0,2,0,2,0,2,2,2,2,2,0,2},
    {2,0,2,0,2,2,2,0,0,0,0,0,2,0,2,0,2,0,2},
    {2,0,2,0,2,0,2,2,2,2,2,2,2,0,2,0,0,0,2},
    {2,0,2,0,2,0,0,0,0,0,0,0,0,0,2,0,2,0,2},
    {2,0,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,0,2},
    {2,0,2,0,0,0,0,0,2,0,2,0,0,0,0,0,2,0,2},
    {2,0,2,2,2,2,2,2,2,0,2,2,2,0,2,2,2,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2}
};

float* tileOffset;

// Currently loaded map ambient occlusion, generated on map load
int* ambientMap;
// Currently loaded map size
Vector2i currentMapSize = {19, 19};

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

            swapInt = collisionMap[x][y];
            collisionMap[x][y] = collisionMap[currentMapSize.x-x-1][y];
            collisionMap[currentMapSize.x-x-1][y] = swapInt;

            swapInt = flagMap[x][y];
            flagMap[x][y] = flagMap[currentMapSize.x-x-1][y];
            flagMap[currentMapSize.x-x-1][y] = swapInt;
        }
    }
}

void TC_GenerateAmbient() {
    ambientMap = malloc(sizeof(int)*currentMapSize.x*currentMapSize.y);
    int xLimit = currentMapSize.x-1;
    int yLimit = currentMapSize.y-1;
    for (int x = 0; x < currentMapSize.x; x++) {
        for (int y = 0; y < currentMapSize.y; y++) {
            int corners = 0;
            int flagMapCurrent = TC_GetMapFlags(x, y);
            if (TC_CHECKIFPAINTWALL(flagMapCurrent)) {
                if (x > 0) {
                    flagMapCurrent = TC_GetMapFlags(x-1, y-1);
                    if (y > 0 && TC_CHECKIFPAINTWALL(flagMapCurrent)) {
                        corners |= 1;
                    }
                    flagMapCurrent = TC_GetMapFlags(x-1, y+1);
                    if (y < yLimit && TC_CHECKIFPAINTWALL(flagMapCurrent)) {
                        corners |= (1 << 1);
                    }
                }
                if (x < xLimit) {
                    flagMapCurrent = TC_GetMapFlags(x+1, y-1);
                    if (y > 0 && TC_CHECKIFPAINTWALL(flagMapCurrent)) {
                        corners |= (1 << 3);
                    }
                    flagMapCurrent = TC_GetMapFlags(x+1, y+1);
                    if (y < yLimit && TC_CHECKIFPAINTWALL(flagMapCurrent)) {
                        corners |= (1 << 2);
                    }
                }
                ambientMap[x+y * currentMapSize.x] = corners;
            }
        }
    }
}

void TC_InitializeMap() {
    TC_ReverseMap();
    TC_GenerateAmbient();
    mapTextures = malloc(sizeof(void*)*5);
    mapTextures[0] = CFW_CreateTexture("assets/textures/wall1.png");
    mapTextures[1] = CFW_CreateTexture("assets/textures/wall1.png");
    mapTextures[2] = CFW_CreateTexture("assets/textures/wall1.png");
    mapTextures[3] = CFW_CreateTexture("assets/textures/wall1.png");
    mapTextures[4] = CFW_CreateTexture("assets/textures/water.png");

    for (int x = 0; x < 5; x++) {
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

int TC_GetMapAmbient(int x, int y) {
    return ambientMap[x+y * currentMapSize.x];
}
int TC_GetMapCollision(int x, int y) {
    if (x >= 0 && y >= 0 && x < currentMapSize.x && y < currentMapSize.y)
        return collisionMap[x][y];
    else
        return 0;
}
Vector2i* TC_GetMapSizePointer() {
    return &currentMapSize;
}

void TC_FreeMap() {
    for (int x = 0; x < 5; x++) {
        CFW_DestroyTexture(mapTextures[x], true);
    }
    CFW_DestroyTexture(floorTexture, true);
    CFW_DestroyTexture(ceilingTexture, true);
    free(ambientMap);
    free(mapTextures);
}
