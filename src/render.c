#include "../include/render.h"

#define FOG_THICKNESS 4.f
#define HEIGHT_OFFSET 8

Vector2i* renderMapSize;
Vector2i screenResolution = (Vector2i){.x = 256, .y = 224};
SDL_Surface* renderSurface;
SDL_Surface* lightingSurface;

RayCamera* camera;

void TC_SetupRenderer(Vector2i* mapSizePointer, SDL_Surface* targetSurface) {
    renderMapSize = mapSizePointer;
    renderSurface = targetSurface;
    lightingSurface = SDL_CreateRGBSurface(0, 256, 224, 32, 0, 0, 0, 0);
    SDL_SetSurfaceBlendMode(lightingSurface, SDL_BLENDMODE_MUL);

    camera = malloc(sizeof(RayCamera));
    camera->cameraPosition = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraDirection = (Vector2){.x = 0.0f, .y = 0.0f};
    camera->cameraPlane = (Vector2){.x = 0.0f, .y = 0.66f};
}

void TC_RenderGeo() {
    SDL_FillRect(lightingSurface, NULL, SDL_MapRGB(renderSurface->format, 0x00, 0x00, 0x00));
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
            
        SDL_Surface* targetTexture = TC_GetMapTexture(TC_GetMapTile(mapX, mapY));

        float wallX;
        if (side == 0)
            wallX = posY + perpWallDist * rayDirY;
        else
            wallX = posX + perpWallDist * rayDirX;
        wallX -= floor(wallX);

        //x coordinate on the texture
        int textureX = (int)(wallX * (float)targetTexture->w);
        int face = 0;
        if(side == 0) {
            if (rayDirX > 0) {
                textureX = targetTexture->w - textureX - 1;
                // North
                face = 0;
            } else {
                // South
                face = 2;
            }
        }
        else if(side == 1) {
            if (rayDirY < 0) {
                textureX = targetTexture->w - textureX - 1;
                // East
                face = 1;
            } else {
                // West
                face = 3;
            }
        }

        int lineHeight = (int)(screenHeight/perpWallDist);

        float scaleRatio = (float)targetTexture->h/(float)lineHeight;

        int drawStart = -lineHeight/2+screenHeight/2 + HEIGHT_OFFSET;
        int fetchStart = 0;
        if (drawStart < 0) {
            fetchStart = -drawStart * scaleRatio;
            drawStart = 0;
        }
        int drawEnd = lineHeight/2+screenHeight/2 + HEIGHT_OFFSET;
        int fetchEnd = targetTexture->h-1;
        if (drawEnd >= screenHeight) {
            fetchEnd = targetTexture->h-((drawEnd - screenHeight) * scaleRatio);
            drawEnd = screenHeight-1;
        }

        SDL_Rect targetRect = (SDL_Rect){.x = x, .y = drawStart, .w = 1, .h = drawEnd-drawStart};
        
        SDL_BlitScaled(targetTexture, &(SDL_Rect){.x = textureX, .y = fetchStart, .w = 1, .h = fetchEnd-fetchStart}, renderSurface, &targetRect);

        float lightDistance = perpWallDist / FOG_THICKNESS;
        if (lightDistance < 0)
            lightDistance = 0;
        else if (lightDistance > 1)
            lightDistance = 1;
        lightDistance = 1.f - lightDistance;

        float wallRClamp = flerp(0.25, 1.25, wallX);
        if (wallRClamp > 1.0)
            wallRClamp = 1.0;
        float wallLClamp = flerp(1.25, 0.25, wallX);
        if (wallLClamp > 1.0)
            wallLClamp = 1.0;
        
        // NW Corner
        if (TC_CheckFaceAmbient(mapX, mapY, 0)) {
            if (face == 0)
                lightDistance *= wallRClamp;
            else if (face == 3)
                lightDistance *= wallRClamp;
        }
        // NE Corner
        if (TC_CheckFaceAmbient(mapX, mapY, 1)) {
            if (face == 0)
                lightDistance *= wallLClamp;
            else if (face == 1)
                lightDistance *= wallRClamp;
        }
        // SE Corner
        if (TC_CheckFaceAmbient(mapX, mapY, 2)) {
            if (face == 2)
                lightDistance *= wallLClamp;
            else if (face == 1)
                lightDistance *= wallLClamp;
        }
        // SW Corner
        if (TC_CheckFaceAmbient(mapX, mapY, 3)) {
            if (face == 2)
                lightDistance *= wallRClamp;
            else if (face == 3)
                lightDistance *= wallLClamp;
        }

        SDL_FillRect(lightingSurface, &targetRect, SDL_MapRGB(renderSurface->format, (int)(lightDistance*255), (int)(lightDistance*255), (int)(lightDistance*255)));
    }
    SDL_BlitSurface(lightingSurface, NULL, renderSurface, NULL);
}

RayCamera* TC_GetCamera() {
    return camera;
}

void TC_CloseRenderer() {
    free(camera);
}