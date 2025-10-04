#include "../CosmaFW/include/cfw.h"

SDL_Window* gameWindow = NULL;
SDL_Surface* gameWindowSurface = NULL;

int main(int argumentCount, char* arguments[]) {
    if (!CFW_Init())
        return CFW_Close(1);
    gameWindow = CFW_CreateWindow("CosmaFW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 1080, SDL_WINDOW_SHOWN);
    CFW_SetMainWindow(gameWindow);
    if (!gameWindow)
        return CFW_Close(1);
    gameWindowSurface = SDL_GetWindowSurface(gameWindow);

    SDL_FillRect(gameWindowSurface, NULL, SDL_MapRGB(gameWindowSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(gameWindow);

    SDL_Event sdlEvent;
    bool doQuit = false;
    while(!doQuit) {
        while(SDL_PollEvent(&sdlEvent)) {
            if (sdlEvent.type == SDL_QUIT)
                doQuit = true;
        }
    }

    return CFW_Close(0);
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
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

    return main(argumentCount, arguments);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    int argumentLength = strlen(lpCmdLine);
    int argumentCount = argumentLength > 0 ? 1 : 0;

    for (int i = 0; i < argumentLength; i++) {
        if (isspace(lpCmdLine[i]))
            argumentCount++;
    }

    char* arguments[argumentCount];

    if (argumentCount > 0) {
        char* token = strtok(lpCmdLine, " ");

        int currentArgument = 0;
        while (token != NULL) {
            arguments[currentArgument++] = token;
            token = strtok(NULL, " ");
        }
    }

    return main(argumentCount, arguments);
}
#endif