#include "../include/collide.h"

bool TC_CheckTilesWithinSquare(Vector2 position, float width) {
    float halfWidth = width * .5f;
    Vector2 checkPosition = (Vector2){.x = position.x - halfWidth, .y = position.y - halfWidth};
    float checkDelta = halfWidth < 1.0 ? halfWidth : 1.0;
    Vector2i lastCheckTile = (Vector2i){.x = -255, .y = -255};
    while (checkPosition.x <= position.x + halfWidth) {
        while (checkPosition.y <= position.y + halfWidth) {
            if (TC_GetMapTile((int)checkPosition.x, (int)checkPosition.y) != 0)
                return true;

            checkPosition.y += checkDelta;
        }

        checkPosition.x += checkDelta;
        checkPosition.y = position.y - halfWidth;
    }
    return false;
}
