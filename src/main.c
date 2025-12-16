#include "../CosmaFW/include/cfw.h"
#include "../include/map.h"
#include "../include/render.h"
#include "../include/player.h"
#include "../include/joy.h"

CFW_Window* gameWindow = NULL;
SDL_Surface* gameSurface = NULL;
RayCamera* gameCamera = NULL;
PlayerData gamePlayer;

bool CFW_OnStart(int argumentCount, char* arguments[]) {
    gameWindow = CFW_CreateWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN);
    SDL_RaiseWindow(gameWindow->window);
    gameSurface = SDL_CreateRGBSurface(0, 256, 224, 32, 0, 0, 0, 0);
    if (!gameSurface) {
        printf("NO GAME SURFACE!!!\n");
    }
    TC_SetupRenderer(TC_GetMapSizePointer(), gameSurface);
    gameCamera = TC_GetCamera();
    if (!gameWindow)
        return false;
    
    gamePlayer.position = (Vector2){.x = 22.0f, .y = 12.0f};
    gamePlayer.direction = (Vector2){.x = -1.0f, .y = 0.0f};
    
    return true;
}

void CFW_OnUpdate(float deltaTime) {
    float playerX = gamePlayer.position.x;
    float playerY = gamePlayer.position.y;
    float turnX = gamePlayer.direction.x;
    float turnY = gamePlayer.direction.y;
    float planeX = gameCamera->cameraPlane.x;
    float planeY = gameCamera->cameraPlane.y;

    float moveSpeed = deltaTime * 5.0;
    float rotSpeed = deltaTime * 3.0;
    
    if(TC_KeyUp()) {
        if(TC_GetMapTile((int)(playerX + turnX * moveSpeed),(int)(playerY)) == false) {
            playerX += turnX * moveSpeed;
        }
        if(TC_GetMapTile((int)(playerX),(int)(playerY + turnY * moveSpeed)) == false) {
            playerY += turnY * moveSpeed;
        }
    }
    if (TC_KeyDown())
    {
      if(TC_GetMapTile((int)(playerX - turnX * moveSpeed),(int)(playerY)) == false)
        playerX -= turnX * moveSpeed;
      if(TC_GetMapTile((int)(playerX),(int)(playerY - turnY * moveSpeed)) == false)
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

    // Draw Ceiling and Floor gradients here
    SDL_FillRect(gameSurface, &(SDL_Rect){.x = 0, .y = 0, .w = 256, .h = 224}, SDL_MapRGB(gameSurface->format, 0x00, 0x00, 0x00));
    TC_RenderGeo();
    
    SDL_BlitScaled(gameSurface, NULL, gameWindow->surface, &(SDL_Rect){.x = 0, .y = 0, .w = 1440, .h = 1080});

    SDL_UpdateWindowSurface(gameWindow->window);
}

void CFW_OnEnd(int exitCode) {
    SDL_FreeSurface(gameSurface);
    TC_CloseRenderer();
}