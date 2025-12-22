#include "../CosmaFW/include/cfw.h"
#include "../include/map.h"
#include "../include/render.h"
#include "../include/player.h"
#include "../include/joy.h"
#include "../include/collide.h"
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
    gamePlayer.radius = 0.25;

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

    Vector2 targetPosition = (Vector2){.x = playerX, .y = playerY};

    if(TC_KeyUp()) {
        targetPosition.x += turnX * moveSpeed;
        targetPosition.y += turnY * moveSpeed;
    }
    if (TC_KeyDown())
    {
        targetPosition.x -= turnX * moveSpeed;
        targetPosition.y -= turnY * moveSpeed;
    }

    if (!TC_CheckTilesWithinCircle(targetPosition, gamePlayer.radius)) {
        playerX = targetPosition.x;
        playerY = targetPosition.y;
    } else if (!TC_CheckTilesWithinCircle((Vector2){.x = targetPosition.x, .y = playerY}, gamePlayer.radius)) {
        playerX = targetPosition.x;
    } else if (!TC_CheckTilesWithinCircle((Vector2){.x = playerX, .y = targetPosition.y}, gamePlayer.radius)) {
        playerY = targetPosition.y;
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

    TC_RenderFloorCeiling();
    // Draw Walls
    TC_RenderWalls();

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
