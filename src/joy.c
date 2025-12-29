#include "../include/joy.h"

bool isKeyUp = false;
bool isKeyDown = false;
bool isKeyLeft = false;
bool isKeyRight = false;
bool isKeyShift = false;

void CFW_KeyDown(SDL_Event* event, float deltaTime) {
    switch(event->key.keysym.sym) {
        case SDLK_UP:
            isKeyUp = true;
            break;
        case SDLK_DOWN:
            isKeyDown = true;
            break;
        case SDLK_LEFT:
            isKeyLeft = true;
            break;
        case SDLK_RIGHT:
            isKeyRight = true;
            break;
        case SDLK_LSHIFT:
            isKeyShift = true;
            break;
    }
}

void CFW_KeyUp(SDL_Event* event, float deltaTime) {
    switch(event->key.keysym.sym) {
        case SDLK_UP:
            isKeyUp = false;
            break;
        case SDLK_DOWN:
            isKeyDown = false;
            break;
        case SDLK_LEFT:
            isKeyLeft = false;
            break;
        case SDLK_RIGHT:
            isKeyRight = false;
            break;
        case SDLK_LSHIFT:
            isKeyShift = false;
            break;
    }
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
