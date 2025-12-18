#include "../CosmaFW/include/cfw.h"
#include "../include/map.h"
#include "../include/render.h"
#include "../include/player.h"
#include "../include/joy.h"
#include <SDL2/SDL_surface.h>

CFW_Window* gameWindow = NULL;
SDL_Surface* gameSurface = NULL;
Vector2i gameResolution = (Vector2i){.x = 256, .y = 224};
RayCamera* gameCamera = NULL;
PlayerData gamePlayer;

bool CFW_OnStart(int argumentCount, char* arguments[]) {
    gameWindow = CFW_CreateWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN);
    SDL_RaiseWindow(gameWindow->window);
    gameSurface = SDL_CreateRGBSurface(0, gameResolution.x, gameResolution.y, 12, 0, 0, 0, 0);
    if (!gameSurface) {
        printf("NO GAME SURFACE!!!\n");
    }
    TC_InitializeMap();
    TC_SetupRenderer(TC_GetMapSizePointer(), gameSurface);
    gameCamera = TC_GetCamera();
    if (!gameWindow)
        return false;

    gamePlayer.position = (Vector2){.x = 9.5f, .y = 9.5f};
    gamePlayer.direction = (Vector2){.x = -1.0f, .y = 0.0f};
    gamePlayer.wallRadius = 0.1;

    return true;
}

void TC_UpdateJoy(float deltaTime) {
    float playerX = gamePlayer.position.x;
    float playerY = gamePlayer.position.y;
    float turnX = gamePlayer.direction.x;
    float turnY = gamePlayer.direction.y;
    float planeX = gameCamera->cameraPlane.x;
    float planeY = gameCamera->cameraPlane.y;

    float moveSpeed = deltaTime * 5.0;
    float rotSpeed = deltaTime * 3.0;

    if(TC_KeyUp()) {
        if(TC_GetMapTile((int)(playerX + turnX * moveSpeed + gamePlayer.wallRadius * turnX),(int)(playerY)) == false) {
            playerX += turnX * moveSpeed;
        }
        if(TC_GetMapTile((int)(playerX),(int)(playerY + turnY * moveSpeed + gamePlayer.wallRadius * turnY)) == false) {
            playerY += turnY * moveSpeed;
        }
    }
    if (TC_KeyDown())
    {
        if(TC_GetMapTile((int)(playerX - turnX * moveSpeed - gamePlayer.wallRadius * turnX),(int)(playerY)) == false)
            playerX -= turnX * moveSpeed;
        if(TC_GetMapTile((int)(playerX),(int)(playerY - turnY * moveSpeed - gamePlayer.wallRadius * turnY)) == false)
            playerY -= turnY * moveSpeed;
    }
    if (TC_KeyRight())
    {
        float oldDirX = turnX;
        turnX = turnX * cos(-rotSpeed) - turnY * sin(-rotSpeed);
        turnY = oldDirX * sin(-rotSpeed) + turnY * cos(-rotSpeed);
        float oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    if (TC_KeyLeft())
    {
        float oldDirX = turnX;
        turnX = turnX * cos(rotSpeed) - turnY * sin(rotSpeed);
        turnY = oldDirX * sin(rotSpeed) + turnY * cos(rotSpeed);
        float oldPlaneX = planeX;
        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }

    gamePlayer.position.x = playerX;
    gamePlayer.position.y = playerY;
    gamePlayer.direction.x = turnX;
    gamePlayer.direction.y = turnY;
    gameCamera->cameraPosition.x = playerX;
    gameCamera->cameraPosition.y = playerY;
    gameCamera->cameraDirection.x = turnX;
    gameCamera->cameraDirection.y = turnY;
    gameCamera->cameraPlane.x = planeX;
    gameCamera->cameraPlane.y = planeY;
}

void CFW_OnUpdate(float deltaTime) {
    TC_UpdateJoy(deltaTime);

    // Draw Walls
    TC_RenderGeo();

    int windowWidth = 0;
    int windowHeight = 0;

    SDL_GetWindowSizeInPixels(gameWindow->window, &windowWidth, &windowHeight);

    // Update Window
    SDL_BlitScaled(gameSurface, NULL, gameWindow->surface, &(SDL_Rect){.x = 0, .y = 0, .w = windowWidth, .h = windowHeight});
    SDL_UpdateWindowSurface(gameWindow->window);
}

void CFW_OnEnd(int exitCode) {
    SDL_FreeSurface(gameSurface);
    TC_FreeMap();
    TC_CloseRenderer();
}
