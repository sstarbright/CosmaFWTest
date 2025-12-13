#include "../include/cfw.h"

const char* CFW_SDL_FAILED_MESSAGE = "oh no, can't start sdl :'(\nsdl error: ";
const char* CFW_SDL_WINDOW_FAILED_MESSAGE = "oh no, can't create a window :'( \nsdl error: ";
const char* CFW_SDL_IMAGE_FAILED_MESSAGE = "oh no, can't load an image :'( \nsdl error: ";
const char* CFW_SDL_TEXTURE_FAILED_MESSAGE = "oh no, can't upload a texture :'( \nsdl error: ";
const char* CFW_SDL_FILE_FAILED_MESSAGE = "oh no, can't load a file :'( \nsdl error: ";

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
        newCWindow->surface = SDL_GetWindowSurface(newWindow);
        newCWindow->renderer = SDL_CreateRenderer(newWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
        SDL_DestroyRenderer(window->renderer);
        SDL_DestroyWindow(window->window);
        free(window);
    }
}

CFW_Texture* CFW_CreateTexture(const char* path) {
    if (SDL_WasInit(SDL_INIT_VIDEO)&SDL_INIT_VIDEO) {
        SDL_Surface* newSurface = IMG_Load(path);

        if (!newSurface) {
            const char* error = SDL_GetError();
            char buffer[strlen(error)+strlen(CFW_SDL_IMAGE_FAILED_MESSAGE)];
            strcpy(buffer, CFW_SDL_IMAGE_FAILED_MESSAGE);
            strcat(buffer, error);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
            return NULL;
        }

        CFW_Texture* newCTexture = malloc(sizeof(CFW_Texture));
        newCTexture->surface = newSurface;
        newCTexture->texture = NULL;
        newCTexture->owners = 0;
    }
    return NULL;
}
void CFW_DestroyTexture(CFW_Texture* texture, bool individual) {
    if (texture) {
        if (texture->surface)
            SDL_FreeSurface(texture->surface);
        if (texture->texture)
            SDL_DestroyTexture(texture->texture);
        if (individual)
            free(texture);
    }
}
void CFW_ReqTexture(CFW_Texture* texture) {
    if (texture->owners == 0) {
        SDL_Texture* loadedTexture = SDL_CreateTextureFromSurface(firstWindow->renderer, (SDL_Surface*)texture->surface);

        if (!loadedTexture) {
            const char* error = SDL_GetError();
            char buffer[strlen(error)+strlen(CFW_SDL_TEXTURE_FAILED_MESSAGE)];
            strcpy(buffer, CFW_SDL_TEXTURE_FAILED_MESSAGE);
            strcat(buffer, error);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
        }
        texture->texture = loadedTexture;
    }

    texture->owners += 1;
}
void CFW_UnreqTexture(CFW_Texture* texture) {
    if (texture->owners == 1) {
        if (texture->texture) {
            SDL_DestroyTexture(texture->texture);
            texture->texture = NULL;
        }
    }
    texture->owners -= 1;
}

CFW_AnimTexture* CFW_CreateAnimTexture(const char* imagePath, const char* framePath) {
    CFW_Texture* sourceTexture = CFW_CreateTexture(imagePath);

    if (!sourceTexture)
        return NULL;
    
    SDL_Surface* sourceSurface = sourceTexture->surface;
    SDL_RWops* frameStream = DL_RWFromFile(framePath, "r");

    if (!frameStream) {
        const char* error = SDL_GetError();
        char buffer[strlen(error)+strlen(CFW_SDL_FILE_FAILED_MESSAGE)];
        strcpy(buffer, CFW_SDL_FILE_FAILED_MESSAGE);
        strcat(buffer, error);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
        return NULL;
    }

    int frameCount = 1;
    if (sizeof(int) == 4)
        frameCount = max(SDL_ReadBE32(frameStream), 1);
    else if (sizeof(int) == 2)
        frameCount = max(SDL_ReadBE16(frameStream), 1);
    
    CFW_Texture* frameSlots = malloc(sizeof(CFW_Texture) * frameCount);
    float* frameTimings = malloc(sizeof(float) * frameCount);
    void* sourcePixels = sourceSurface->pixels;
    SDL_PixelFormat* sourceFormat = sourceSurface->format;
    Uint8 sourceDepth = sourceFormat->BitsPerPixel;
    int sourcePitch = sourceSurface->pitch;
    int textureWidth = sourceSurface->w/frameCount;
    int textureHeight = sourceSurface->h;

    SDL_Rect sourceRect;
    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.w = textureWidth;
    sourceRect.h = textureHeight;

    SDL_Rect targetRect;
    targetRect.x = 0;
    targetRect.y = 0;
    targetRect.w = textureWidth;
    targetRect.h = textureHeight;

    for (int i = 0; i < frameCount; i++) {
        CFW_Texture* frameSlot = &frameSlots[i];
        Uint32 nextFrameTiming = SDL_ReadBE32(frameStream);
        memcpy((void*)&frameTimings[i], (void*)&nextFrameTiming, 4);
        frameSlot->texture = NULL;
        frameSlot->surface = SDL_CreateRGBSurfaceWithFormatFrom(sourcePixels, textureWidth, textureHeight, sourceDepth, sourcePitch, sourceFormat);
        SDL_BlitSurface(sourceSurface, &sourceRect, frameSlot->surface, &targetRect);
        sourceRect.x += textureWidth;
        frameSlot->owners = 0;
    }

    SDL_FreeSurface(sourceSurface);

    CFW_AnimTexture* animTexture = malloc(sizeof(CFW_AnimTexture));
    animTexture->frameSlots = frameSlots;
    animTexture->frameCount = frameCount;
    animTexture->frameTimings = frameTimings;

    return animTexture;
}
void CFW_DestroyAnimTexture(CFW_AnimTexture* texture) {
    int frameCount = texture->frameCount;
    for (int i = 0; i < frameCount; i++)
        CFW_DestroyTexture(&texture->frameSlots[i], false);

    free(texture->frameSlots);
    free(texture->frameTimings);
    free(texture);
}

CFW_AngleTexture* CFW_CreateAngleTexture(const char* path, int angleCount) {
    CFW_Texture* sourceTexture = CFW_CreateTexture(path);

    if (!sourceTexture)
        return NULL;
    
    SDL_Surface* sourceSurface = sourceTexture->surface;
    
    CFW_Texture* angleSlots = malloc(sizeof(CFW_Texture) * angleCount);
    void* sourcePixels = sourceSurface->pixels;
    SDL_PixelFormat* sourceFormat = sourceSurface->format;
    Uint8 sourceDepth = sourceFormat->BitsPerPixel;
    int sourcePitch = sourceSurface->pitch;
    int textureWidth = sourceSurface->w/angleCount;
    int textureHeight = sourceSurface->h;

    SDL_Rect sourceRect;
    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.w = textureWidth;
    sourceRect.h = textureHeight;

    SDL_Rect targetRect;
    targetRect.x = 0;
    targetRect.y = 0;
    targetRect.w = textureWidth;
    targetRect.h = textureHeight;

    for (int i = 0; i < angleCount; i++) {
        CFW_Texture* angleSlot = &angleSlots[i];
        angleSlot->texture = NULL;
        angleSlot->surface = SDL_CreateRGBSurfaceWithFormatFrom(sourcePixels, textureWidth, textureHeight, sourceDepth, sourcePitch, sourceFormat);
        SDL_BlitSurface(sourceSurface, &sourceRect, angleSlot->surface, &targetRect);
        sourceRect.x += textureWidth;
        angleSlot->owners = 0;
    }

    SDL_FreeSurface(sourceSurface);

    CFW_AngleTexture* angleTexture = malloc(sizeof(CFW_AngleTexture));
    angleTexture->angleSlots = angleSlots;
    angleTexture->angleCount = angleCount;

    return angleTexture;
}
void CFW_DestroyAngleTexture(CFW_AngleTexture* texture) {
    int angleCount = texture->angleCount;
    for (int i = 0; i < angleCount; i++)
        CFW_DestroyTexture(&texture->angleSlots[i], false);
    
    free(texture->angleSlots);
    free(texture);
}

CFW_AnimAngleTexture* CFW_CreateAnimAngleTexture(const char* imagePath, const char* framePath, int angleCount) {
    CFW_Texture* sourceTexture = CFW_CreateTexture(imagePath);

    if (!sourceTexture)
        return NULL;
    
    SDL_Surface* sourceSurface = sourceTexture->surface;
    SDL_RWops* frameStream = DL_RWFromFile(framePath, "r");

    if (!frameStream) {
        const char* error = SDL_GetError();
        char buffer[strlen(error)+strlen(CFW_SDL_FILE_FAILED_MESSAGE)];
        strcpy(buffer, CFW_SDL_FILE_FAILED_MESSAGE);
        strcat(buffer, error);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "uh oh!", buffer, NULL);
        return NULL;
    }

    int frameCount = 1;
    if (sizeof(int) == 4)
        frameCount = max(SDL_ReadBE32(frameStream), 1);
    else if (sizeof(int) == 2)
        frameCount = max(SDL_ReadBE16(frameStream), 1);
    
    CFW_Texture** frameSlots = malloc(sizeof(void*) * frameCount);
    float* frameTimings = malloc(sizeof(float) * frameCount);
    void* sourcePixels = sourceSurface->pixels;
    SDL_PixelFormat* sourceFormat = sourceSurface->format;
    Uint8 sourceDepth = sourceFormat->BitsPerPixel;
    int sourcePitch = sourceSurface->pitch;
    int textureWidth = sourceSurface->w/frameCount;
    int textureHeight = sourceSurface->h;

    SDL_Rect sourceRect;
    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.w = textureWidth;
    sourceRect.h = textureHeight;

    SDL_Rect targetRect;
    targetRect.x = 0;
    targetRect.y = 0;
    targetRect.w = textureWidth;
    targetRect.h = textureHeight;

    for (int x = 0; x < frameCount; x++) {
        CFW_Texture* frameSlot = malloc(sizeof(CFW_Texture) * angleCount);
        frameSlots[x] = frameSlot;
        Uint32 nextFrameTiming = SDL_ReadBE32(frameStream);
        memcpy((void*)&frameTimings[x], (void*)&nextFrameTiming, 4);

        for (int y = 0; y < angleCount; y++) {
            CFW_Texture* angleSlot = &frameSlot[y];
            angleSlot->texture = NULL;
            angleSlot->surface = SDL_CreateRGBSurfaceWithFormatFrom(sourcePixels, textureWidth, textureHeight, sourceDepth, sourcePitch, sourceFormat);
            SDL_BlitSurface(sourceSurface, &sourceRect, angleSlot->surface, &targetRect);
            sourceRect.y += textureHeight;
            angleSlot->owners = 0;
        }

        sourceRect.x += textureWidth;
        sourceRect.y = 0;
    }

    SDL_FreeSurface(sourceSurface);

    CFW_AnimAngleTexture* animTexture = malloc(sizeof(CFW_AnimAngleTexture));
    animTexture->frameSlots = frameSlots;
    animTexture->frameCount = frameCount;
    animTexture->frameTimings = frameTimings;
    animTexture->angleCount = angleCount;

    return animTexture;
}
void CFW_DestroyAnimAngleTexture(CFW_AnimAngleTexture* texture) {
    int frameCount = texture->frameCount;
    int angleCount = texture->angleCount;

    for (int x = 0; x < frameCount; x++) {
        for (int y = 0; y < angleCount; y++) {
            CFW_DestroyTexture(&texture->frameSlots[x][y], false);
        }
        free(texture->frameSlots[x]);
    }

    free(texture->frameSlots);
    free(texture->frameTimings);
    free(texture);
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