#include "../include/map.h"

int currentMap[19][19] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,3,3,3,3,3,3,3,3,0,4,4,4,4,4,4,0,1},
    {1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,1},
    {1,0,3,0,1,1,0,4,4,4,4,0,3,3,3,0,3,0,1},
    {1,0,3,0,1,0,0,0,0,0,4,0,0,0,3,0,3,0,1},
    {1,0,3,3,1,0,2,1,1,1,1,1,1,0,3,0,0,0,1},
    {1,0,0,0,1,0,2,0,0,0,0,0,1,0,3,0,2,0,1},
    {1,0,2,0,1,0,2,1,1,0,4,0,1,0,3,3,2,0,1},
    {1,2,2,0,1,0,2,0,1,0,4,4,1,0,0,0,2,0,1},
    {1,0,0,0,1,0,0,0,1,0,4,0,3,2,2,2,2,0,1},
    {1,0,3,0,1,3,3,0,0,0,0,0,3,0,3,0,2,0,1},
    {1,0,3,0,1,0,3,3,3,3,3,3,3,0,3,0,0,0,1},
    {1,0,3,0,1,0,0,0,0,0,0,0,0,0,3,0,2,0,1},
    {1,0,3,0,1,1,1,0,4,4,4,0,3,3,3,0,2,0,1},
    {1,0,3,0,0,0,0,0,4,0,4,0,0,0,0,0,2,0,1},
    {1,0,3,2,2,2,2,2,2,0,4,4,4,0,2,2,2,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,2,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,5,1,1,1,1,1,1,1}
};
Vector2i currentMapSize = {19, 19};

SDL_Surface* mapTextures[5];

void TC_InitializeMap() {
    TC_ReverseMap();
    mapTextures[0] = IMG_Load("assets/textures/1.png");
    mapTextures[1] = IMG_Load("assets/textures/2.png");
    mapTextures[2] = IMG_Load("assets/textures/3.png");
    mapTextures[3] = IMG_Load("assets/textures/4.png");
    mapTextures[4] = IMG_Load("assets/textures/5.png");
}

void TC_ReverseMap() {
    for (int x = 0; x < currentMapSize.x/2; x++) {
        for (int y = 0; y < currentMapSize.y; y++) {
            int swapInt = currentMap[x][y];
            currentMap[x][y] = currentMap[currentMapSize.x-x-1][y];
            currentMap[currentMapSize.x-x-1][y] = swapInt;
        }
    }
}

int TC_GetMapTile(int x, int y) {
    return currentMap[x][y];
}
SDL_Surface* TC_GetMapTexture(int id) {
    return mapTextures[id-1];
}
Vector2i* TC_GetMapSizePointer() {
    return &currentMapSize;
}

void TC_FreeMap() {
    for (int x = 0; x < 5; x++) {
        SDL_FreeSurface(mapTextures[x]);
    }
}