#ifndef COLLIDE_H
#define COLLIDE_H

#include "map.h"
#include "../CosmaFW/include/cfw.h"

bool TC_CheckTilesWithinSquare(Vector2 position, float width);
bool TC_CheckTilesWithinCircle(Vector2 position, float radius);

#endif
