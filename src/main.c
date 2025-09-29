#ifdef WINDOWS
#include <SDL2/SDL.h>
#include <windows.h>
#endif

#ifdef LINUX
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <stdbool.h>
#include <stdio.h>

SDL_Window* mainWindow = NULL;
SDL_Surface* mainWindowSurface = NULL;

const char* SDL_FAILED_MESSAGE = "oh no, can't start sdl :'(\nsdl error: ";
const char* SDL_WINDOW_FAILED_MESSAGE = "oh no, can't create a window :'( \nsdl error: ";

bool Startup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        const char* error = SDL_GetError();
        char buffer[strlen(error)+strlen(SDL_FAILED_MESSAGE)];
        strcpy(buffer, SDL_FAILED_MESSAGE);
        strcat(buffer, error);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
        return false;
    }
    return true;
}

SDL_Window* InitWindow(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, Uint32 flags) {
    if (SDL_WasInit(SDL_INIT_VIDEO)&SDL_INIT_VIDEO) {
        SDL_Window* newWindow = SDL_CreateWindow(title, windowX, windowY, windowWidth, windowHeight, flags);

        if (!newWindow) {
            const char* error = SDL_GetError();
            char buffer[strlen(error)+strlen(SDL_WINDOW_FAILED_MESSAGE)];
            strcpy(buffer, SDL_WINDOW_FAILED_MESSAGE);
            strcat(buffer, error);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
            return false;
        }
        return newWindow;
    }
    return NULL;
}

int Shutdown(int exitCode) {
    SDL_DestroyWindow(mainWindow);
    mainWindow = NULL;
    SDL_Quit();
}

#ifdef WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    // Transforms pCmdLine into a list of arguments and argumentCount
    int argumentLength = wcslen(pCmdLine);
    char argString[argumentLength];
    wcstombs(argString, pCmdLine, argumentLength);

    int argumentCount = argumentLength > 0 ? 1 : 0;

    for (int i = 0; i < argumentLength; i++) {
        if (isspace(argString[i]))
            argumentCount++;
    }

    char* arguments[argumentCount];

    if (argumentCount > 0) {
        char* token = strtok(argString, " ");

        int currentArgument = 0;
        while (token != NULL) {
            arguments[currentArgument++] = token;
            token = strtok(NULL, " ");
        }
    }
#else
int main(int argumentCount, char* arguments[]) {
#endif
    for (int i = 0; i < argumentCount; i++) {
        printf("%s\n", arguments[i]);
    }
    if (!Startup())
        return Shutdown(1);
    mainWindow = InitWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN);
    if (!mainWindow)
        return Shutdown(1);
    mainWindowSurface = SDL_GetWindowSurface(mainWindow);

    SDL_FillRect(mainWindowSurface, NULL, SDL_MapRGB(mainWindowSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(mainWindow);

    SDL_Event sdlEvent;
    bool doQuit = false;
    while(!doQuit) {
        while(SDL_PollEvent(&sdlEvent)) {
            if (sdlEvent.type == SDL_QUIT)
                doQuit = true;
        }
    }

    return Shutdown(0);
}