#include "../CosmaFW/include/cfw.h"
#include "../include/map.h"
#include "../include/render.h"
#include "../include/player.h"
#include "../include/joy.h"
#include "../include/collide.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <math.h>

CFW_Window* gameWindow = NULL;
SDL_Texture* gameTexture = NULL;
Vector2i gameResolution = (Vector2i){.x = 256, .y = 224};
RayCamera* gameCamera = NULL;
PlayerData gamePlayer;
float bobTime = 0.f;
float sideTime = 0.f;
float baseBobTime = 0.f;
float baseSideTime = 0.f;
bool movedThisFrame = false;

bool CFW_OnStart(int argumentCount, char* arguments[]) {
    gameWindow = CFW_CreateWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_RaiseWindow(gameWindow->window);
    if (!gameWindow->renderer) {
        printf("NO RENDERER FOUND!!!\n");
        return false;
    }
    if (SDL_RenderTargetSupported(gameWindow->renderer) != SDL_TRUE) {
        printf("CANNOT RENDER TO TARGET TEXTURE!!!\n");
        return false;
    }
    gameTexture = SDL_CreateTexture(gameWindow->renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, gameResolution.x, gameResolution.y);
    if (!gameTexture) {
        printf("NO GAME RENDER TARGET!!!\n");
        return false;
    }

    TC_InitializeMap();
    TC_SetupRenderer(TC_GetMapSizePointer(), gameWindow, gameTexture);
    gameCamera = TC_GetCamera();

    gamePlayer.position = (Vector2){.x = 9.5f, .y = 9.5f};
    gamePlayer.direction = (Vector2){.x = -1.0f, .y = 0.0f};
    gamePlayer.radius = 0.25;

    baseBobTime = asin(1.f);
    baseSideTime = asin(0.f);

    return true;
}

void TC_UpdateJoy(float deltaTime) {
    float playerX = gamePlayer.position.x;
    float playerY = gamePlayer.position.y;
    float turnX = gamePlayer.direction.x;
    float turnY = gamePlayer.direction.y;
    float planeX = gameCamera->cameraPlane.x;
    float planeY = gameCamera->cameraPlane.y;
    float lookAngle = gameCamera->cameraAngle;

    float moveSpeed = deltaTime * 5.0;
    float rotSpeed = deltaTime * 3.0;

    if (TC_KeyRight())
    {
        float oldDirX = turnX;
        lookAngle -= rotSpeed;
        turnX = turnX * cos(-rotSpeed) - turnY * sin(-rotSpeed);
        turnY = oldDirX * sin(-rotSpeed) + turnY * cos(-rotSpeed);
        float oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    if (TC_KeyLeft())
    {
        float oldDirX = turnX;
        lookAngle += rotSpeed;
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
        movedThisFrame = true;
    }
    if (TC_KeyDown())
    {
        targetPosition.x -= turnX * moveSpeed;
        targetPosition.y -= turnY * moveSpeed;
        movedThisFrame = true;
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
    gameCamera->cameraAngle = lookAngle;
}

void CFW_OnUpdate(float deltaTime) {
    TC_UpdateJoy(deltaTime);

    SDL_SetRenderTarget(gameWindow->renderer, gameTexture);
    //SDL_SetRenderDrawColor(gameWindow->renderer, 255, 255, 255, 255);
    //SDL_RenderClear(gameWindow->renderer);

    if (movedThisFrame) {
        movedThisFrame = false;

        bobTime += deltaTime*15.f;
        float bobAmount = (sinf(bobTime)+1.f)/2.f;
        bobAmount = flerp(-0.125f, .0f, bobAmount);
        gameCamera->verticalOffset = bobAmount;


        sideTime += deltaTime*7.5f;
        float sideAmount = sinf(sideTime);
        sideAmount = flerp(-.025f, .025f, sideAmount);
        gameCamera->horizontalOffset = sideAmount;
    } else {
        bobTime = baseBobTime;
        sideTime = baseSideTime;
        gameCamera->verticalOffset = ftoward(gameCamera->verticalOffset, .0f, deltaTime*1.f);

        gameCamera->horizontalOffset = ftoward(gameCamera->horizontalOffset, .0f, deltaTime*2.f);
    }

    TC_RenderFloorCeiling();
    TC_RenderWalls();

    int windowWidth = 0;
    int windowHeight = 0;

    SDL_GetWindowSizeInPixels(gameWindow->window, &windowWidth, &windowHeight);

    SDL_SetRenderTarget(gameWindow->renderer, NULL);
    // Update Window
    SDL_RenderCopy(gameWindow->renderer, gameTexture, NULL, NULL);
    SDL_RenderPresent(gameWindow->renderer);
}

void CFW_OnEnd(int exitCode) {
    SDL_DestroyTexture(gameTexture);
    TC_FreeMap();
    TC_CloseRenderer();
}
