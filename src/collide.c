#include "../include/collide.h"

bool TC_CheckCollisionCircleSquare(Vector2 circlePosition, float circleRadius, Vector2 squarePosition, float squareSize) {
    float halfSquareSize = squareSize * .5f;
    Vector2 difference = SUBTRACT_VECTOR2(circlePosition, squarePosition);
    Vector2 clampedDistance = (Vector2){.x = clampFloat(difference.x, -halfSquareSize, halfSquareSize), .y = clampFloat(difference.y, -halfSquareSize, halfSquareSize)};
    Vector2 closestPoint = ADD_VECTOR2(squarePosition, clampedDistance);
    difference = SUBTRACT_VECTOR2(closestPoint, circlePosition);
    return (fabsf(difference.x*difference.x) + fabsf(difference.y*difference.y)) < circleRadius*circleRadius;
}
bool TC_CheckCollisionCircleCircle(Vector2 circleAPosition, float circleARadius, Vector2 circleBPosition, float circleBRadius) {
    float radiusDistance = circleARadius+circleBRadius;
    Vector2 difference = SUBTRACT_VECTOR2(circleBPosition, circleAPosition);
    return (fabsf(difference.x*difference.x) + fabsf(difference.y*difference.y)) < radiusDistance*radiusDistance;
}

bool TC_CheckTilesWithinSquare(Vector2 position, float width) {
    float halfWidth = width * .5f;
    Vector2 checkPosition = (Vector2){.x = position.x - halfWidth, .y = position.y - halfWidth};
    float checkDelta = halfWidth < 1.0 ? halfWidth : 1.0;
    Vector2i lastCheckTile = (Vector2i){.x = -255, .y = -255};
    while (checkPosition.x <= position.x + halfWidth) {
        if (((int)checkPosition.x) != lastCheckTile.x) {
            while (checkPosition.y <= position.y + halfWidth) {
                if (((int)checkPosition.y) != lastCheckTile.y) {
                    if (TC_GetMapCollision((int)checkPosition.x, (int)checkPosition.y) != 0)
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
bool TC_CheckTilesWithinCircle(Vector2 position, float radius) {
    Vector2 checkPosition = (Vector2){.x = position.x - radius, .y = position.y - radius};
    float checkDelta = radius < 1.0 ? radius : 1.0;
    Vector2i lastCheckTile = (Vector2i){.x = -255, .y = -255};
    while (checkPosition.x <= position.x + radius) {
        if (((int)checkPosition.x) != lastCheckTile.x) {
            while (checkPosition.y <= position.y + radius) {
                if (((int)checkPosition.y) != lastCheckTile.y) {

                    if (TC_GetMapCollision((int)checkPosition.x, (int)checkPosition.y) != 0 && TC_CheckCollisionCircleSquare(position, radius, checkPosition, 1.f))
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

bool TC_CheckLineIntersect(Line2 lineA, Line2 lineB) {
    float intersectionA = ((lineB.end.x-lineB.start.x)*(lineA.start.y-lineB.start.y) - (lineB.end.y-lineB.start.y)*(lineA.start.x-lineB.start.x))
                        / ((lineB.end.y-lineB.start.y)*(lineA.end.x-lineA.start.x) - (lineB.end.x-lineB.start.x)*(lineA.end.y-lineA.start.y));
    float intersectionB = ((lineA.end.x-lineA.start.x)*(lineA.start.y-lineB.start.y) - (lineA.end.y-lineA.start.y)*(lineA.start.x-lineB.start.x))
                        / ((lineB.end.y-lineB.start.y)*(lineA.end.x-lineA.start.x) - (lineB.end.x-lineB.start.x)*(lineA.end.y-lineA.start.y));
    return intersectionA >= 0.f && intersectionA <= 1.f && intersectionB >= 0.f && intersectionB <= 1.f;
}
