#include "../include/joy.h"
#include <SDL2/SDL_keycode.h>

bool isKeyUp = false;
bool isKeyDown = false;
bool isKeyLeft = false;
bool isKeyRight = false;
bool isKeyShift = false;
bool isKeyEsc = false;
bool isMouseLeft = false;
bool isMouseMiddle = false;
bool isMouseRight = false;

Sint32 storedMouseX = 0;

void CFW_KeyDown(SDL_Event* event, float deltaTime) {
    switch(event->key.keysym.sym) {
        case SDLK_w:
            isKeyUp = true;
            break;
        case SDLK_s:
            isKeyDown = true;
            break;
        case SDLK_a:
            isKeyLeft = true;
            break;
        case SDLK_d:
            isKeyRight = true;
            break;
        case SDLK_LSHIFT:
            isKeyShift = true;
            break;
        case SDLK_ESCAPE:
            isKeyEsc = true;
            break;
    }
}
void CFW_KeyUp(SDL_Event* event, float deltaTime) {
    switch(event->key.keysym.sym) {
        case SDLK_w:
            isKeyUp = false;
            break;
        case SDLK_s:
            isKeyDown = false;
            break;
        case SDLK_a:
            isKeyLeft = false;
            break;
        case SDLK_d:
            isKeyRight = false;
            break;
        case SDLK_LSHIFT:
            isKeyShift = false;
            break;
        case SDLK_ESCAPE:
            isKeyEsc = false;
            break;
    }
}

void CFW_MouseButtonDown(SDL_Event* event, float deltaTime) {
    switch(event->button.button) {
        case 1:
            // Left Mouse Button
            isMouseLeft = true;
            break;
        case 2:
            // Middle Mouse Button
            isMouseMiddle = true;
            break;
        case 3:
            // Right Mouse Button
            isMouseRight = true;
            break;
    }
}
void CFW_MouseButtonUp(SDL_Event* event, float deltaTime) {
    switch(event->button.button) {
        case 1:
            // Left Mouse Button
            isMouseLeft = false;
            break;
        case 2:
            // Middle Mouse Button
            isMouseMiddle = false;
            break;
        case 3:
            // Right Mouse Button
            isMouseRight = false;
            break;
    }
}

void CFW_MouseMotion(SDL_Event* event, float deltaTime) {
    storedMouseX += event->motion.xrel;
}

bool TC_KeyUp() {
    return isKeyUp;
}
bool TC_KeyDown() {
    return isKeyDown;
}
bool TC_KeyLeft() {
    return isKeyLeft;
}
bool TC_KeyRight() {
    return isKeyRight;
}
bool TC_KeyShift() {
    return isKeyShift;
}
bool TC_KeyEsc() {
    return isKeyEsc;
}

bool TC_MouseLeft() {
    return isMouseLeft;
}
bool TC_MouseMiddle() {
    return isMouseMiddle;
}
bool TC_MouseRight() {
    return isMouseRight;
}

Sint32 TC_QueryMouseMotion() {
    Sint32 motion = storedMouseX;
    storedMouseX = 0;
    return motion;
}
