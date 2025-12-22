#include "../include/collide.h"

bool TC_CheckTilesWithinSquare(Vector2 position, float width) {
    float halfWidth = width * .5f;
    Vector2 checkPosition = (Vector2){.x = position.x - halfWidth, .y = position.y - halfWidth};
    float checkDelta = halfWidth < 1.0 ? halfWidth : 1.0;
    Vector2i lastCheckTile = (Vector2i){.x = -255, .y = -255};
    while (checkPosition.x <= position.x + halfWidth) {
        if (((int)checkPosition.x) != lastCheckTile.x) {
            while (checkPosition.y <= position.y + halfWidth) {
                if (((int)checkPosition.y) != lastCheckTile.y) {
                    if (TC_GetMapTile((int)checkPosition.x, (int)checkPosition.y) != 0)
                        return true;

                    lastCheckTile.y = (int)checkPosition.y;
                }

                checkPosition.y += checkDelta;
            }
            lastCheckTile.x = (int)checkPosition.x;
        }

        checkPosition.x += checkDelta;
        checkPosition.y = position.y - halfWidth;
        lastCheckTile.y = -255;
    }
    return false;
}

bool TC_CheckCollisionCircleSquare(Vector2 circlePosition, float circleRadius, Vector2 squarePosition, float squareSize) {
    float halfSquareSize = squareSize * .5f;
    Vector2 distance = SUBTRACT_VECTOR2(circlePosition, squarePosition);
    Vector2 clampedDistance = (Vector2){.x = clampFloat(distance.x, -halfSquareSize, halfSquareSize), .y = clampFloat(distance.y, -halfSquareSize, halfSquareSize)};
    Vector2 closestPoint = ADD_VECTOR2(squarePosition, clampedDistance);
    distance = SUBTRACT_VECTOR2(closestPoint, circlePosition);
    return (fabsf(distance.x*distance.x) + fabsf(distance.y*distance.y)) < circleRadius*circleRadius;
}

bool TC_CheckTilesWithinCircle(Vector2 position, float radius) {
    Vector2 checkPosition = (Vector2){.x = position.x - radius, .y = position.y - radius};
    float checkDelta = radius < 1.0 ? radius : 1.0;
    Vector2i lastCheckTile = (Vector2i){.x = -255, .y = -255};
    while (checkPosition.x <= position.x + radius) {
        if (((int)checkPosition.x) != lastCheckTile.x) {
            while (checkPosition.y <= position.y + radius) {
                if (((int)checkPosition.y) != lastCheckTile.y) {

                    if (TC_GetMapTile((int)checkPosition.x, (int)checkPosition.y) != 0 && TC_CheckCollisionCircleSquare(position, radius, checkPosition, 1.f))
                        return true;

                    lastCheckTile.y = (int)checkPosition.y;
                }

                checkPosition.y += checkDelta;
            }
            lastCheckTile.x = (int)checkPosition.x;
        }

        checkPosition.x += checkDelta;
        checkPosition.y = position.y - radius;
        lastCheckTile.y = -255;
    }
    return false;
}
