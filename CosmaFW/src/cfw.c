#include "../include/cfw.h"

const char* CFW_SDL_FAILED_MESSAGE = "oh no, can't start sdl :'(\nsdl error: ";
const char* CFW_SDL_WINDOW_FAILED_MESSAGE = "oh no, can't create a window :'( \nsdl error: ";

// First window in the CFW_Window linked list
CFW_Window* firstWindow = NULL;
bool isQuit = false;
bool isEnd = false;
int endCode = 0;

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
int CFW_Kill(int exitCode) {
    CFW_Window* currentWindow = firstWindow;
    CFW_Window* nextWindow;
    while(currentWindow) {
        nextWindow = currentWindow->next;
        if (nextWindow == currentWindow)
            nextWindow = NULL;
        SDL_DestroyWindow(currentWindow->window);
        free(currentWindow);
        currentWindow = nextWindow;
    }
    SDL_Quit();
    return exitCode;
}
int CFW_End(int exitCode) {
    if (!isEnd) {
        endCode = exitCode;
        isEnd = true;
        isQuit = true;
        CFW_OnEnd(exitCode);
        return CFW_Kill(exitCode);
    }
    return exitCode;
}

CFW_Window* CFW_CreateWindow(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, Uint32 flags) {
    if (SDL_WasInit(SDL_INIT_VIDEO)&SDL_INIT_VIDEO) {
        SDL_Window* newWindow = SDL_CreateWindow(title, windowX, windowY, windowWidth, windowHeight, flags);

        if (!newWindow) {
            const char* error = SDL_GetError();
            char buffer[strlen(error)+strlen(CFW_SDL_WINDOW_FAILED_MESSAGE)];
            strcpy(buffer, CFW_SDL_WINDOW_FAILED_MESSAGE);
            strcat(buffer, error);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
            return NULL;
        }

        CFW_Window* newCWindow = malloc(sizeof(CFW_Window));
        if (!firstWindow) {
            newCWindow->prev = newCWindow;
            newCWindow->next = newCWindow;
            firstWindow = newCWindow;
        } else {
            newCWindow->prev = firstWindow->prev;
            newCWindow->next = firstWindow;
            firstWindow->prev->next = newCWindow;
            firstWindow->prev = newCWindow;
        }
        newCWindow->window = newWindow;
        return newCWindow;
    }
    return NULL;
}

void CFW_KillWindow(CFW_Window* window) {
    if (window) {
        if (window != window->next) {
            window->next->prev = window->prev;
            window->prev->next = window->next;
        } else {
            firstWindow = NULL;
        }
        SDL_DestroyWindow(window->window);
        free(window);
    }
}

int main(int argumentCount, char* arguments[]) {
    if (!CFW_Init() || !CFW_OnStart(argumentCount, arguments))
        return CFW_Kill(1);

    SDL_Event sdlEvent;
    while(!isQuit) {
        while(SDL_PollEvent(&sdlEvent)) {
            isQuit = isQuit || sdlEvent.type == SDL_QUIT;
            CFW_OnUpdate(&sdlEvent);
        }
    }

    return CFW_End(endCode);
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