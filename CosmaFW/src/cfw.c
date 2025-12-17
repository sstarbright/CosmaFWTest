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
Uint64 lastTime;
Uint64 currentTime;
float deltaTime = 0.0f;

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
    SDL_RWops* frameStream = SDL_RWFromFile(framePath, "r");

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
        frameCount = SDL_ReadBE32(frameStream);
    else if (sizeof(int) == 2)
        frameCount = SDL_ReadBE16(frameStream);
    if (frameCount < 1)
        frameCount = 1;
    
    CFW_Texture* frameSlots = malloc(sizeof(CFW_Texture) * frameCount);
    float* frameTimings = malloc(sizeof(float) * frameCount);
    void* sourcePixels = sourceSurface->pixels;
    Uint32 sourceFormat = sourceSurface->format->format;
    Uint8 sourceDepth = sourceSurface->format->BitsPerPixel;
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
    Uint32 sourceFormat = sourceSurface->format->format;
    Uint8 sourceDepth = sourceSurface->format->BitsPerPixel;
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
    SDL_RWops* frameStream = SDL_RWFromFile(framePath, "r");

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
        frameCount = SDL_ReadBE32(frameStream);
    else if (sizeof(int) == 2)
        frameCount = SDL_ReadBE16(frameStream);
    if (frameCount < 1)
        frameCount = 1;
    
    CFW_Texture** frameSlots = malloc(sizeof(void*) * frameCount);
    float* frameTimings = malloc(sizeof(float) * frameCount);
    void* sourcePixels = sourceSurface->pixels;
    Uint32 sourceFormat = sourceSurface->format->format;
    Uint8 sourceDepth = sourceSurface->format->BitsPerPixel;
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

float flerp(float from, float to, float weight) {
    return from + (to - from) * weight;
}

int main(int argumentCount, char* arguments[]) {
    if (!CFW_Init() || !CFW_OnStart(argumentCount, arguments))
        return CFW_Kill(1);

    SDL_Event event;

    currentTime = 0;
    memcpy(&lastTime, &currentTime, sizeof(Uint64));

    while(!isQuit) {
        currentTime = SDL_GetTicks64();
        int timeDiff = currentTime-lastTime;
        float timeDiffFloat = (float)timeDiff;
        deltaTime = ((float)timeDiff)/1000.0f;

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                #ifdef CFW_EVENT_AUDIODEVICEADDED
                case SDL_AUDIODEVICEADDED:
                    CFW_AudioDeviceAdded(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_AUDIODEVICEREMOVED
                case SDL_AUDIODEVICEREMOVED:
                    CFW_AudioDeviceRemoved(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_CONTROLLERAXISMOTION
                case SDL_CONTROLLERAXISMOTION:
                    CFW_ControllerAxisMotion(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_CONTROLLERBUTTONDOWN
                case SDL_CONTROLLERBUTTONDOWN:
                    CFW_ControllerButtonDown(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_CONTROLLERBUTTONUP
                case SDL_CONTROLLERBUTTONUP:
                    CFW_ControllerButtonUp(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_CONTROLLERDEVICEADDED
                case SDL_CONTROLLERDEVICEADDED:
                    CFW_ControllerDeviceAdded(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_CONTROLLERDEVICEREMOVED
                case SDL_CONTROLLERDEVICEREMOVED:
                    CFW_ControllerDeviceRemoved(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_CONTROLLERDEVICEREMAPPED
                case SDL_CONTROLLERDEVICEREMAPPED:
                    CFW_ControllerDeviceRemapped(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_DOLLARGESTURE
                case SDL_DOLLARGESTURE:
                    CFW_DollarGesture(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_DOLLARRECORD
                case SDL_DOLLARRECORD:
                    CFW_DollarRecord(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_DROPFILE
                case SDL_DROPFILE:
                    CFW_DropFile(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_DROPTEXT
                case SDL_DROPTEXT:
                    CFW_DropText(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_DROPBEGIN
                case SDL_DROPBEGIN:
                    CFW_DropBegin(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_DROPCOMPLETE
                case SDL_DROPCOMPLETE:
                    CFW_DropComplete(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_FINGERMOTION
                case SDL_FINGERMOTION:
                    CFW_FingerMotion(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_FINGERDOWN
                case SDL_FINGERDOWN:
                    CFW_FingerDown(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_FINGERUP
                case SDL_FINGERUP:
                    CFW_FingerUp(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_KEYDOWN
                case SDL_KEYDOWN:
                    CFW_KeyDown(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_KEYUP
                case SDL_KEYUP:
                    CFW_KeyUp(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYAXISMOTION
                case SDL_JOYAXISMOTION:
                    CFW_JoyAxisMotion(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYBALLMOTION
                case SDL_JOYBALLMOTION:
                    CFW_JoyBallMotion(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYHATMOTION
                case SDL_JOYHATMOTION:
                    CFW_JoyHatMotion(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYBUTTONDOWN
                case SDL_JOYBUTTONDOWN:
                    CFW_JoyButtonDown(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYBUTTONUP
                case SDL_JOYBUTTONUP:
                    CFW_JoyButtonUp(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYDEVICEADDED
                case SDL_JOYDEVICEADDED:
                    CFW_JoyDeviceAdded(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_JOYDEVICEREMOVED
                case SDL_JOYDEVICEREMOVED:
                    CFW_JoyDeviceRemoved(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_MOUSEMOTION
                case SDL_MOUSEMOTION:
                    CFW_MouseMotion(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_MOUSEBUTTONDOWN
                case SDL_MOUSEBUTTONDOWN:
                    CFW_MouseButtonDown(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_MOUSEBUTTONUP
                case SDL_MOUSEBUTTONUP:
                    CFW_MouseButtonUp(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_MOUSEWHEEL
                case SDL_MOUSEWHEEL:
                    CFW_MouseWheel(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_MULTIGESTURE
                case SDL_MULTIGESTURE:
                    CFW_MultiGesture(&event, deltaTime);
                    break;
                #endif
                case SDL_QUIT:
                    #ifdef CFW_EVENT_QUIT
                    if (CFW_Quit(&event));
                        isQuit = true;
                    #endif
                    #ifndef CFW_EVENT_QUIT
                    isQuit = true;
                    #endif
                    break;
                #ifdef CFW_EVENT_SYSWMEVENT
                case SDL_SYSWMEVENT:
                    CFW_SysWMEvent(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_TEXTEDITING
                case SDL_TEXTEDITING:
                    CFW_TextEditing(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_TEXTEDITING_EXT
                case SDL_TEXTEDITING_EXT:
                    CFW_TextEditing_Ext(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_TEXTINPUT
                case SDL_TEXTINPUT:
                    CFW_TextInput(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_USEREVENT
                case SDL_USEREVENT:
                    CFW_UserEvent(&event, deltaTime);
                    break;
                #endif
                #ifdef CFW_EVENT_WINDOWEVENT
                case SDL_WINDOWEVENT:
                    CFW_WindowEvent(&event, deltaTime);
                    break;
                #endif
            }
        }

        CFW_OnUpdate(deltaTime);

        memcpy(&lastTime, &currentTime, sizeof(Uint32));
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