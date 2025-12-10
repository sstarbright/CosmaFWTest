#ifndef CFW_SDL_H
#define CFW_SDL_H

#ifdef _WIN32
#include <SDL2/SDL.h>
#include <windows.h>
#else
#include <SDL2/SDL.h>
#endif

#include <stdbool.h>
#include <stdio.h>
typedef struct CFW_Window CFW_Window;

struct CFW_Window {
    SDL_Window* window;
    CFW_Window* next;
    CFW_Window* prev;
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

#endif