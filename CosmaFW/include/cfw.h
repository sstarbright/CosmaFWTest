#ifndef CFW_SDL_H
#define CFW_SDL_H

#ifdef _WIN32
#include <SDL.h>
#include <windows.h>
#else
#include <SDL2/SDL.h>
#endif

#include <stdbool.h>
#include <stdio.h>

bool CFW_Init();
int CFW_Close(int exitCode);

SDL_Window* CFW_CreateWindow(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, Uint32 flags);
void CFW_SetMainWindow(SDL_Window* newMainWindow);
SDL_Window* CFW_GetMainWindow();

#endif