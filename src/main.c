#include "../CosmaFW/include/cfw.h"

CFW_Window* gameWindow = NULL;

bool CFW_OnStart(int argumentCount, char* arguments[]) {
    gameWindow = CFW_CreateWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN);
    if (!gameWindow)
        return false;

    SDL_FillRect(gameWindow->surface, NULL, SDL_MapRGB(gameWindow->surface->format, 0x00, 0xFF, 0x00));
    SDL_UpdateWindowSurface(gameWindow->window);
    
    return true;
}

void CFW_OnUpdate(SDL_Event *event) {

}

void CFW_OnEnd(int exitCode) {

}