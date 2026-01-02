#include "../CosmaFW/include/cfw.h"
#include "../include/map.h"
#include "../include/render.h"
#include "../include/player.h"
#include "../include/joy.h"
#include "../include/collide.h"
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <math.h>

#define GAME_SCREEN_X 256
#define GAME_SCREEN_Y 224
#define GAME_PIXEL_FORMAT SDL_PIXELFORMAT_RGB555

CFW_Window* gameWindow = NULL;
CFW_Window* viewWindow = NULL;
SDL_Texture* gameTexture = NULL;
SDL_Surface* gameSurface;

Vector2i gameResolution = (Vector2i){.x = GAME_SCREEN_X, .y = GAME_SCREEN_Y};
int gamePixelBpp = SDL_BITSPERPIXEL(GAME_PIXEL_FORMAT);
int gamePixelBypp = SDL_BYTESPERPIXEL(GAME_PIXEL_FORMAT);
SDL_PixelFormat* pixelFormat;

RayCamera* gameCamera = NULL;
PlayerData gamePlayer;
float mouseSensitivity = .25f;
bool trackMouse = true;

float bobTime = 0.f;
float sideTime = 0.f;
float baseBobTime = 0.f;
float baseSideTime = 0.f;
bool movedThisFrame = false;
float moveSpeed = .0f;

bool CFW_OnStart(int argumentCount, char* arguments[]) {
    gameWindow = CFW_CreateWindow("CosmaFW Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1440, 1080,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        false,
        0
    );
    viewWindow = CFW_CreateWindow("RENDERER",
        0, 0,
        gameResolution.x, gameResolution.y,
        SDL_WINDOW_HIDDEN,
        true,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE
    );
    pixelFormat = SDL_AllocFormat(GAME_PIXEL_FORMAT);
    gameSurface = SDL_CreateRGBSurface(0, gameResolution.x, gameResolution.y, pixelFormat->BitsPerPixel, pixelFormat->Rmask, pixelFormat->Gmask, pixelFormat->Bmask, pixelFormat->Amask);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_RaiseWindow(gameWindow->window);
    if (!viewWindow->renderer) {
        printf("NO RENDERER FOUND!!!\n");
        return false;
    }
    if (SDL_RenderTargetSupported(viewWindow->renderer) != SDL_TRUE) {
        printf("CANNOT RENDER TO TARGET TEXTURE!!!\n");
        return false;
    }
    gameTexture = SDL_CreateTexture(viewWindow->renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, gameResolution.x, gameResolution.y);
    if (!gameTexture) {
        printf("NO GAME RENDER TARGET!!!\n");
        return false;
    }

    Line2 testLine = (Line2){.start = (Vector2){.x = 0.f, .y = 0.f}, .end = (Vector2){.x = 0.49f, .y = 1.f}};
    Line2 testLine2 = (Line2){.start = (Vector2){.x = 1.f, .y = 0.f}, .end = (Vector2){.x = 0.51f, .y = 1.f}};

    if (TC_CheckLineIntersect(testLine, testLine2)) {
        printf("THESE LINES INTERSECT!\n");
    }

    TC_InitializeMap();
    TC_SetupRenderer(TC_GetMapSizePointer(), viewWindow, gameTexture);
    gameCamera = TC_GetCamera();

    gamePlayer.position = (Vector2){.x = 9.5f, .y = 9.5f};
    gamePlayer.direction = (Vector2){.x = -1.0f, .y = 0.0f};
    gamePlayer.radius = 0.25;

    baseBobTime = asin(1.f);
    baseSideTime = asin(0.f);

    return true;
}

void TC_UpdateJoy(float deltaTime) {
    if (TC_MouseLeft() && !trackMouse) {
        trackMouse = true;
        SDL_SetRelativeMouseMode(SDL_TRUE);
        TC_QueryMouseMotion();
    }
    if (TC_KeyEsc()) {
        trackMouse = false;
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
    float playerX = gamePlayer.position.x;
    float playerY = gamePlayer.position.y;
    float turnX = gamePlayer.direction.x;
    float turnY = gamePlayer.direction.y;
    float planeX = gameCamera->cameraPlane.x;
    float planeY = gameCamera->cameraPlane.y;
    float lookAngle = gameCamera->cameraAngle;

    moveSpeed = TC_KeyShift() ? 3.75f : 2.5f;
    float moveDelta = deltaTime * moveSpeed;

    if (trackMouse) {
        Sint32 mouseMotion = -TC_QueryMouseMotion();
        float rotSpeed = deltaTime * mouseSensitivity * mouseMotion;

        if (fabs(rotSpeed) > 0)
        {
            float oldDirX = turnX;
            lookAngle += rotSpeed;
            turnX = turnX * cos(rotSpeed) - turnY * sin(rotSpeed);
            turnY = oldDirX * sin(rotSpeed) + turnY * cos(rotSpeed);
            float oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
    }

    Vector2 targetPosition = (Vector2){.x = playerX, .y = playerY};

    if(TC_KeyUp()) {
        targetPosition.x += turnX * moveDelta;
        targetPosition.y += turnY * moveDelta;
        movedThisFrame = true;
    }
    if (TC_KeyDown())
    {
        targetPosition.x -= turnX * moveDelta;
        targetPosition.y -= turnY * moveDelta;
        movedThisFrame = true;
    }

    if(TC_KeyLeft()) {
        targetPosition.x -= planeX * moveDelta;
        targetPosition.y -= planeY * moveDelta;
        movedThisFrame = true;
    }
    if (TC_KeyRight())
    {
        targetPosition.x += planeX * moveDelta;
        targetPosition.y += planeY * moveDelta;
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

    SDL_SetRenderTarget(viewWindow->renderer, gameTexture);
    SDL_SetRenderDrawColor(viewWindow->renderer, 50, 50, 50, 255);
    SDL_RenderClear(viewWindow->renderer);

    if (movedThisFrame) {
        movedThisFrame = false;

        bobTime += deltaTime*3.75f*moveSpeed;
        float bobAmount = (sinf(bobTime)+1.f)/2.f;
        bobAmount = flerp(-0.1f, .0f, bobAmount);
        gameCamera->verticalOffset = bobAmount;


        sideTime += deltaTime*1.75f*moveSpeed;
        float sideAmount = sinf(sideTime);
        sideAmount = flerp(-.0275f, .0275f, sideAmount);
        gameCamera->horizontalOffset = sideAmount;
    } else {
        bobTime = baseBobTime;
        sideTime = baseSideTime;
        gameCamera->verticalOffset = ftoward(gameCamera->verticalOffset, .0f, deltaTime*moveSpeed*.2f);
        gameCamera->horizontalOffset = ftoward(gameCamera->horizontalOffset, .0f, deltaTime*moveSpeed*.4f);
    }

    TC_RenderFloorCeiling();
    TC_RenderWalls();
    TC_RenderViewport();

    SDL_SetRenderTarget(viewWindow->renderer, gameTexture);
    // Update Window
    SDL_RenderReadPixels(viewWindow->renderer, NULL, GAME_PIXEL_FORMAT, gameSurface->pixels, gameSurface->pitch);
    SDL_RenderPresent(viewWindow->renderer);
    SDL_BlitScaled(gameSurface, NULL, gameWindow->surface, NULL);
    SDL_UpdateWindowSurface(gameWindow->window);
}

void CFW_WindowEvent(SDL_Event* event, float deltaTime) {
    switch (((SDL_WindowEvent*)&event)->event) {
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            printf("FOCUS GAINED\n");
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            printf("FOCUS LOST\n");
            break;
    }
}

void CFW_OnEnd(int exitCode) {
    SDL_FreeFormat(pixelFormat);
    SDL_DestroyTexture(gameTexture);
    SDL_FreeSurface(gameSurface);
    TC_FreeMap();
    TC_CloseRenderer();
}
