#include "../CosmaFW/include/cfw.h"

CFW_Window* gameWindow = NULL;
SDL_Window* gameSdlWindow = NULL;
SDL_Surface* gameWindowSurface = NULL;

bool CFW_OnStart(int argumentCount, char* arguments[]) {
    gameWindow = CFW_CreateWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN);
    if (!gameWindow)
        return false;
    gameSdlWindow = gameWindow->window;
    gameWindowSurface = SDL_GetWindowSurface(gameSdlWindow);

    SDL_FillRect(gameWindowSurface, NULL, SDL_MapRGB(gameWindowSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(gameSdlWindow);
    
    return true;
}

void CFW_OnUpdate(SDL_Event *event) {

}

void CFW_OnEnd(int exitCode) {

}