#ifndef JOY_H
#define JOY_H

#include "../CosmaFW/include/cfw.h"

bool TC_KeyUp();
bool TC_KeyDown();
bool TC_KeyLeft();
bool TC_KeyRight();
bool TC_KeyShift();
bool TC_KeyEsc();

bool TC_MouseLeft();
bool TC_MouseMiddle();
bool TC_MouseRight();

Sint32 TC_QueryMouseMotion();

#endif
