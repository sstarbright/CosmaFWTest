#ifndef CFW_SDL_H
#define CFW_SDL_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdbool.h>
#include <stdio.h>
typedef struct CFW_Window CFW_Window;
typedef struct CFW_Texture CFW_Texture;
typedef struct CFW_AnimTexture CFW_AnimTexture;
typedef struct CFW_AngleTexture CFW_AngleTexture;
typedef struct CFW_AnimAngleTexture CFW_AnimAngleTexture;

struct CFW_Window {
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Renderer* renderer;
    CFW_Window* next;
    CFW_Window* prev;
};

struct CFW_Texture {
    SDL_Texture* texture;
    SDL_Surface* surface;
    int owners;
};
struct CFW_AnimTexture {
    CFW_Texture* frameSlots;
    int frameCount;
    float* frameTimings;
};
struct CFW_AngleTexture {
    CFW_Texture* angleSlots;
    int angleCount;
};
struct CFW_AnimAngleTexture {
    CFW_Texture** frameSlots;
    int frameCount;
    int angleCount;
    float* frameTimings;
};


// Initializes CosmaFW
// Prints a message box on SDL failure
bool CFW_Init();
// Cleans up resources and kills the program
int CFW_Kill(int exitCode);
// Calls the OnEnd function and kills CFW
int CFW_End(int exitCode);


// Function that gets called when CFW starts up
// Must be defined by user
// Return true if program should continue, or false if program should quit
bool CFW_OnStart(int argumentCount, char* arguments[]);
// Function that gets called when CFW updates
// Must be defined by user
void CFW_OnUpdate(SDL_Event *event);
// Function that gets called when CFW ends
// Must be defined by user
void CFW_OnEnd(int exitCode);


// Creates an SDL Window and adds it to a linked list
// Prints a message box on Window failure
// Don't worry about killing windows when "CFW_End()"ing your game
CFW_Window* CFW_CreateWindow(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, Uint32 flags);
// Kills an SDL Window
// Removes it from CFW's window linked list, destroys the SDL Window, and frees itself
void CFW_KillWindow(CFW_Window* window);


// Creates an SDL Surface that can be automatically loaded/unloaded in VRAM on command
CFW_Texture* CFW_CreateTexture(const char* path);
// Destroy a Texture (indicate if the CFW_Texture is an individual texture and should be freed)
void CFW_DestroyTexture(CFW_Texture* texture, bool individual);
// Request a Texture to be uploaded to VRAM if it isn't already
void CFW_ReqTexture(CFW_Texture* texture);
// Retract a request for a Texture, automatically removing from VRAM if nothing else is currently requesting it
void CFW_UnreqTexture(CFW_Texture* texture);

// Creates an array of SDL Surfaces, for each animation frame, that can be automatically loaded/unloaded in VRAM on command
CFW_AnimTexture* CFW_CreateAnimTexture(const char* imagePath, const char* framePath);
// Destroy an Animation Texture
void CFW_DestroyAnimTexture(CFW_AnimTexture* texture);

// Creates an array of SDL Surfaces, for each angle, that can be automatically loaded/unloaded in VRAM on command
CFW_AngleTexture* CFW_CreateAngleTexture(const char* path, int angleCount);
// Destroy an Angle Texture
void CFW_DestroyAngleTexture(CFW_AngleTexture* texture);

// Creates a 2D array of SDL Surfaces, x for animation frame and y for angle, that can be automatically loaded/unloaded in VRAM on command
CFW_AnimAngleTexture* CFW_CreateAnimAngleTexture(const char* imagePath, const char* framePath, int angleCount);
// Destroy an Animation Angle Texture
void CFW_DestroyAnimAngleTexture(CFW_AnimAngleTexture* texture);

#endif