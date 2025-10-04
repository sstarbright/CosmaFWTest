#include "../include/cfw.h"

const char* CFW_SDL_FAILED_MESSAGE = "oh no, can't start sdl :'(\nsdl error: ";
const char* CFW_SDL_WINDOW_FAILED_MESSAGE = "oh no, can't create a window :'( \nsdl error: ";

SDL_Window* mainWindow = NULL;

bool CFW_Init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        const char* error = SDL_GetError();
        char buffer[strlen(error)+strlen(CFW_SDL_FAILED_MESSAGE)];
        strcpy(buffer, CFW_SDL_FAILED_MESSAGE);
        strcat(buffer, error);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
        return false;
    }
    return true;
}
int CFW_Close(int exitCode) {
    SDL_DestroyWindow(mainWindow);
    mainWindow = NULL;
    SDL_Quit();
    return exitCode;
}

SDL_Window* CFW_CreateWindow(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, Uint32 flags) {
    if (SDL_WasInit(SDL_INIT_VIDEO)&SDL_INIT_VIDEO) {
        SDL_Window* newWindow = SDL_CreateWindow(title, windowX, windowY, windowWidth, windowHeight, flags);

        if (!newWindow) {
            const char* error = SDL_GetError();
            char buffer[strlen(error)+strlen(CFW_SDL_WINDOW_FAILED_MESSAGE)];
            strcpy(buffer, CFW_SDL_WINDOW_FAILED_MESSAGE);
            strcat(buffer, error);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
            return false;
        }
        return newWindow;
    }
    return NULL;
}
void CFW_SetMainWindow(SDL_Window* newMainWindow) {
    mainWindow = newMainWindow;
}
SDL_Window* CFW_GetMainWindow() {
    return mainWindow;
}