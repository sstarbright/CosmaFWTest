#ifndef COLLIDE_H
#define COLLIDE_H

#include "map.h"
#include "../CosmaFW/include/cfw.h"

bool TC_CheckCollisionCircleSquare(Vector2 circlePosition, float circleRadius, Vector2 squarePosition, float squareSize);
bool TC_CheckCollisionCircleCircle(Vector2 circleAPosition, float circleARadius, Vector2 circleBPosition, float circleBRadius);

bool TC_CheckTilesWithinSquare(Vector2 position, float width);
bool TC_CheckTilesWithinCircle(Vector2 position, float radius);

#endif
