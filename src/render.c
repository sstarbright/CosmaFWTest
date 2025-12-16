#include "../include/render.h"

Vector2i* renderMapSize;
Vector2i screenResolution = (Vector2i){.x = 256, .y = 224};
SDL_Surface* renderSurface;

RayCamera* camera;

void TC_SetupRenderer(Vector2i* mapSizePointer, SDL_Surface* targetSurface) {
    renderMapSize = mapSizePointer;
    renderSurface = targetSurface;

    camera = malloc(sizeof(RayCamera));
    camera->cameraPosition = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraDirection = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraPlane = (Vector2){.x = 0.0f, .y = 0.66f};
}

void TC_RenderGeo() {
    float dirX = camera->cameraDirection.x;
    float dirY = camera->cameraDirection.y;
    float planeX = camera->cameraPlane.x;
    float planeY = camera->cameraPlane.y;
    float posX = camera->cameraPosition.x;
    float posY = camera->cameraPosition.y;
    int screenWidth = screenResolution.x;
    int screenHeight = screenResolution.y;
    float screenWidthF = screenResolution.x;
    float screenHeightF = screenResolution.y;
    for (int x = 0; x < screenWidth; x++) {
        float cameraX = 2 * x / ((float)screenWidth) - 1;

        float rayDirX = dirX + planeX * cameraX;
        float rayDirY = dirY + planeY * cameraX;

        int mapX = (int)posX;
        int mapY = (int)posY;

        float sideDistX;
        float sideDistY;

        float deltaDistX = rayDirX == 0.0f ? 1e30 : fabs(1.0f / rayDirX);
        float deltaDistY = rayDirY == 0.0f ? 1e30 : fabs(1.0f / rayDirY);

        float perpWallDist;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0.0f) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0.0f) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (TC_GetMapTile(mapX, mapY) > 0)
                hit = 1;
        }

        perpWallDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);

        int lineHeight = (int)(screenHeight/perpWallDist);

        int drawStart = -lineHeight/2+screenHeight/2;
        if (drawStart < 0)
            drawStart = 0;
        int drawEnd = lineHeight/2+screenHeight/2;
        if (drawEnd >= screenHeight)
            drawEnd = screenHeight-1;
        CFW_Color color;
        switch(TC_GetMapTile(mapX, mapY)) {
            case 1:
                if (side == 0)
                    color = (CFW_Color){.r = 0xFF, .g = 0x00, .b = 0x00};
                else
                    color = (CFW_Color){.r = 0x7F, .g = 0x00, .b = 0x00};
                break;
            case 2:
                if (side == 0)
                    color = (CFW_Color){.r = 0x00, .g = 0xFF, .b = 0x00};
                else
                    color = (CFW_Color){.r = 0x00, .g = 0x7F, .b = 0x00};
                break;
            case 3:
                if (side == 0)
                    color = (CFW_Color){.r = 0x00, .g = 0x00, .b = 0xFF};
                else
                    color = (CFW_Color){.r = 0x00, .g = 0x00, .b = 0x7F};
                break;
            case 4:
                if (side == 0)
                    color = (CFW_Color){.r = 0xFF, .g = 0xFF, .b = 0xFF};
                else
                    color = (CFW_Color){.r = 0x7F, .g = 0x7F, .b = 0x7F};
                break;
            default:
                if (side == 0)
                    color = (CFW_Color){.r = 0xFF, .g = 0x00, .b = 0xFF};
                else
                    color = (CFW_Color){.r = 0x7F, .g = 0x00, .b = 0x7F};
                break;
        }
        SDL_FillRect(renderSurface, &(SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart}, SDL_MapRGB(renderSurface->format, color.r, color.g, color.b));
    }
}

RayCamera* TC_GetCamera() {
    return camera;
}

void TC_CloseRenderer() {
    free(camera);
}