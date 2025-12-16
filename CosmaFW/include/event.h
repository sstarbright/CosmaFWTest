#ifndef CFW_EVENT_H
#define CFW_EVENT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void CFW_AudioDeviceAdded(SDL_Event* event, float deltaTime);
void CFW_AudioDeviceRemoved(SDL_Event* event, float deltaTime);
void CFW_ControllerAxisMotion(SDL_Event* event, float deltaTime);
void CFW_ControllerButtonDown(SDL_Event* event, float deltaTime);
void CFW_ControllerButtonUp(SDL_Event* event, float deltaTime);
void CFW_ControllerDeviceAdded(SDL_Event* event, float deltaTime);
void CFW_ControllerDeviceRemoved(SDL_Event* event, float deltaTime);
void CFW_ControllerDeviceRemapped(SDL_Event* event, float deltaTime);
void CFW_DollarGesture(SDL_Event* event, float deltaTime);
void CFW_DollarRecord(SDL_Event* event, float deltaTime);
void CFW_DropFile(SDL_Event* event, float deltaTime);
void CFW_DropText(SDL_Event* event, float deltaTime);
void CFW_DropBegin(SDL_Event* event, float deltaTime);
void CFW_DropComplete(SDL_Event* event, float deltaTime);
void CFW_FingerMotion(SDL_Event* event, float deltaTime);
void CFW_FingerDown(SDL_Event* event, float deltaTime);
void CFW_FingerUp(SDL_Event* event, float deltaTime);
void CFW_KeyDown(SDL_Event* event, float deltaTime);
void CFW_KeyUp(SDL_Event* event, float deltaTime);
void CFW_JoyAxisMotion(SDL_Event* event, float deltaTime);
void CFW_JoyBallMotion(SDL_Event* event, float deltaTime);
void CFW_JoyHatMotion(SDL_Event* event, float deltaTime);
void CFW_JoyButtonDown(SDL_Event* event, float deltaTime);
void CFW_JoyButtonUp(SDL_Event* event, float deltaTime);
void CFW_JoyDeviceAdded(SDL_Event* event, float deltaTime);
void CFW_JoyDeviceRemoved(SDL_Event* event, float deltaTime);
void CFW_MouseMotion(SDL_Event* event, float deltaTime);
void CFW_MouseButtonDown(SDL_Event* event, float deltaTime);
void CFW_MouseButtonUp(SDL_Event* event, float deltaTime);
void CFW_MouseWheel(SDL_Event* event, float deltaTime);
void CFW_MultiGesture(SDL_Event* event, float deltaTime);

// Return True if we should close up
bool CFW_Quit(SDL_Event* event);
void CFW_SysWMEvent(SDL_Event* event, float deltaTime);
void CFW_TextEditing(SDL_Event* event, float deltaTime);
void CFW_TextEditing_Ext(SDL_Event* event, float deltaTime);
void CFW_TextInput(SDL_Event* event, float deltaTime);
void CFW_UserEvent(SDL_Event* event, float deltaTime);
void CFW_WindowEvent(SDL_Event* event, float deltaTime);

#endif